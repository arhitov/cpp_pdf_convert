#include "handler.h"
#include <iostream>

int CopyHandler(const CopyDto& dto) {
    std::cout << "\n=== PDF Convert: Simple Copy ===\n";
    std::cout << "Source:      " << dto.source << "\n";
    std::cout << "Destination: " << dto.destination << "\n";
    std::cout << "================================\n";
    return 0;
}