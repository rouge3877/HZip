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

// 工具函数声明（假设在 Utils.h 中）
std::string getAbsolutePath(const std::string& filename);

void HuffmanEncoder::compress(const std::string& inputPath, const std::string& outputPath) {
    // 获取原始文件大小
    size_t originalSize = 0;
    try {
        originalSize = fs::file_size(inputPath);
    } catch (const fs::filesystem_error& e) {
        throw std::runtime_error("无法获取输入文件大小: " + std::string(e.what()));
    }

    // 读取输入文件并统计字符频率
    std::ifstream inputFile(inputPath, std::ios::binary);
    if (!inputFile.is_open()) {
        throw std::runtime_error("无法打开输入文件: " + inputPath);
    }

    std::unordered_map<ORIGINAL_DATA_TYPE, int> frequencyMap;
    int totalChars = 0;
    ORIGINAL_DATA_TYPE ch;
    while (inputFile.read(reinterpret_cast<char*>(&ch), sizeof(ORIGINAL_DATA_TYPE))) {
        frequencyMap[ch]++;
        totalChars++;
    }

    if (totalChars == 0) {
        throw std::runtime_error("输入文件为空");
    }

    // 打印字符频率到标准错误
    std::cerr << "字符频率统计:\n";
    for (const auto& [charKey, freq] : frequencyMap) {
        if (std::isprint(charKey)) {
            std::cerr << " '" << charKey << "': " << freq << "\n";
        } else {
            std::cerr << " '\\x" << std::hex << static_cast<int>(charKey) << std::dec << "': " << freq << "\n";
        }
    }

    // 构建哈夫曼树并生成编码表
    HuffmanTree tree;
    tree.buildTree(frequencyMap);
    tree.generateCodeTable();

    // 打印哈夫曼编码表到标准错误
    std::cerr << "\n哈夫曼编码表:\n";
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

    // 计算头部大小
    size_t headerSize = 1 + 4; // tableSize (1 byte) + totalChars (4 bytes)
    for (const auto& [charKey, code] : tree.codeTable) {
        headerSize += 1 + 1; // character (1 byte) + codeLength (1 byte)
        headerSize += (code.size() + 7) / 8; // packed code bits
    }

    // 计算压缩后的数据大小（以比特为单位）
    size_t compressedDataBits = 0;
    for (const auto& [charKey, freq] : frequencyMap) {
        compressedDataBits += freq * tree.codeTable[charKey].size();
    }
    size_t compressedDataBytes = (compressedDataBits + 7) / 8; // 向上取整
    compressedDataBytes += 1; // padding byte

    size_t totalCompressedSize = headerSize + compressedDataBytes;

    // 比较压缩后的大小与原始文件大小
    if (totalCompressedSize >= originalSize) {
        // 不进行压缩，直接复制原始文件到输出文件
        std::cerr << "\n压缩后的文件大小（" << totalCompressedSize << " 字节）不小于原始文件大小（" << originalSize << " 字节）。\n";
        std::cerr << "跳过压缩，直接复制原始文件到输出文件。\n";

        // 重新定位到文件开头
        inputFile.clear();
        inputFile.seekg(0, std::ios::beg);

        // 打开输出文件
        std::ofstream outputFile(outputPath, std::ios::binary);
        if (!outputFile.is_open()) {
            throw std::runtime_error("无法打开输出文件: " + outputPath);
        }

        // 复制文件内容
        outputFile << inputFile.rdbuf();

        // 关闭文件
        inputFile.close();
        outputFile.close();

        return;
    }

    // 打开输出文件
    std::ofstream outputFile(outputPath, std::ios::binary);
    if (!outputFile.is_open()) {
        throw std::runtime_error("无法打开输出文件: " + outputPath);
    }

    // 写入编码表的大小（1字节）
    uint8_t tableSize = static_cast<uint8_t>(tree.codeTable.size());
    outputFile.write(reinterpret_cast<const char*>(&tableSize), sizeof(uint8_t));

    // 写入总字符数（4字节）
    outputFile.write(reinterpret_cast<const char*>(&totalChars), sizeof(int));

    // 写入编码表
    for (const auto& [charKey, code] : tree.codeTable) {
        // 写入字符（1字节）
        outputFile.write(reinterpret_cast<const char*>(&charKey), sizeof(ORIGINAL_DATA_TYPE));

        // 写入编码长度（1字节）
        uint8_t codeLength = static_cast<uint8_t>(code.size());
        outputFile.write(reinterpret_cast<const char*>(&codeLength), sizeof(uint8_t));

        // 将编码比特打包成字节
        std::vector<uint8_t> packedBytes = packBits(code);

        // 写入打包后的字节
        if (!packedBytes.empty()) {
            outputFile.write(reinterpret_cast<const char*>(packedBytes.data()), packedBytes.size());
        }
    }

    // 使用BitWriter写入编码后的数据
    BitWriter bitWriter(outputFile);

    // 重新定位到文件开头
    inputFile.clear();
    inputFile.seekg(0, std::ios::beg);

    // 将编码写入缓冲区
    while (inputFile.read(reinterpret_cast<char*>(&ch), sizeof(ORIGINAL_DATA_TYPE))) {
        const auto& code = tree.codeTable[ch];
        bitWriter.writeBits(code);
    }

    // 刷新缓冲区并获取填充位数
    int padding = bitWriter.getBitCount() > 0 ? (8 - bitWriter.getBitCount()) : 0;
    bitWriter.flush();

    // 写入填充位数（1字节）
    uint8_t paddingByte = static_cast<uint8_t>(padding);
    outputFile.write(reinterpret_cast<const char*>(&paddingByte), sizeof(uint8_t));

    // 关闭文件
    inputFile.close();
    outputFile.close();

    // 计算并打印压缩率和头部大小到标准错误
    double compressionRatio = (static_cast<double>(totalCompressedSize) / static_cast<double>(originalSize)) * 100.0;

    std::cerr << "\n压缩完成！\n";
    std::cerr << "输入文件大小: " << originalSize << " 字节\n";
    std::cerr << "头部大小: " << headerSize << " 字节\n";
    std::cerr << "压缩后的文件大小: " << totalCompressedSize << " 字节\n";
    std::cerr << "压缩率: " << std::fixed << std::setprecision(2) << compressionRatio << "%\n";
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
