#include "handler.h"
#include "handlers/convert_to_img/rules.h"
#include "handlers/test/rules.h"
#include <iostream>

int HelpHandler(const HelpDto& dto) {
    std::vector<Handler> handlers;
    handlers.push_back(GetConvertHandler());
    handlers.push_back(GetTestHandler());

    std::cout << "\n=== Help Page ===\n";
    std::cerr << "Usage: pdf_convert <arguments>\n\n";
    std::cerr << "Available handlers:\n";
    for (const auto& h : handlers) {
        std::cerr << "  " << h.name << ": " << h.description << "\n";
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
    std::cout << "================================\n";
    return EXIT_SUCCESS;
}