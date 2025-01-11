// src/HuffmanDecoder.cpp
#include "HuffmanDecoder.h"
#include "HuffmanTree.h"
#include "BitIO.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <filesystem>

// 工具函数声明（假设在 Utils.h 中）
std::string getAbsolutePath(const std::string& filename);

void HuffmanDecoder::decompress(const std::string& inputPath, const std::string& outputPath) {
    // 打开输入文件
    std::ifstream inputFile(inputPath, std::ios::binary);
    if (!inputFile.is_open()) {
        throw std::runtime_error("无法打开输入文件: " + inputPath);
    }

    // 获取文件大小
    inputFile.seekg(0, std::ios::end);
    std::streampos fileSize = inputFile.tellg();

    // 回到文件开头
    inputFile.seekg(0, std::ios::beg);

    // 读取编码表的大小（1字节）
    uint8_t tableSize = 0;
    if (!inputFile.read(reinterpret_cast<char*>(&tableSize), sizeof(uint8_t))) {
        throw std::runtime_error("无法读取编码表大小");
    }

    // 读取总字符数（4字节）
    int totalChars = 0;
    if (!inputFile.read(reinterpret_cast<char*>(&totalChars), sizeof(int))) {
        throw std::runtime_error("无法读取总字符数");
    }

    // 读取编码表
    std::unordered_map<std::vector<bool>, ORIGINAL_DATA_TYPE, VectorBoolHash> inverseCodeTable;
    for (int i = 0; i < tableSize; ++i) {
        // 读取字符（1字节）
        ORIGINAL_DATA_TYPE charKey;
        if (!inputFile.read(reinterpret_cast<char*>(&charKey), sizeof(ORIGINAL_DATA_TYPE))) {
            throw std::runtime_error("无法读取编码表中的字符");
        }

        // 读取编码长度（1字节）
        uint8_t codeLength = 0;
        if (!inputFile.read(reinterpret_cast<char*>(&codeLength), sizeof(uint8_t))) {
            throw std::runtime_error("无法读取编码长度");
        }

        // 计算需要读取的字节数
        int bytesNeeded = (codeLength + 7) / 8;

        // 读取编码比特
        std::vector<uint8_t> packedBytes(bytesNeeded);
        if (!inputFile.read(reinterpret_cast<char*>(packedBytes.data()), bytesNeeded)) {
            throw std::runtime_error("无法读取编码比特");
        }

        // 解包比特
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

    // 计算填充位的位置
    // 填充位在文件的最后一个字节
    if (fileSize < 1) {
        throw std::runtime_error("文件过小，无法包含填充位");
    }
    inputFile.seekg(-static_cast<std::streamoff>(1), std::ios::end);
    uint8_t padding = 0;
    if (!inputFile.read(reinterpret_cast<char*>(&padding), sizeof(uint8_t))) {
        throw std::runtime_error("无法读取填充位数");
    }

    // 计算数据部分的起始位置和长度
    // 注意：此处的逻辑需要确保正确计算数据的起始位置和大小
    // 首先，跳过头部信息（tableSize 和 totalChars）
    inputFile.seekg(1 + sizeof(int), std::ios::beg); // 跳过 tableSize 和 totalChars

    // 跳过编码表
    for (int i = 0; i < tableSize; ++i) {
        // 跳过字符（1字节）和编码长度（1字节）
        inputFile.seekg(1 + 1, std::ios::cur);
        // 读取编码长度
        uint8_t codeLength = 0;
        inputFile.seekg(-1, std::ios::cur); // Move back to read codeLength
        if (!inputFile.read(reinterpret_cast<char*>(&codeLength), sizeof(uint8_t))) {
            throw std::runtime_error("无法读取编码长度");
        }
        // 计算需要跳过的字节数
        int bytesToSkip = (codeLength + 7) / 8;
        inputFile.seekg(bytesToSkip, std::ios::cur);
    }

    // 数据开始的位置
    std::streampos dataBegin = inputFile.tellg();

    // 数据大小：文件总大小 - 数据开始位置 - 1（填充位）
    // 需要将 std::streampos 转换为 std::streamoff 以进行减法
    std::streamoff dataSizeOffset = fileSize - static_cast<std::streamoff>(dataBegin) - static_cast<std::streamoff>(1);
    size_t dataSize = static_cast<size_t>(dataSizeOffset);

    // 重新定位到数据开始的位置
    inputFile.seekg(dataBegin, std::ios::beg);

    // 构建哈夫曼树
    HuffmanTree tree;
    tree.buildTreeFromCodeTable(inverseCodeTable);

    // 打开输出文件
    std::ofstream outputFile(outputPath, std::ios::binary);
    if (!outputFile.is_open()) {
        throw std::runtime_error("无法打开输出文件: " + outputPath);
    }

    // 初始化BitReader，传入数据大小
    BitReader bitReader(inputFile, dataSize);

    // 解码数据
    std::shared_ptr<HuffmanNode> currentNode = tree.root;
    int decodedChars = 0;
    bool bitValue;

    while (decodedChars < totalChars && bitReader.readBit(bitValue)) {
        if (bitValue) {
            if (currentNode->right) {
                currentNode = currentNode->right;
            } else {
                throw std::runtime_error("解码错误：无效的比特序列");
            }
        } else {
            if (currentNode->left) {
                currentNode = currentNode->left;
            } else {
                throw std::runtime_error("解码错误：无效的比特序列");
            }
        }

        if (currentNode->isLeaf()) {
            outputFile.write(reinterpret_cast<const char*>(&currentNode->character), sizeof(ORIGINAL_DATA_TYPE));
            decodedChars++;
            currentNode = tree.root;
        }
    }

    // 关闭文件
    inputFile.close();
    outputFile.close();

    std::cout << "解压完成！" << std::endl;
}
