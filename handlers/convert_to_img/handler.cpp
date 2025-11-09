#include "handler.h"
#include <mupdf/fitz.h>
#include <iostream>

int PdfConvertHandler(const PdfConvertDto& dto) {
    fz_context* ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
    if (!ctx) {
        std::cerr << "Failed to create MuPDF context";
        return EXIT_FAILURE;
    }

    fz_register_document_handlers(ctx);

    fz_document* doc = fz_open_document(ctx, dto.inputFile.c_str());
    if (!doc) {
        std::cerr << "Failed to open document: " << dto.inputFile;
        fz_drop_context(ctx);
        return EXIT_FAILURE;
    }

    fz_pixmap* pix = NULL;
    int result;
    std::string resultStr;

    fz_try(ctx) {
        fz_page* page = fz_load_page(ctx, doc, dto.pageNumber - 1);
        fz_matrix transform = fz_scale(dto.dpi / 72.0f, dto.dpi / 72.0f);
        pix = fz_new_pixmap_from_page(ctx, page, transform, fz_device_rgb(ctx), 0);
        fz_save_pixmap_as_jpeg(ctx, pix, dto.outputFile.c_str(), 90);
        fz_drop_page(ctx, page);
        resultStr = "Success!";
        result = EXIT_SUCCESS;
    }
    fz_catch(ctx) {
        resultStr = "Error: " + std::string(fz_caught_message(ctx));
        result = EXIT_FAILURE;
    }

    if (pix) fz_drop_pixmap(ctx, pix);
    fz_drop_document(ctx, doc);
    fz_drop_context(ctx);

    if (result == EXIT_SUCCESS) {
        std::cout << resultStr;
    } else {
        std::cerr << resultStr;
    }
    
    return result;
}