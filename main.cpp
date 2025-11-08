#include <iostream>
#include <vector>
#include <memory>
#include "types.h"
#include "validators.h"
#include "handlers/convert_to_img/rules.h"
#include "handlers/help/rules.h"
#include "handlers/test/rules.h"

int ParseAndExecute(int argc, char* argv[], const std::vector<Handler>& handlers) {
    if (argc < 2) {
        std::cerr << "Error: No arguments provided";
        return EXIT_FAILURE;
    }
    
    const int argumentsCount = argc - 1;
    // std::vector<std::string> candidateErrors;
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
			
			// std::cout << "pdf_convert: Executing handler '" << handler.name << "'\n";
			// Вызываем executor, который создаст DTO и вызовет handler
			return handler.executor(args);
        }
        
        // std::string errorBlock = "Handler '" + handler.name + "':\n";
        // for (const auto& err : errors) {
        //     errorBlock += err + "\n";
        // }
        // candidateErrors.push_back(errorBlock);
    }
    
    std::cerr << "Error: No matching handler found for provided arguments";
    // std::cerr << "Provided arguments (" << argumentsCount << "):\n";
    // for (int i = 1; i < argc; ++i) {
    //     std::cerr << "  [" << i << "] '" << argv[i] << "'\n";
    // }
    
    // if (foundCandidate && !candidateErrors.empty()) {
    //     std::cerr << "\nValidation errors:\n";
    //     for (const auto& err : candidateErrors) {
    //         std::cerr << err << "\n";
    //     }
    // } else if (!foundCandidate) {
    //     std::cerr << "\nNo handler expects " << argumentsCount << " arguments.\n";
    // }
    
    // std::cerr << "\nAvailable handlers for pdf_convert:\n";
    // for (const auto& handler : handlers) {
    //     std::cerr << "\n  " << handler.name << ": " << handler.description << "\n";
    //     for (size_t i = 0; i < handler.arguments.size(); ++i) {
    //         const auto& arg = handler.arguments[i];
    //         std::cerr << "    Arg #" << (i+1) << ": " << arg.name 
    //                  << " - " << arg.description << "\n";
    //     }
    // }
    
    // std::cerr << "\nExample usage:\n";
    // std::cerr << "  pdf_convert c:/in/file.pdf 1 300 c:/out/img.jpg\n";
    // std::cerr << "  pdf_convert /in/file.pdf 5 600 /out/image.png\n";
    
    return EXIT_FAILURE;
}


int main(int argc, char* argv[]) {
    std::vector<Handler> handlers;
    handlers.push_back(GetConvertHandler());
    handlers.push_back(GetHelpHandler());
    handlers.push_back(GetTestHandler());
    
    return ParseAndExecute(argc, argv, handlers);
}
