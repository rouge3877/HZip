#ifndef HUFFMAN_NODE_H
#define HUFFMAN_NODE_H

#include <memory>
#include <vector>
#include <unordered_map>

// Define original data type
using ORIGINAL_DATA_TYPE = unsigned char;

// Huffman node structure
struct HuffmanNode {
    ORIGINAL_DATA_TYPE character;
    int frequency;
    std::shared_ptr<HuffmanNode> left;
    std::shared_ptr<HuffmanNode> right;

    // Constructor
    HuffmanNode(ORIGINAL_DATA_TYPE ch, int freq)
        : character(ch), frequency(freq), left(nullptr), right(nullptr) {}

    // Check if the node is a leaf
    bool isLeaf() const {
        return !left && !right;
    }
};

#endif // HUFFMAN_NODE_H
