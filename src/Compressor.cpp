#include "Compressor.h"
#include "HuffmanTree.h"
#include <fstream>
#include <iostream>
#include <cstdint>
#include <stdexcept>

std::vector<unsigned char> Compressor::readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("File opened failed: " + path);
    return std::vector<unsigned char>(std::istreambuf_iterator<char>(file), {});
}

bool Compressor::writeFile(const std::string& path, const std::vector<unsigned char>& data) {
    std::ofstream file(path, std::ios::binary);
    if (!file) return false;
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    return true;
}

bool Compressor::compress(const std::string& inputPath, const std::string& outputPath) {
    std::vector<unsigned char> inputData;
    try { inputData = readFile(inputPath); }
    catch (const std::exception& e) { std::cerr << "[Error] " << e.what() << "\n"; return false; }

    if (inputData.empty()) { std::cerr << "[Error] File empty!\n"; return false; }

    auto freqTable = HuffmanTree::buildFrequencyTable(inputData);
    auto root      = HuffmanTree::buildTree(freqTable);
    std::unordered_map<unsigned char, std::string> codes;
    HuffmanTree::generateCodes(root, "", codes);

    std::vector<unsigned char> treeData;
    HuffmanTree::serializeTree(root, treeData);

    std::vector<unsigned char> compressedBits;
    BitWriter writer(compressedBits);
    for (unsigned char byte : inputData) writer.writeBits(codes[byte]);
    int paddingBits = writer.getPaddingBits();
    writer.flush();

    std::vector<unsigned char> output;
    output.push_back('H'); output.push_back('U');
    output.push_back('F'); output.push_back('F');
    output.push_back(static_cast<unsigned char>(paddingBits));

    uint32_t treeSize = static_cast<uint32_t>(treeData.size());
    output.push_back( treeSize        & 0xFF);
    output.push_back((treeSize >>  8) & 0xFF);
    output.push_back((treeSize >> 16) & 0xFF);
    output.push_back((treeSize >> 24) & 0xFF);

    output.insert(output.end(), treeData.begin(), treeData.end());
    output.insert(output.end(), compressedBits.begin(), compressedBits.end());

    if (!writeFile(outputPath, output)) { std::cerr << "[Error] Write failed!\n"; return false; }

    double ratio = 100.0 * (1.0 - (double)output.size() / inputData.size());
    std::cout << "Compressed!\n";
    std::cout << "   Original  : " << inputData.size() << " bytes\n";
    std::cout << "   Compressed: " << output.size()    << " bytes\n";
    std::cout << "   Ratio     : " << ratio << "%\n";
    return true;
}

bool Compressor::decompress(const std::string& inputPath, const std::string& outputPath) {
    std::vector<unsigned char> inputData;
    try { inputData = readFile(inputPath); }
    catch (const std::exception& e) { std::cerr << "[Error] " << e.what() << "\n"; return false; }

    if (inputData.size() < 9 ||
        inputData[0]!='H'||inputData[1]!='U'||inputData[2]!='F'||inputData[3]!='F') {
        std::cerr << "[Error] Not a valid .huf file!\n"; return false;
    }

    size_t idx = 4;
    int paddingBits = inputData[idx++];
    uint32_t treeSize = inputData[idx] | (inputData[idx+1]<<8) |
                        (inputData[idx+2]<<16) | (inputData[idx+3]<<24);
    idx += 4;

    std::vector<unsigned char> treeData(inputData.begin()+idx, inputData.begin()+idx+treeSize);
    size_t treeIdx = 0;
    auto root = HuffmanTree::deserializeTree(treeData, treeIdx);
    idx += treeSize;

    std::vector<unsigned char> compressedBits(inputData.begin()+idx, inputData.end());
    long long totalBits = (long long)compressedBits.size() * 8 - paddingBits;

    std::vector<unsigned char> output;
    BitReader reader(compressedBits, 0);
    auto current = root;
    long long bitsRead = 0;

    while (bitsRead < totalBits) {
        int bit = reader.readBit();
        if (bit < 0) break;
        bitsRead++;
        if (current->isLeaf) { output.push_back(current->ch); current = root; continue; }
        current = (bit == 0) ? current->left : current->right;
        if (current && current->isLeaf) { output.push_back(current->ch); current = root; }
    }

    if (!writeFile(outputPath, output)) { std::cerr << "[Error] Write failed!\n"; return false; }
    std::cout << "Decompressed!\n";
    std::cout << "   Recovered : " << output.size() << " bytes\n";
    return true;
}
