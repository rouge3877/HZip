// src/HuffmanDecoder.cpp
#include "HuffmanDecoder.h"
#include "HuffmanTree.h"
#include "BitIO.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <filesystem>

// Utility function declaration (assumed to be in Utils.h)
std::string getAbsolutePath(const std::string& filename);

void HuffmanDecoder::decompress(const std::string& inputPath, const std::string& outputPath) {
    // Open input file
    std::ifstream inputFile(inputPath, std::ios::binary);
    if (!inputFile.is_open()) {
        throw std::runtime_error("Unable to open input file: " + inputPath);
    }

    // Get file size
    inputFile.seekg(0, std::ios::end);
    std::streampos fileSize = inputFile.tellg();

    // Return to the beginning of the file
    inputFile.seekg(0, std::ios::beg);

    // Read the size of the code table (1 byte)
    uint8_t tableSize = 0;
    if (!inputFile.read(reinterpret_cast<char*>(&tableSize), sizeof(uint8_t))) {
        throw std::runtime_error("Unable to read code table size");
    }

    // Read the total number of characters (4 bytes)
    int totalChars = 0;
    if (!inputFile.read(reinterpret_cast<char*>(&totalChars), sizeof(int))) {
        throw std::runtime_error("Unable to read total number of characters");
    }

    // Read the code table
    std::unordered_map<std::vector<bool>, ORIGINAL_DATA_TYPE, VectorBoolHash> inverseCodeTable;
    for (int i = 0; i < tableSize; ++i) {
        // Read character (1 byte)
        ORIGINAL_DATA_TYPE charKey;
        if (!inputFile.read(reinterpret_cast<char*>(&charKey), sizeof(ORIGINAL_DATA_TYPE))) {
            throw std::runtime_error("Unable to read character from code table");
        }

        // Read code length (1 byte)
        uint8_t codeLength = 0;
        if (!inputFile.read(reinterpret_cast<char*>(&codeLength), sizeof(uint8_t))) {
            throw std::runtime_error("Unable to read code length");
        }

        // Calculate the number of bytes needed to read
        int bytesNeeded = (codeLength + 7) / 8;

        // Read encoded bits
        std::vector<uint8_t> packedBytes(bytesNeeded);
        if (!inputFile.read(reinterpret_cast<char*>(packedBytes.data()), bytesNeeded)) {
            throw std::runtime_error("Unable to read encoded bits");
        }

        // Unpack bits
        std::vector<bool> codeBits;
        for (size_t b = 0; b < packedBytes.size(); ++b) {
            uint8_t byte = packedBytes[b];
            for (int bit = 7; bit >= 0; --bit) {
                if (codeBits.size() < codeLength) {
                    bool bitValue = (byte >> bit) & 1;
                    codeBits.push_back(bitValue);
                }
            }
        }

        inverseCodeTable[codeBits] = charKey;
    }

    // Calculate the position of the padding bits
    // Padding bits are in the last byte of the file
    if (fileSize < 1) {
        throw std::runtime_error("File too small to contain padding bits");
    }
    inputFile.seekg(-static_cast<std::streamoff>(1), std::ios::end);
    uint8_t padding = 0;
    if (!inputFile.read(reinterpret_cast<char*>(&padding), sizeof(uint8_t))) {
        throw std::runtime_error("Unable to read number of padding bits");
    }

    // Calculate the start position and length of the data section
    // Note: Ensure correct calculation of data start position and size
    // Skip header information (tableSize and totalChars)
    inputFile.seekg(1 + sizeof(int), std::ios::beg); // Skip tableSize and totalChars

    // Skip code table
    for (int i = 0; i < tableSize; ++i) {
        // Skip character (1 byte) and code length (1 byte)
        inputFile.seekg(1 + 1, std::ios::cur);
        // Read code length
        uint8_t codeLength = 0;
        inputFile.seekg(-1, std::ios::cur); // Move back to read codeLength
        if (!inputFile.read(reinterpret_cast<char*>(&codeLength), sizeof(uint8_t))) {
            throw std::runtime_error("Unable to read code length");
        }
        // Calculate the number of bytes to skip
        int bytesToSkip = (codeLength + 7) / 8;
        inputFile.seekg(bytesToSkip, std::ios::cur);
    }

    // Data start position
    std::streampos dataBegin = inputFile.tellg();

    // Data size: total file size - data start position - 1 (padding byte)
    // Convert std::streampos to std::streamoff for subtraction
    std::streamoff dataSizeOffset = fileSize - static_cast<std::streamoff>(dataBegin) - static_cast<std::streamoff>(1);
    size_t dataSize = static_cast<size_t>(dataSizeOffset);

    // Reposition to the start of the data section
    inputFile.seekg(dataBegin, std::ios::beg);

    // Build Huffman tree
    HuffmanTree tree;
    tree.buildTreeFromCodeTable(inverseCodeTable);

    // Open output file
    std::ofstream outputFile(outputPath, std::ios::binary);
    if (!outputFile.is_open()) {
        throw std::runtime_error("Unable to open output file: " + outputPath);
    }

    // Initialize BitReader with data size
    BitReader bitReader(inputFile, dataSize);

    // Decode data
    std::shared_ptr<HuffmanNode> currentNode = tree.root;
    int decodedChars = 0;
    bool bitValue;

    while (decodedChars < totalChars && bitReader.readBit(bitValue)) {
        if (bitValue) {
            if (currentNode->right) {
                currentNode = currentNode->right;
            } else {
                throw std::runtime_error("Decoding error: invalid bit sequence");
            }
        } else {
            if (currentNode->left) {
                currentNode = currentNode->left;
            } else {
                throw std::runtime_error("Decoding error: invalid bit sequence");
            }
        }

        if (currentNode->isLeaf()) {
            outputFile.write(reinterpret_cast<const char*>(&currentNode->character), sizeof(ORIGINAL_DATA_TYPE));
            decodedChars++;
            currentNode = tree.root;
        }
    }

    // Close files
    inputFile.close();
    outputFile.close();

    std::cout << "Decompression complete!" << std::endl;
}
