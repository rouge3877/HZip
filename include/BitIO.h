// include/BitIO.h
#ifndef BITIO_H
#define BITIO_H

#include <fstream>
#include <vector>
#include <cstdint>

class BitWriter {
public:
    BitWriter(std::ofstream& output);
    void writeBit(bool bit);
    void writeBits(const std::vector<bool>& bits);
    void writeByte(uint8_t byte);
    void flush();
    int flushAndGetPadding();
    int getBitCount() const;

private:
    std::ofstream& out;
    uint8_t buffer;
    int bitCount;
};

class BitReader {
public:
    BitReader(std::ifstream& input, size_t dataSize);
    bool readBit(bool& bit);

private:
    std::ifstream& in;
    uint8_t buffer;
    int bitCount;
    size_t bytesRead;
    size_t dataSize;
};

#endif // BITIO_H
