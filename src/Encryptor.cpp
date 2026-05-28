#include "Encryptor.h"
#include <vector>
#include <stdexcept>

std::vector<unsigned char> Encryptor::deriveKey(const std::string& password) {
    std::vector<unsigned char> key(32, 0);
    for (size_t i = 0; i < 32; i++)
        key[i] = static_cast<unsigned char>(password[i % password.size()] ^ (i * 31 + 7));
    for (size_t i = 1; i < 32; i++) key[i] ^= key[i-1];
    return key;
}

std::vector<unsigned char>
Encryptor::encrypt(const std::vector<unsigned char>& data, const std::string& password) {
    if (password.empty()) throw std::invalid_argument("Password empty!");
    auto key = deriveKey(password);
    std::vector<unsigned char> result(data.size());
    for (size_t i = 0; i < data.size(); i++) result[i] = data[i] ^ key[i % key.size()];
    return result;
}

std::vector<unsigned char>
Encryptor::decrypt(const std::vector<unsigned char>& data, const std::string& password) {
    return encrypt(data, password);
}
