#include "rules.h"
#include "validators.h"
#include "handler.h"
#include "dto.h"
#include <memory>

Handler GetConvertHandler() {
    auto fileReadableValidator = std::make_shared<FileReadableValidator>();
    auto pageValidator = std::make_shared<IntValidator>(1, 10000);
    auto dpiValidator = std::make_shared<ChoiceIntValidator>(std::initializer_list<int>{150, 300, 600});
    auto fileWritableValidator = std::make_shared<FileWritableValidator>();
    
    auto executor = [](const std::vector<std::string>& args) -> int {
        // Конвертируем после валидации - безопасно
        PdfConvertDto dto(args[0], std::stoi(args[1]), std::stoi(args[2]), args[3]);
        return PdfConvertHandler(dto);
    };
    
    return {
        "convert",
        "Convert PDF page to image (pdf_convert convert <file> <page> <dpi> <output>)",
        {
            {"inputFile", "Input PDF file path (must exist and be readable)", fileReadableValidator},
            {"pageNumber", "Page number to convert (1-10000)", pageValidator},
            {"dpi", "DPI resolution (150, 300, 600)", dpiValidator},
            {"outputFile", "Output image file path (directory must be writable)", fileWritableValidator}
        },
        executor
    };
}