#ifndef HELP_DTO_H
#define HELP_DTO_H

#include "types.h"

struct HelpDto : public HandlerDto {
    std::string help;
    
    HelpDto(std::string help)
        : help(std::move(help)) {}
};

#endif