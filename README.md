# Discord Token Grabber

## 概要

C++で組んでObfuscator-LLVMで難読化したらアンチウイルスは検知するのか？という実験

## 環境

Arch Linux

## コンパイル

### Linux

```sh
clang++ main.cpp -o main -O3
strip --strip-unneeded main.exe
```

### Windows

```sh
clang++ main.cpp -o main.exe -target x86_64-pc-windows-gnu -pthread -lws2_32 -O3
strip --strip-unneeded main.exe
```
