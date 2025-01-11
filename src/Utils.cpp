// src/Utils.cpp
#include "Utils.h"
#include <filesystem>

namespace fs = std::filesystem;

std::string getAbsolutePath(const std::string& filename) {
    fs::path p = fs::absolute(filename);
    return p.string();
}

void printHelp(std::ostream& out) {
    out << "Usage: huff -[c|d] <infile> <outfile>\n";
    out << "Compress or decompress file using Huffman coding.\n";
    out << "<infile>  Input file, it's required to be in the same directory as the executable file.\n";
    out << "<outfile> Output file, it's required to be in the same directory as the executable file.\n";
    out << "Example: huff -c input.txt output.huff\n";
    out << "         huff -d output.huff recovered.txt\n";
    out << "Options:\n";
    out << "  -c  Compress infile to outfile\n";
    out << "  -d  Decompress infile to outfile\n";
    out << "  -h, --help  Show this help message\n";
}
