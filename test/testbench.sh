#!/bin/bash

# ...existing code...

if [ -z "$1" ]; then
    echo "Usage: $0 <input-file>"
    exit 1
fi

input_file="$1"
huff_bin="../build/huff"


mkdir -p zip-result
mkdir -p unzip-result

before_size=$(stat -c%s "$input_file")
compressed_file="zip-result/$(basename "$input_file").huff"
uncompressed_file="unzip-result/$(basename "$input_file")"

$huff_bin -c "$input_file" "$compressed_file"
after_size=$(stat -c%s "$compressed_file")
$huff_bin -d "$compressed_file" "$uncompressed_file"

echo "Original size: $before_size"
echo "Compressed size: $after_size"

diff "$input_file" "$uncompressed_file" && echo "Files are identical" || echo "Files differ"

# ...existing code...
echo "Test completed."