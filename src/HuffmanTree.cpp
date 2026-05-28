#include "HuffmanTree.h"
#include <stdexcept>

std::unordered_map<unsigned char, int>
HuffmanTree::buildFrequencyTable(const std::vector<unsigned char>& data) {
    std::unordered_map<unsigned char, int> freq;
    for (unsigned char byte : data) freq[byte]++;
    return freq;
}

std::shared_ptr<HuffNode>
HuffmanTree::buildTree(const std::unordered_map<unsigned char, int>& freqTable) {
    if (freqTable.empty()) return nullptr;

    std::priority_queue<std::shared_ptr<HuffNode>,
                        std::vector<std::shared_ptr<HuffNode>>,
                        NodeCompare> minHeap;

    for (auto& [ch, freq] : freqTable)
        minHeap.push(std::make_shared<HuffNode>(ch, freq));

    if (minHeap.size() == 1) {
        auto only = minHeap.top(); minHeap.pop();
        return std::make_shared<HuffNode>(only->freq, only, nullptr);
    }

    while (minHeap.size() > 1) {
        auto left  = minHeap.top(); minHeap.pop();
        auto right = minHeap.top(); minHeap.pop();
        minHeap.push(std::make_shared<HuffNode>(left->freq + right->freq, left, right));
    }
    return minHeap.top();
}

void HuffmanTree::generateCodes(
    const std::shared_ptr<HuffNode>& node,
    const std::string& currentCode,
    std::unordered_map<unsigned char, std::string>& codes)
{
    if (!node) return;
    if (node->isLeaf) { codes[node->ch] = currentCode.empty() ? "0" : currentCode; return; }
    generateCodes(node->left,  currentCode + "0", codes);
    generateCodes(node->right, currentCode + "1", codes);
}

void HuffmanTree::serializeTree(const std::shared_ptr<HuffNode>& node,
                                std::vector<unsigned char>& out) {
    if (!node) return;
    if (node->isLeaf) { out.push_back('1'); out.push_back(node->ch); }
    else { out.push_back('0'); serializeTree(node->left, out); serializeTree(node->right, out); }
}

std::shared_ptr<HuffNode>
HuffmanTree::deserializeTree(const std::vector<unsigned char>& data, size_t& idx) {
    if (idx >= data.size()) throw std::runtime_error("Corrupt file!");
    unsigned char marker = data[idx++];
    if (marker == '1') {
        if (idx >= data.size()) throw std::runtime_error("Corrupt file!");
        return std::make_shared<HuffNode>(data[idx++], 0);
    }
    auto left  = deserializeTree(data, idx);
    auto right = deserializeTree(data, idx);
    return std::make_shared<HuffNode>(0, left, right);
}
