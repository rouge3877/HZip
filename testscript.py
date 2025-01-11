#!/usr/bin/env python3

import os
import sys
import subprocess

def main():
    """
    Compress and decompress the input file using the Huffman tool
    and verify if the decompressed file matches the original.
    """
    if len(sys.argv) < 2:
        print("\033[1;31mUsage: {} <input-file>\033[0m".format(sys.argv[0]))
        sys.exit(1)

    input_file = sys.argv[1]
    huff_bin = "./build/hzip"
    compressed_file = os.path.join("test", "zip-result", os.path.basename(input_file) + ".huff")
    uncompressed_file = os.path.join("test", "unzip-result", os.path.basename(input_file))

    os.makedirs(os.path.dirname(compressed_file), exist_ok=True)
    os.makedirs(os.path.dirname(uncompressed_file), exist_ok=True)

    before_size = os.path.getsize(input_file)
    subprocess.run([huff_bin, "-c", input_file, compressed_file], check=True)
    after_size = os.path.getsize(compressed_file)
    subprocess.run([huff_bin, "-d", compressed_file, uncompressed_file], check=True)

    print("\033[1;34mOriginal size:\033[0m", before_size)
    print("\033[1;34mCompressed size:\033[0m", after_size)

    with open(input_file, "rb") as f1, open(uncompressed_file, "rb") as f2:
        if f1.read() == f2.read():
            print("\033[1;32mFiles are identical\033[0m")
        else:
            print("\033[1;31mFiles differ\033[0m")

    print("\033[1;36mTest completed.\033[0m")

if __name__ == "__main__":
    main()