// handler.cpp
#include "handler.h"
#include <mupdf/fitz.h>
#include <iostream>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#include <io.h>

// Функция для корректного открытия файлов с Unicode-путями в Windows
FILE* open_file_utf8(const std::string& path, const char* mode) {
    // Пробуем несколько вариантов кодировок
    
    // Вариант 1: UTF-8
    int wsize = MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, NULL, 0);
    if (wsize > 0) {
        std::wstring wpath(wsize, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, &wpath[0], wsize);
        
        std::wstring wmode = L"rb";
        if (mode[0] == 'w') wmode = L"wb";
        
        FILE* file = _wfopen(wpath.c_str(), wmode.c_str());
        if (file) return file;
    }
    
    // Вариант 2: CP1251 (кириллица)
    wsize = MultiByteToWideChar(1251, 0, path.c_str(), -1, NULL, 0);
    if (wsize > 0) {
        std::wstring wpath(wsize, L'\0');
        MultiByteToWideChar(1251, 0, path.c_str(), -1, &wpath[0], wsize);
        
        std::wstring wmode = L"rb";
        if (mode[0] == 'w') wmode = L"wb";
        
        FILE* file = _wfopen(wpath.c_str(), wmode.c_str());
        if (file) return file;
    }
    
    // Вариант 3: Попробуем как есть (может сработать в некоторых случаях)
    return fopen(path.c_str(), mode);
}
#endif

// Функция для проверки существования файла
bool file_exists(const std::string& filename) {
#ifdef _WIN32
    FILE* file = open_file_utf8(filename, "rb");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
#else
    std::ifstream file(filename);
    return file.good();
#endif
}

int PdfConvertHandler(const PdfConvertDto& dto) {
    // // === ОТЛАДОЧНЫЙ ВЫВОД В НАЧАЛЕ ===
    // std::cout << "=== DEBUG INFORMATION ===" << std::endl;
    // std::cout << "Input file path: '" << dto.inputFile << "'" << std::endl;
    // std::cout << "Output file path: '" << dto.outputFile << "'" << std::endl;
    // std::cout << "Page number: " << dto.pageNumber << std::endl;
    // std::cout << "DPI: " << dto.dpi << std::endl;
    
    // // Проверяем существование файла
    // bool exists = file_exists(dto.inputFile);
    // std::cout << "Input file exists: " << (exists ? "YES" : "NO") << std::endl;
    
    // // Выводим длину пути
    // std::cout << "Input file path length: " << dto.inputFile.length() << " characters" << std::endl;
    
    // // Выводим байты пути (для анализа кодировки)
    // std::cout << "Input file path bytes: ";
    // for (size_t i = 0; i < dto.inputFile.length() && i < 50; ++i) {
    //     printf("%02X ", (unsigned char)dto.inputFile[i]);
    // }
    // std::cout << std::endl;
    // std::cout << "=== END DEBUG ===" << std::endl;

    fz_context* ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
    if (!ctx) {
        std::cerr << "Failed to create MuPDF context" << std::endl;
        return EXIT_FAILURE;
    }

    fz_register_document_handlers(ctx);

    fz_document* doc = NULL;
    fz_pixmap* pix = NULL;
    fz_stream* stream = NULL;
    fz_buffer* buffer = NULL;
    int result = EXIT_FAILURE;
    std::string resultStr;

    fz_try(ctx) {
        // Открываем входной файл
#ifdef _WIN32
        FILE* file_in = open_file_utf8(dto.inputFile, "rb");
        if (!file_in) {
            std::cerr << "Error: Cannot open input file in any encoding" << std::endl;
            fz_throw(ctx, FZ_ERROR_GENERIC, "Cannot open input file: %s", dto.inputFile.c_str());
        }
        stream = fz_open_file_ptr_no_close(ctx, file_in);
#else
        stream = fz_open_file(ctx, dto.inputFile.c_str());
#endif
        
        if (!stream) {
            fz_throw(ctx, FZ_ERROR_GENERIC, "Cannot create stream from: %s", dto.inputFile.c_str());
        }

        // Открываем PDF из stream
        doc = fz_open_document_with_stream(ctx, "application/pdf", stream);
        if (!doc) {
            fz_throw(ctx, FZ_ERROR_GENERIC, "Failed to open document: %s", dto.inputFile.c_str());
        }

        // Рендерим страницу
        fz_page* page = fz_load_page(ctx, doc, dto.pageNumber - 1);
        fz_matrix transform = fz_scale(dto.dpi / 72.0f, dto.dpi / 72.0f);
        pix = fz_new_pixmap_from_page(ctx, page, transform, fz_device_rgb(ctx), 0);
        fz_drop_page(ctx, page);

        // Сохраняем в буфер памяти
        buffer = fz_new_buffer(ctx, 1024 * 1024);
        fz_output* out = fz_new_output_with_buffer(ctx, buffer);
        
        fz_write_pixmap_as_jpeg(ctx, out, pix, 90, 0);
        fz_drop_output(ctx, out);

        // Записываем буфер в файл
        unsigned char* data;
        size_t size = fz_buffer_storage(ctx, buffer, &data);
        
#ifdef _WIN32
        FILE* file_out = open_file_utf8(dto.outputFile, "wb");
        if (!file_out) {
            std::cerr << "Error: Cannot create output file in any encoding" << std::endl;
            fz_throw(ctx, FZ_ERROR_GENERIC, "Cannot create output file: %s", dto.outputFile.c_str());
        }
#else
        FILE* file_out = fopen(dto.outputFile.c_str(), "wb");
        if (!file_out) {
            fz_throw(ctx, FZ_ERROR_GENERIC, "Cannot create output file: %s", dto.outputFile.c_str());
        }
#endif
        
        if (fwrite(data, 1, size, file_out) != size) {
            fclose(file_out);
            fz_throw(ctx, FZ_ERROR_GENERIC, "Failed to write output file: %s", dto.outputFile.c_str());
        }
        
        fclose(file_out);
        
        resultStr = "Success!";
        result = EXIT_SUCCESS;
    }
    fz_catch(ctx) {
        resultStr = "Error: " + std::string(fz_caught_message(ctx));
        result = EXIT_FAILURE;
    }

    // Освобождаем ресурсы
    if (buffer) fz_drop_buffer(ctx, buffer);
    if (pix) fz_drop_pixmap(ctx, pix);
    if (doc) fz_drop_document(ctx, doc);
    if (stream) fz_drop_stream(ctx, stream);
    fz_drop_context(ctx);

    // Вывод результата
    if (result == EXIT_SUCCESS) {
        std::cout << resultStr << std::endl;
    } else {
        std::cerr << resultStr << std::endl;
    }
    
    return result;
}