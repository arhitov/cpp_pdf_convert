#include "rules.h"
#include "validators.h"
#include "handler.h"
#include "dto.h"
#include <memory>

Handler GetTestHandler() {
    auto testValidator = std::make_shared<ChoiceStrValidator>(std::initializer_list<std::string>{"test"});
    auto answerValidator = std::make_shared<ChoiceStrValidator>(std::initializer_list<std::string>{"success", "error"});
    
    auto executor = [](const std::vector<std::string>& args) -> int {
        TestDto dto(args[0], args[1]);
        return TestHandler(dto);
    };
    
    return {
        "test",
        "Return reques answer",
        {
            {"test", "test string", testValidator},
            {"answer", "answer string (success, error)", answerValidator},
        },
        executor
    };
}