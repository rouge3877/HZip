# HuffmanCoding

## 简介

HuffmanCoding 是一个基于哈夫曼编码的文件压缩和解压工具。它能够高效地压缩文本文件，并在需要时将其解压回原始状态。

## 构建

本项目使用 CMake 进行构建。确保你的系统上已安装 CMake 和支持的 C++ 编译器（如 g++ 或 clang++）。

### 步骤

1. 克隆仓库：

    ```bash
    git clone <repository_url>
    cd HuffmanCoding
    ```

2. 创建构建目录并进入：

    ```bash
    mkdir build
    cd build
    ```

3. 运行 CMake 并构建项目：

    ```bash
    cmake ..
    make
    ```

4. 构建完成后，可执行文件 `hzip` 将位于 `build/` 目录下。

## 使用

```bash
hzip -[c|d] <infile> <outfile>
```

- `-c`：压缩文件
- `-d`：解压文件
- `<infile>`：输入文件
- `<outfile>`：输出文件

## 示例

```bash
hzip -c input.txt output.huff
hzip -d output.huff output.txt
```
## TODO

1. 支持二进制文件的压缩和解压
2. 支持多线程压缩和解压
3. 支持压缩文件的加密和解密
