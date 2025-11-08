#ifndef CONVERT_DTO_H
#define CONVERT_DTO_H

#include "types.h"

struct PdfConvertDto : public HandlerDto {
    std::string inputFile;
    int pageNumber;
    int dpi;
    std::string outputFile;
    
    PdfConvertDto(std::string file, int page, int resolution, std::string out)
        : inputFile(std::move(file)), pageNumber(page), dpi(resolution), outputFile(std::move(out)) {}
};

#endif