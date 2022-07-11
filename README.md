# soothDNS

A simple Domain Name Relay Server writing in pure C

Pure go version is in another branch

## Features

- DNS over HTTPS support
- Efficient cache with LRU and Trie algorithm
- Support loading local resolving files (like hosts.txt)

## Usage

- `make help`
- if you compile it in windows, please config the related lib and remove the valgrind and test part in `Makefile` or using WSL
