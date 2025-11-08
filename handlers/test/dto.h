#ifndef TEST_DTO_H
#define TEST_DTO_H

#include "types.h"

struct TestDto : public HandlerDto {
    std::string test;
    std::string answer;
    
    TestDto(std::string test, std::string answer)
        : test(std::move(test)), answer(std::move(answer)) {}
};

#endif