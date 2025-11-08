#include "handler.h"
#include "handlers/convert_to_img/rules.h"
#include "handlers/test/rules.h"
#include <iostream>

int TestHandler(const TestDto& dto) {
    if (dto.answer == "success") {
        std::cout << "Success!";
        return EXIT_SUCCESS;
    } else {
        std::cerr << "Error!";
        return EXIT_FAILURE;
    }
}