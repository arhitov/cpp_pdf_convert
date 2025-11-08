#include <iostream>
#include <vector>
#include <memory>
#include "types.h"
#include "validators.h"
#include "handlers/convert_to_img/rules.h"
#include "handlers/copy/rules.h"

// #include <iostream>
// #include <string>
// // #include <mupdf/fitz.h>
// #include <stdio.h>
// #include <stdlib.h>

int ParseAndExecute(int argc, char* argv[], const std::vector<Handler>& handlers) {
    if (argc < 2) {
        std::cerr << "Error: No arguments provided\n\n";
        std::cerr << "Usage: pdf_convert <arguments>\n\n";
        std::cerr << "Available handlers:\n";
        for (const auto& h : handlers) {
            std::cerr << "  " << h.name << ": " << h.description << "\n";
        }
        return 1;
    }
    
    const int argumentsCount = argc - 1;
    std::vector<std::string> candidateErrors;
    bool foundCandidate = false;
    
    for (const auto& handler : handlers) {
        if (handler.arguments.size() != argumentsCount) {
            continue;
        }
        
        foundCandidate = true;
        bool allValid = true;
        std::vector<std::string> errors;
        
        for (int i = 0; i < argumentsCount; ++i) {
            std::string errorMsg;
            if (!handler.arguments[i].validator->Validate(argv[i + 1], errorMsg)) {
                allValid = false;
                errors.push_back("  Arg #" + std::to_string(i+1) + " (" + 
                               handler.arguments[i].name + "): " + errorMsg);
            }
        }
        
        if (allValid) {
			std::vector<std::string> args;
			for (int i = 1; i < argc; ++i) args.push_back(argv[i]);
			
			std::cout << "pdf_convert: Executing handler '" << handler.name << "'\n";
			// Вызываем executor, который создаст DTO и вызовет handler
			return handler.executor(args);
        }
        
        std::string errorBlock = "Handler '" + handler.name + "':\n";
        for (const auto& err : errors) {
            errorBlock += err + "\n";
        }
        candidateErrors.push_back(errorBlock);
    }
    
    std::cerr << "pdf_convert: Error: No matching handler found for provided arguments\n\n";
    std::cerr << "Provided arguments (" << argumentsCount << "):\n";
    for (int i = 1; i < argc; ++i) {
        std::cerr << "  [" << i << "] '" << argv[i] << "'\n";
    }
    
    if (foundCandidate && !candidateErrors.empty()) {
        std::cerr << "\nValidation errors:\n";
        for (const auto& err : candidateErrors) {
            std::cerr << err << "\n";
        }
    } else if (!foundCandidate) {
        std::cerr << "\nNo handler expects " << argumentsCount << " arguments.\n";
    }
    
    std::cerr << "\nAvailable handlers for pdf_convert:\n";
    for (const auto& handler : handlers) {
        std::cerr << "\n  " << handler.name << ": " << handler.description << "\n";
        for (size_t i = 0; i < handler.arguments.size(); ++i) {
            const auto& arg = handler.arguments[i];
            std::cerr << "    Arg #" << (i+1) << ": " << arg.name 
                     << " - " << arg.description << "\n";
        }
    }
    
    std::cerr << "\nExample usage:\n";
    std::cerr << "  pdf_convert c:/in/file.pdf 1 300 c:/out/img.jpg\n";
    std::cerr << "  pdf_convert /in/file.pdf 5 600 /out/image.png\n";
    
    return 1;
}


