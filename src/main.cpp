#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "Compressor.h"
#include "Encryptor.h"

std::vector<unsigned char> readFileBytes(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("Cannot open: " + path);
    return std::vector<unsigned char>(std::istreambuf_iterator<char>(file), {});
}

bool writeFileBytes(const std::string& path, const std::vector<unsigned char>& data) {
    std::ofstream file(path, std::ios::binary);
    if (!file) return false;
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    return true;
}

void printUsage(const std::string& name) {
    std::cout << "\nHuffZip - File Compressor\n\n";
    std::cout << "  " << name << " -c <input> <output.huf>         Compress\n";
    std::cout << "  " << name << " -d <input.huf> <output>         Decompress\n";
    std::cout << "  " << name << " -c <input> <output.huf> -p pass Encrypt+Compress\n";
    std::cout << "  " << name << " -d <input.huf> <output> -p pass Decrypt+Decompress\n\n";
}

int main(int argc, char* argv[]) {
    if (argc < 4) { printUsage(argv[0]); return 1; }

    std::string mode = argv[1], inputPath = argv[2], outputPath = argv[3], password = "";

    for (int i = 4; i < argc - 1; i++)
        if (std::string(argv[i]) == "-p") { password = argv[i+1]; i++; }

    if (mode == "-c") {
        std::cout << "\nCompressing: " << inputPath << " -> " << outputPath << "\n";
        if (!password.empty()) {
            std::cout << "Encrypting...\n";
            try {
                auto raw = readFileBytes(inputPath);
                auto enc = Encryptor::encrypt(raw, password);
                std::string tmp = inputPath + ".tmp";
                writeFileBytes(tmp, enc);
                bool ok = Compressor::compress(tmp, outputPath);
                std::remove(tmp.c_str());
                return ok ? 0 : 1;
            } catch (const std::exception& e) {
                std::cerr << "[Error] " << e.what() << "\n"; return 1;
            }
        }
        return Compressor::compress(inputPath, outputPath) ? 0 : 1;
    }
    else if (mode == "-d") {
        std::cout << "\nDecompressing: " << inputPath << " -> " << outputPath << "\n";
        if (!password.empty()) {
            std::cout << "Decrypting...\n";
            try {
                std::string tmp = outputPath + ".tmp";
                bool ok = Compressor::decompress(inputPath, tmp);
                if (!ok) return 1;
                auto dec = Encryptor::decrypt(readFileBytes(tmp), password);
                writeFileBytes(outputPath, dec);
                std::remove(tmp.c_str());
                std::cout << "Done!\n";
                return 0;
            } catch (const std::exception& e) {
                std::cerr << "[Error] " << e.what() << "\n"; return 1;
            }
        }
        return Compressor::decompress(inputPath, outputPath) ? 0 : 1;
    }

    printUsage(argv[0]);
    return 1;
}
