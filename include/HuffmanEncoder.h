// include/HuffmanEncoder.h
#ifndef HUFFMANENCODER_H
#define HUFFMANENCODER_H

#include <string>
#include "HuffmanTree.h"

class HuffmanEncoder {
public:
    void compress(const std::string& inputPath, const std::string& outputPath);

private:
    std::vector<uint8_t> packBits(const std::vector<bool>& bits);
};

#endif // HUFFMANENCODER_H
