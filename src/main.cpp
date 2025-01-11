// src/main.cpp
#include "HuffmanEncoder.h"
#include "HuffmanDecoder.h"
#include "Utils.h"
#include <iostream>
#include <cstring>
#include <cstdlib>

int main(int argc, char* argv[]) {
    try {
        if (argc != 4) {
            if (argc == 2 && (std::strcmp(argv[1], "-h") == 0 || std::strcmp(argv[1], "--help") == 0)) {
                printHelp(std::cout);
                return EXIT_SUCCESS;
            }
            if (argc > 4)
                std::cerr << "Too many arguments\n\n";
            else
                std::cerr << "Too few arguments\n\n";
            printHelp(std::cout);
            return EXIT_FAILURE;
        }

        std::string option = argv[1];
        std::string inputPath = getAbsolutePath(argv[2]);
        std::string outputPath = getAbsolutePath(argv[3]);

        if (option == "-c") {
            HuffmanEncoder encoder;
            encoder.compress(inputPath, outputPath);
        } else if (option == "-d") {
            HuffmanDecoder decoder;
            decoder.decompress(inputPath, outputPath);
        } else {
            std::cerr << "Unknown command\n\n";
            printHelp(std::cout);
            return EXIT_FAILURE;
        }

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
