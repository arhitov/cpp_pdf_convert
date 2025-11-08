#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>
#include <memory>
#include <functional>

class ArgumentValidator {
public:
    virtual ~ArgumentValidator() = default;
    virtual bool Validate(const std::string& value, std::string& errorMsg) const = 0;
};

struct ArgumentDescriptor {
    std::string name;
    std::string description;
    std::shared_ptr<ArgumentValidator> validator;
};

// Базовый DTO (пустой)
struct HandlerDto {
    virtual ~HandlerDto() = default;
};

// Умный указатель для DTO
using HandlerDtoPtr = std::unique_ptr<HandlerDto>;

struct Handler {
    std::string name;
    std::string description;
    std::vector<ArgumentDescriptor> arguments;
    std::function<int(const std::vector<std::string>&)> executor; // Создает DTO и вызывает handler
};

#endif