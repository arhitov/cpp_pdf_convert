#ifndef COPY_DTO_H
#define COPY_DTO_H

#include "types.h"

struct CopyDto : public HandlerDto {
    std::string source;
    std::string destination;
    
    CopyDto(std::string src, std::string dest)
        : source(std::move(src)), destination(std::move(dest)) {}
};

#endif