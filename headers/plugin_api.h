#ifndef PLUGIN_API_H
#define PLUGIN_API_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>

using namespace std;

struct AlgorithmInfo {
    const char* name;
    size_t key_size;
    size_t block_size;
    const char* key_info;
};

using GetAlgorithmInfoFunc = const AlgorithmInfo* (*)();
using GetMinKeySizeFunc = size_t (*)();              
using GetMaxKeySizeFunc = size_t (*)();
using EncryptFunc = const char* (*)(const char* text, unsigned char key);
using DecryptFunc = const char* (*)(const char* text, unsigned char key);
using GenerateKeyFunc = unsigned char (*)();
using FreeMemoryFunc = void (*)(void* ptr);

struct Plugin {
    string name;
    string path;
    void* handle = nullptr;
    
    GetAlgorithmInfoFunc get_info = nullptr;
    GetMinKeySizeFunc getMinKeySize = nullptr;        
    GetMaxKeySizeFunc getMaxKeySize = nullptr;
    EncryptFunc encrypt = nullptr;
    DecryptFunc decrypt = nullptr;
    GenerateKeyFunc generate_key = nullptr;
    FreeMemoryFunc free_memory = nullptr;
};

#endif
