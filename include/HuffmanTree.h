#pragma once
#include <unordered_map>
#include <queue>
#include <string>
#include <memory>

struct HuffNode {
    unsigned char ch;
    int freq;
    bool isLeaf;
    std::shared_ptr<HuffNode> left, right;

    HuffNode(int f, std::shared_ptr<HuffNode> l, std::shared_ptr<HuffNode> r)
        : ch(0), freq(f), isLeaf(false), left(l), right(r) {}

    HuffNode(unsigned char c, int f)
        : ch(c), freq(f), isLeaf(true), left(nullptr), right(nullptr) {}
};

struct NodeCompare {
    bool operator()(const std::shared_ptr<HuffNode>& a,
                    const std::shared_ptr<HuffNode>& b) {
        return a->freq > b->freq;
    }
};

class HuffmanTree {
public:
    static std::unordered_map<unsigned char, int>
    buildFrequencyTable(const std::vector<unsigned char>& data);

    static std::shared_ptr<HuffNode>
    buildTree(const std::unordered_map<unsigned char, int>& freqTable);

    static void generateCodes(
        const std::shared_ptr<HuffNode>& node,
        const std::string& currentCode,
        std::unordered_map<unsigned char, std::string>& codes);

    static void serializeTree(const std::shared_ptr<HuffNode>& node,
                              std::vector<unsigned char>& out);

    static std::shared_ptr<HuffNode>
    deserializeTree(const std::vector<unsigned char>& data, size_t& idx);
};