int main(int argc, char* argv[]) {
    std::vector<Handler> handlers;
    handlers.push_back(GetConvertHandler());
    handlers.push_back(GetCopyHandler());
    
    return ParseAndExecute(argc, argv, handlers);

/*
	char *input;
	float zoom, rotate;
	int page_number, page_count;
	fz_context *ctx;
	fz_document *doc;
	fz_pixmap *pix;
	fz_matrix ctm;
	int x, y;

	if (argc < 3)
	{
		fprintf(stderr, "usage: example input-file page-number [ zoom [ rotate ] ]\n");
		fprintf(stderr, "\tinput-file: path of PDF, XPS, CBZ or EPUB document to open\n");
		fprintf(stderr, "\tPage numbering starts from one.\n");
		fprintf(stderr, "\tZoom level is in percent (100 percent is 72 dpi).\n");
		fprintf(stderr, "\tRotation is in degrees clockwise.\n");
		return EXIT_FAILURE;
	}

	input = argv[1];
	page_number = atoi(argv[2]) - 1;
	zoom = argc > 3 ? atof(argv[3]) : 100;
	rotate = argc > 4 ? atof(argv[4]) : 0;

	/* Create a context to hold the exception stack and various caches. *\/
	ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
	if (!ctx)
	{
		fprintf(stderr, "cannot create mupdf context\n");
		return EXIT_FAILURE;
	}

	/* Register the default file types to handle. *\/
	fz_try(ctx)
		fz_register_document_handlers(ctx);
	fz_catch(ctx)
	{
		// fz_report_error(ctx);
		fprintf(stderr, "cannot register document handlers\n");
		fz_drop_context(ctx);
		return EXIT_FAILURE;
	}

	/* Open the document. *\/
	fz_try(ctx)
		doc = fz_open_document(ctx, input);
	fz_catch(ctx)
	{
		// fz_report_error(ctx);
		fprintf(stderr, "cannot open document\n");
		fz_drop_context(ctx);
		return EXIT_FAILURE;
	}

	/* Count the number of pages. *\/
	fz_try(ctx)
		page_count = fz_count_pages(ctx, doc);
	fz_catch(ctx)
	{
		// fz_report_error(ctx);
		fprintf(stderr, "cannot count number of pages\n");
		fz_drop_document(ctx, doc);
		fz_drop_context(ctx);
		return EXIT_FAILURE;
	}

	if (page_number < 0 || page_number >= page_count)
	{
		fprintf(stderr, "page number out of range: %d (page count %d)\n", page_number + 1, page_count);
		fz_drop_document(ctx, doc);
		fz_drop_context(ctx);
		return EXIT_FAILURE;
	}

	/* Compute a transformation matrix for the zoom and rotation desired. *\/
	/* The default resolution without scaling is 72 dpi. *\/
	ctm = fz_scale(zoom / 100, zoom / 100);
	ctm = fz_pre_rotate(ctm, rotate);

	/* Render page to an RGB pixmap. *\/
	fz_try(ctx)
		pix = fz_new_pixmap_from_page_number(ctx, doc, page_number, ctm, fz_device_rgb(ctx), 0);
	fz_catch(ctx)
	{
		// fz_report_error(ctx);
		fprintf(stderr, "cannot render page\n");
		fz_drop_document(ctx, doc);
		fz_drop_context(ctx);
		return EXIT_FAILURE;
	}

	/* Print image data in ascii PPM format. *\/
	printf("P3\n");
	printf("%d %d\n", pix->w, pix->h);
	printf("255\n");
	for (y = 0; y < pix->h; ++y)
	{
		unsigned char *p = &pix->samples[y * pix->stride];
		for (x = 0; x < pix->w; ++x)
		{
			if (x > 0)
				printf("  ");
			printf("%3d %3d %3d", p[0], p[1], p[2]);
			p += pix->n;
		}
		printf("\n");
	}

	/* Clean up. *\/
	fz_drop_pixmap(ctx, pix);
	fz_drop_document(ctx, doc);
	fz_drop_context(ctx);
	return EXIT_SUCCESS;
*/

    // if (argc != 4) {
    //     std::cout << "Usage: " << argv[0] << " input.pdf page_number output.jpg" << std::endl;
    //     return 1;
    // }

    // const char* pdf_file = argv[1];
    // int page_num = std::stoi(argv[2]) - 1;
    // const char* output_file = argv[3];

    // fz_context* ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
    // if (!ctx) return 1;

    // fz_register_document_handlers(ctx);

    // fz_document* doc = fz_open_document(ctx, pdf_file);
    // if (!doc) {
    //     fz_drop_context(ctx);
    //     return 1;
    // }

    // fz_pixmap* pix = NULL;
    // int result = 0;

    // fz_try(ctx) {
    //     fz_page* page = fz_load_page(ctx, doc, page_num);
    //     fz_matrix transform = fz_scale(300.0f / 72.0f, 300.0f / 72.0f); // 300 DPI
    //     pix = fz_new_pixmap_from_page(ctx, page, transform, fz_device_rgb(ctx), 0);
    //     fz_save_pixmap_as_jpeg(ctx, pix, output_file, 90);
    //     fz_drop_page(ctx, page);
    //     std::cout << "Success!" << std::endl;
    // }
    // fz_catch(ctx) {
    //     std::cerr << "Error!" << std::endl;
    //     result = 1;
    // }

    // if (pix) fz_drop_pixmap(ctx, pix);
    // fz_drop_document(ctx, doc);
    // fz_drop_context(ctx);

    // return result;
}
