// include/HuffmanDecoder.h
#ifndef HUFFMANDECODER_H
#define HUFFMANDECODER_H

#include <string>
#include "HuffmanTree.h"
#include "BitIO.h"

class HuffmanDecoder {
public:
    void decompress(const std::string& inputPath, const std::string& outputPath);
};

#endif // HUFFMANDECODER_H
