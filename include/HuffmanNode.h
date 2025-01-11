#ifndef HUFFMAN_NODE_H
#define HUFFMAN_NODE_H

#include <memory>
#include <vector>
#include <unordered_map>

// 定义原始数据类型
using ORIGINAL_DATA_TYPE = unsigned char;

// 哈夫曼节点结构
struct HuffmanNode {
    ORIGINAL_DATA_TYPE character;
    int frequency;
    std::shared_ptr<HuffmanNode> left;
    std::shared_ptr<HuffmanNode> right;

    // 构造函数
    HuffmanNode(ORIGINAL_DATA_TYPE ch, int freq)
        : character(ch), frequency(freq), left(nullptr), right(nullptr) {}

    // 判断是否为叶子节点
    bool isLeaf() const {
        return !left && !right;
    }
};

#endif // HUFFMAN_NODE_H
