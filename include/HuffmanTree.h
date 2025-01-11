// include/HuffmanTree.h
#ifndef HUFFMANTREE_H
#define HUFFMANTREE_H

#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>
#include <cstdint>

using ORIGINAL_DATA_TYPE = unsigned char;

struct VectorBoolHash {
    std::size_t operator()(const std::vector<bool>& v) const;
};

struct HuffmanNode {
    ORIGINAL_DATA_TYPE character;
    int frequency;
    std::shared_ptr<HuffmanNode> left;
    std::shared_ptr<HuffmanNode> right;

    HuffmanNode(ORIGINAL_DATA_TYPE ch, int freq);
    bool isLeaf() const;
};

struct CompareNode {
    bool operator()(const std::shared_ptr<HuffmanNode>& a, const std::shared_ptr<HuffmanNode>& b) const;
};

class HuffmanTree {
public:
    std::shared_ptr<HuffmanNode> root;
    std::unordered_map<ORIGINAL_DATA_TYPE, std::vector<bool>> codeTable;

    void buildTree(const std::unordered_map<ORIGINAL_DATA_TYPE, int>& frequencyMap);
    void buildTreeFromCodeTable(const std::unordered_map<std::vector<bool>, ORIGINAL_DATA_TYPE, VectorBoolHash>& inverseCodeTable);
    void generateCodeTable();

private:
    void traverse(const std::shared_ptr<HuffmanNode>& node, std::vector<bool>& currentCode);
};

#endif // HUFFMANTREE_H
