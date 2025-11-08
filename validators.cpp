#include "validators.h"
#include <fstream>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

bool FileReadableValidator::Validate(const std::string& value, std::string& errorMsg) const {
    std::ifstream file(value, std::ios::binary);
    if (!file.good()) {
        errorMsg = "File '" + value + "' does not exist or cannot be read";
        return false;
    }
    return true;
}

IntValidator::IntValidator(int minVal, int maxVal) : minVal_(minVal), maxVal_(maxVal) {}

bool IntValidator::Validate(const std::string& value, std::string& errorMsg) const {
    try {
        size_t pos;
        int num = std::stoi(value, &pos);
        if (pos != value.size()) {
            errorMsg = "Value '" + value + "' is not a valid integer";
            return false;
        }
        if (num < minVal_ || num > maxVal_) {
            errorMsg = "Value " + std::to_string(num) + " is out of range [" + 
                      std::to_string(minVal_) + ", " + std::to_string(maxVal_) + "]";
            return false;
        }
        return true;
    } catch (const std::exception&) {
        errorMsg = "Value '" + value + "' is not a valid integer";
        return false;
    }
}

ChoiceIntValidator::ChoiceIntValidator(std::initializer_list<int> choices) : choices_(choices) {}

bool ChoiceIntValidator::Validate(const std::string& value, std::string& errorMsg) const {
    static IntValidator intValidator;
    if (!intValidator.Validate(value, errorMsg)) {
        return false;
    }
    int num = std::stoi(value);
    if (std::find(choices_.begin(), choices_.end(), num) == choices_.end()) {
        errorMsg = "Value " + std::to_string(num) + " is not in allowed choices: " + 
                  GetChoicesString();
        return false;
    }
    return true;
}

std::string ChoiceIntValidator::GetChoicesString() const {
    if (choices_.empty()) return "";
    std::string result = std::to_string(choices_[0]);
    for (size_t i = 1; i < choices_.size(); ++i) {
        result += ", " + std::to_string(choices_[i]);
    }
    return result;
}

bool FileWritableValidator::Validate(const std::string& value, std::string& errorMsg) const {
    try {
        fs::path filePath(value);
        fs::path parentPath = filePath.parent_path();
        
        if (parentPath.empty()) {
            parentPath = fs::current_path();
        }
        
        if (!fs::exists(parentPath)) {
            errorMsg = "Directory '" + parentPath.string() + "' does not exist";
            return false;
        }
        
        fs::path testFile = parentPath / "write_test.tmp";
        std::ofstream test(testFile);
        if (!test.good()) {
            errorMsg = "Directory '" + parentPath.string() + "' is not writable";
            return false;
        }
        test.close();
        fs::remove(testFile);
        
        return true;
    } catch (const std::exception& e) {
        errorMsg = "Error: " + std::string(e.what());
        return false;
    }
}