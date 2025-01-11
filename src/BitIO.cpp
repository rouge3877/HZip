// src/BitIO.cpp
#include "BitIO.h"
#include <stdexcept>

// BitWriter实现
BitWriter::BitWriter(std::ofstream& output) : out(output), buffer(0), bitCount(0) {}

void BitWriter::writeBit(bool bit) {
    buffer = (buffer << 1) | bit;
    bitCount++;
    if (bitCount == 8) {
        out.write(reinterpret_cast<const char*>(&buffer), sizeof(uint8_t));
        buffer = 0;
        bitCount = 0;
    }
}

void BitWriter::writeBits(const std::vector<bool>& bits) {
    for (bool bit : bits) {
        writeBit(bit);
    }
}

void BitWriter::writeByte(uint8_t byte) {
    if (bitCount == 0) {
        out.write(reinterpret_cast<const char*>(&byte), sizeof(uint8_t));
    } else {
        for (int i = 7; i >= 0; --i) {
            bool bit = (byte >> i) & 1;
            writeBit(bit);
        }
    }
}

void BitWriter::flush() {
    if (bitCount > 0) {
        buffer = buffer << (8 - bitCount); // 将剩余比特左移到高位
        out.write(reinterpret_cast<const char*>(&buffer), sizeof(uint8_t));
        buffer = 0;
        bitCount = 0;
    }
}

int BitWriter::flushAndGetPadding() {
    if (bitCount > 0) {
        buffer = buffer << (8 - bitCount); // 将剩余比特左移到高位
        out.write(reinterpret_cast<const char*>(&buffer), sizeof(uint8_t));
        int padding = 8 - bitCount;
        buffer = 0;
        bitCount = 0;
        return padding;
    }
    return 0;
}

int BitWriter::getBitCount() const {
    return bitCount;
}

// BitReader实现
BitReader::BitReader(std::ifstream& input, size_t dataSize)
    : in(input), buffer(0), bitCount(0), bytesRead(0), dataSize(dataSize) {}

bool BitReader::readBit(bool& bit) {
    if (bytesRead >= dataSize && bitCount == 0) {
        return false; // EOF或错误
    }

    if (bitCount == 0) {
        if (!in.read(reinterpret_cast<char*>(&buffer), sizeof(uint8_t))) {
            return false; // EOF或错误
        }
        bitCount = 8;
        bytesRead++;
    }
    bit = (buffer >> (bitCount - 1)) & 1;
    bitCount--;
    return true;
}
