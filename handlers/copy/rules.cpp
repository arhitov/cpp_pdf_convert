#include "rules.h"
#include "validators.h"
#include "handler.h"
#include "dto.h"
#include <memory>

Handler GetCopyHandler() {
    auto fileReadableValidator = std::make_shared<FileReadableValidator>();
    auto fileWritableValidator = std::make_shared<FileWritableValidator>();
    
    auto executor = [](const std::vector<std::string>& args) -> int {
        CopyDto dto(args[0], args[1]);
        return CopyHandler(dto);
    };
    
    return {
        "copy",
        "Copy file from source to destination (pdf_convert copy <source> <dest>)",
        {
            {"source", "Source file path (must exist and be readable)", fileReadableValidator},
            {"destination", "Destination path (directory must be writable)", fileWritableValidator}
        },
        executor
    };
}