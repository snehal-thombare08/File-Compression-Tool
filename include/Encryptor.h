#pragma once
#include <string>
#include <vector>
class Encryptor {
public:
    static std::vector<unsigned char> encrypt(const std::vector<unsigned char>& data, const std::string& password);
    static std::vector<unsigned char> decrypt(const std::vector<unsigned char>& data, const std::string& password);
private:
    static std::vector<unsigned char> deriveKey(const std::string& password);
};
