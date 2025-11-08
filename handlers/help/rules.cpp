#include "rules.h"
#include "validators.h"
#include "handler.h"
#include "dto.h"
#include <memory>

Handler GetHelpHandler() {
    auto helpValidator = std::make_shared<ChoiceStrValidator>(std::initializer_list<std::string>{"help"});
    
    auto executor = [](const std::vector<std::string>& args) -> int {
        HelpDto dto(args[0]);
        return HelpHandler(dto);
    };
    
    return {
        "help",
        "Help page",
        {
            {"help", "help string", helpValidator},
        },
        executor
    };
}