#pragma once
#include <string>
#include <vector>

class Compressor {
public:
    static bool compress(const std::string& inputPath,
                         const std::string& outputPath);
    static bool decompress(const std::string& inputPath,
                           const std::string& outputPath);

private:
    struct BitWriter {
        std::vector<unsigned char>& out;
        unsigned char buffer = 0;
        int bitCount = 0;

        BitWriter(std::vector<unsigned char>& o) : out(o) {}

        void writeBit(int bit) {
            buffer = (buffer << 1) | (bit & 1);
            bitCount++;
            if (bitCount == 8) flush();
        }

        void writeBits(const std::string& bits) {
            for (char b : bits) writeBit(b - '0');
        }

        void flush() {
            if (bitCount > 0) {
                buffer <<= (8 - bitCount);
                out.push_back(buffer);
                buffer = 0;
                bitCount = 0;
            }
        }

        int getPaddingBits() const {
            return bitCount == 0 ? 0 : (8 - bitCount);
        }
    };

    struct BitReader {
        const std::vector<unsigned char>& data;
        size_t byteIdx;
        int bitIdx;

        BitReader(const std::vector<unsigned char>& d, size_t start)
            : data(d), byteIdx(start), bitIdx(7) {}

        int readBit() {
            if (byteIdx >= data.size()) return -1;
            int bit = (data[byteIdx] >> bitIdx) & 1;
            bitIdx--;
            if (bitIdx < 0) { bitIdx = 7; byteIdx++; }
            return bit;
        }
    };

    static std::vector<unsigned char> readFile(const std::string& path);
    static bool writeFile(const std::string& path,
                          const std::vector<unsigned char>& data);
};
