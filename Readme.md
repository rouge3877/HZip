# HZip

a simple file compression and decompression tool based on Huffman coding (*for educational purposes only*).

## Introduction

HZip is a powerful file compression and decompression tool based on Huffman coding. It efficiently compresses text files and can restore them to their original state when needed. HZip is designed to be fast, reliable, and easy to use.

## Features

- Efficient compression and decompression using Huffman coding
- Simple command-line interface
- Support for text files (*Pity, only ascii text files are supported*)
- Comprehensive testing script

## Build and Usage

This project uses CMake. Ensure that CMake and a supported C++ compiler (e.g., g++ or clang++) are installed on your system.

### Build Steps

1. Clone the repository:
   ```bash
   git clone https://github.com/rouge3877/HZip.git
   cd HZip
   ```
2. Create and navigate to the build directory:
   ```bash
   mkdir build
   cd build
   ```
3. Run CMake and build the project:
   ```bash
   cmake ..
   make
   ```
4. After building, the executable `hzip` will be located in the `build/` directory.

### Usage 

To compress a file:
```bash
./hzip -c <input_file> <output_file>
```
To decompress a file:
```bash
./hzip -d <compressed_file> <output_file>
```

For more advanced options and help:
```bash
./hzip --help
```

## Testing

For testing, you can use the provided script `testscript.sh`:
```bash
./testscript.sh <input_file>
```

This script will compress the input file, decompress the compressed file, and compare the original and decompressed files. It will also automatically create a `test` directory in the root of the project and store the compressed and decompressed files there.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## TODO

- [ ] Add support for non-ASCII text files
- [ ] Add support for different file types (currently, only text files are supported)
- [ ] Add support for directories
- [ ] Implement multithreading for faster compression and decompression
- [ ] Add support for encryption and decryption
