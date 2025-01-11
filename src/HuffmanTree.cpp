// src/HuffmanTree.cpp
#include "HuffmanTree.h"
#include <queue>
#include <stdexcept>

// VectorBoolHash implementation
std::size_t VectorBoolHash::operator()(const std::vector<bool>& v) const {
    std::size_t hash = 0;
    for (bool bit : v) {
        hash = (hash << 1) | bit;
    }
    return hash;
}

// HuffmanNode implementation
HuffmanNode::HuffmanNode(ORIGINAL_DATA_TYPE ch, int freq)
    : character(ch), frequency(freq), left(nullptr), right(nullptr) {}

bool HuffmanNode::isLeaf() const {
    return !left && !right;
}

// CompareNode implementation
bool CompareNode::operator()(const std::shared_ptr<HuffmanNode>& a, const std::shared_ptr<HuffmanNode>& b) const {
    return a->frequency > b->frequency;
}

// HuffmanTree implementation
void HuffmanTree::buildTree(const std::unordered_map<ORIGINAL_DATA_TYPE, int>& frequencyMap) {
    std::priority_queue<std::shared_ptr<HuffmanNode>, std::vector<std::shared_ptr<HuffmanNode>>, CompareNode> minHeap;

    // Create leaf nodes and add them to the priority queue
    for (const auto& [ch, freq] : frequencyMap) {
        minHeap.emplace(std::make_shared<HuffmanNode>(ch, freq));
    }

    // Handle special case: only one unique character in the file
    if (minHeap.size() == 1) {
        auto onlyNode = minHeap.top();
        minHeap.pop();
        auto parent = std::make_shared<HuffmanNode>(0, onlyNode->frequency);
        parent->left = onlyNode;
        minHeap.emplace(parent);
    }

    // Build the Huffman tree
    while (minHeap.size() > 1) {
        auto left = minHeap.top();
        minHeap.pop();
        auto right = minHeap.top();
        minHeap.pop();

        auto parent = std::make_shared<HuffmanNode>(0, left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;

        minHeap.emplace(parent);
    }

    // Root node of the tree
    if (!minHeap.empty()) {
        root = minHeap.top();
        minHeap.pop();
    }
}

void HuffmanTree::buildTreeFromCodeTable(const std::unordered_map<std::vector<bool>, ORIGINAL_DATA_TYPE, VectorBoolHash>& inverseCodeTable) {
    root = std::make_shared<HuffmanNode>(0, 0); // Create an empty root node

    for (const auto& [code, ch] : inverseCodeTable) {
        auto currentNode = root;
        for (size_t i = 0; i < code.size(); ++i) {
            bool bit = code[i];
            if (bit) { // Right child node
                if (!currentNode->right) {
                    currentNode->right = std::make_shared<HuffmanNode>(0, 0);
                }
                currentNode = currentNode->right;
            } else { // Left child node
                if (!currentNode->left) {
                    currentNode->left = std::make_shared<HuffmanNode>(0, 0);
                }
                currentNode = currentNode->left;
            }
        }
        currentNode->character = ch; // Assign value to leaf node
    }
}

void HuffmanTree::generateCodeTable() {
    if (!root) return;
    std::vector<bool> currentCode;
    traverse(root, currentCode);
}

void HuffmanTree::traverse(const std::shared_ptr<HuffmanNode>& node, std::vector<bool>& currentCode) {
    if (node->isLeaf()) {
        codeTable[node->character] = currentCode;
        return;
    }
    if (node->left) {
        currentCode.push_back(false);
        traverse(node->left, currentCode);
        currentCode.pop_back();
    }
    if (node->right) {
        currentCode.push_back(true);
        traverse(node->right, currentCode);
        currentCode.pop_back();
    }
}
