#ifndef VALIDATORS_H
#define VALIDATORS_H

#include "types.h"
#include <climits>

// Валидатор: файл должен существовать и быть доступен для чтения
class FileReadableValidator : public ArgumentValidator {
public:
    bool Validate(const std::string& value, std::string& errorMsg) const override;
};

// Валидатор: целое число в заданном диапазоне
class IntValidator : public ArgumentValidator {
public:
    IntValidator(int minVal = INT_MIN, int maxVal = INT_MAX);
    bool Validate(const std::string& value, std::string& errorMsg) const override;
private:
    int minVal_, maxVal_;
};

// Валидатор: строка должна быть из списка допустимых значений
class ChoiceStrValidator : public ArgumentValidator {
public:
    ChoiceStrValidator(std::initializer_list<std::string> choices);
    bool Validate(const std::string& value, std::string& errorMsg) const override;
private:
    std::string GetChoicesString() const;
    std::vector<std::string> choices_;
};

// Валидатор: число должно быть из списка допустимых значений
class ChoiceIntValidator : public ArgumentValidator {
public:
    ChoiceIntValidator(std::initializer_list<int> choices);
    bool Validate(const std::string& value, std::string& errorMsg) const override;
private:
    std::string GetChoicesString() const;
    std::vector<int> choices_;
};

// Валидатор: директория для выходного файла должна быть доступна для записи
class FileWritableValidator : public ArgumentValidator {
public:
    bool Validate(const std::string& value, std::string& errorMsg) const override;
};

#endif