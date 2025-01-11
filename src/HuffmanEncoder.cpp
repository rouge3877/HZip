// src/HuffmanEncoder.cpp
#include "HuffmanEncoder.h"
#include "HuffmanTree.h"
#include "BitIO.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <iomanip> // For std::hex and std::dec

namespace fs = std::filesystem;

// Utility function declaration (assumed to be in Utils.h)
std::string getAbsolutePath(const std::string& filename);

void HuffmanEncoder::compress(const std::string& inputPath, const std::string& outputPath) {
    // Get the original file size
    size_t originalSize = 0;
    try {
        originalSize = fs::file_size(inputPath);
    } catch (const fs::filesystem_error& e) {
        throw std::runtime_error("Unable to get input file size: " + std::string(e.what()));
    }

    // Read the input file and count character frequencies
    std::ifstream inputFile(inputPath, std::ios::binary);
    if (!inputFile.is_open()) {
        throw std::runtime_error("Unable to open input file: " + inputPath);
    }

    std::unordered_map<ORIGINAL_DATA_TYPE, int> frequencyMap;
    int totalChars = 0;
    ORIGINAL_DATA_TYPE ch;
    while (inputFile.read(reinterpret_cast<char*>(&ch), sizeof(ORIGINAL_DATA_TYPE))) {
        frequencyMap[ch]++;
        totalChars++;
    }

    if (totalChars == 0) {
        throw std::runtime_error("Input file is empty");
    }

    // Print character frequencies to standard error
    std::cerr << "Character frequency statistics:\n";
    for (const auto& [charKey, freq] : frequencyMap) {
        if (std::isprint(charKey)) {
            std::cerr << " '" << charKey << "': " << freq << "\n";
        } else {
            std::cerr << " '\\x" << std::hex << static_cast<int>(charKey) << std::dec << "': " << freq << "\n";
        }
    }

    // Build Huffman tree and generate code table
    HuffmanTree tree;
    tree.buildTree(frequencyMap);
    tree.generateCodeTable();

    // Print Huffman code table to standard error
    std::cerr << "\nHuffman code table:\n";
    for (const auto& [charKey, code] : tree.codeTable) {
        if (std::isprint(charKey)) {
            std::cerr << " '" << charKey << "': ";
        } else {
            std::cerr << " '\\x" << std::hex << static_cast<int>(charKey) << std::dec << "': ";
        }
        for (bool bit : code) {
            std::cerr << bit;
        }
        std::cerr << "\n";
    }

    // Calculate header size
    size_t headerSize = 1 + 4; // tableSize (1 byte) + totalChars (4 bytes)
    for (const auto& [charKey, code] : tree.codeTable) {
        headerSize += 1 + 1; // character (1 byte) + codeLength (1 byte)
        headerSize += (code.size() + 7) / 8; // packed code bits
    }

    // Calculate compressed data size (in bits)
    size_t compressedDataBits = 0;
    for (const auto& [charKey, freq] : frequencyMap) {
        compressedDataBits += freq * tree.codeTable[charKey].size();
    }
    size_t compressedDataBytes = (compressedDataBits + 7) / 8; // Round up
    compressedDataBytes += 1; // padding byte

    size_t totalCompressedSize = headerSize + compressedDataBytes;

    // Compare compressed size with original file size
    if (totalCompressedSize >= originalSize) {
        // Skip compression and copy the original file to the output file
        std::cerr << "\nCompressed file size (" << totalCompressedSize << " bytes) is not smaller than the original file size (" << originalSize << " bytes).\n";
        std::cerr << "Skipping compression and copying the original file to the output file.\n";

        // Rewind to the beginning of the file
        inputFile.clear();
        inputFile.seekg(0, std::ios::beg);

        // Open the output file
        std::ofstream outputFile(outputPath, std::ios::binary);
        if (!outputFile.is_open()) {
            throw std::runtime_error("Unable to open output file: " + outputPath);
        }

        // Copy file content
        outputFile << inputFile.rdbuf();

        // Close files
        inputFile.close();
        outputFile.close();

        return;
    }

    // Open the output file
    std::ofstream outputFile(outputPath, std::ios::binary);
    if (!outputFile.is_open()) {
        throw std::runtime_error("Unable to open output file: " + outputPath);
    }

    // Write the size of the code table (1 byte)
    uint8_t tableSize = static_cast<uint8_t>(tree.codeTable.size());
    outputFile.write(reinterpret_cast<const char*>(&tableSize), sizeof(uint8_t));

    // Write the total number of characters (4 bytes)
    outputFile.write(reinterpret_cast<const char*>(&totalChars), sizeof(int));

    // Write the code table
    for (const auto& [charKey, code] : tree.codeTable) {
        // Write the character (1 byte)
        outputFile.write(reinterpret_cast<const char*>(&charKey), sizeof(ORIGINAL_DATA_TYPE));

        // Write the code length (1 byte)
        uint8_t codeLength = static_cast<uint8_t>(code.size());
        outputFile.write(reinterpret_cast<const char*>(&codeLength), sizeof(uint8_t));

        // Pack the code bits into bytes
        std::vector<uint8_t> packedBytes = packBits(code);

        // Write the packed bytes
        if (!packedBytes.empty()) {
            outputFile.write(reinterpret_cast<const char*>(packedBytes.data()), packedBytes.size());
        }
    }

    // Use BitWriter to write the encoded data
    BitWriter bitWriter(outputFile);

    // Rewind to the beginning of the file
    inputFile.clear();
    inputFile.seekg(0, std::ios::beg);

    // Write the encoded data to the buffer
    while (inputFile.read(reinterpret_cast<char*>(&ch), sizeof(ORIGINAL_DATA_TYPE))) {
        const auto& code = tree.codeTable[ch];
        bitWriter.writeBits(code);
    }

    // Flush the buffer and get the number of padding bits
    int padding = bitWriter.getBitCount() > 0 ? (8 - bitWriter.getBitCount()) : 0;
    bitWriter.flush();

    // Write the number of padding bits (1 byte)
    uint8_t paddingByte = static_cast<uint8_t>(padding);
    outputFile.write(reinterpret_cast<const char*>(&paddingByte), sizeof(uint8_t));

    // Close files
    inputFile.close();
    outputFile.close();

    // Calculate and print compression ratio and header size to standard error
    double compressionRatio = (static_cast<double>(totalCompressedSize) / static_cast<double>(originalSize)) * 100.0;

    std::cerr << "\nCompression complete!\n";
    std::cerr << "Input file size: " << originalSize << " bytes\n";
    std::cerr << "Header size: " << headerSize << " bytes\n";
    std::cerr << "Compressed file size: " << totalCompressedSize << " bytes\n";
    std::cerr << "Compression ratio: " << std::fixed << std::setprecision(2) << compressionRatio << "%\n";
}

std::vector<uint8_t> HuffmanEncoder::packBits(const std::vector<bool>& bits) {
    std::vector<uint8_t> packedBytes;
    size_t i = 0;
    while (i < bits.size()) {
        uint8_t byte = 0;
        for (int bit = 0; bit < 8; ++bit) {
            byte <<= 1;
            if (i < bits.size()) {
                byte |= bits[i++] ? 1 : 0;
            }
        }
        packedBytes.push_back(byte);
    }
    return packedBytes;
}
