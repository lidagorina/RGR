#ifndef PLUGIN_API_H
#define PLUGIN_API_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>

// буфер для входных данных 
struct ConstBuffer {
    const uint8_t* data;
    size_t size;
};

// буфер для выходных данных 
struct MutBuffer {
    uint8_t* data;
    size_t size;
};

// информация об алгоритме
struct AlgorithmInfo {
    const char* name;
    size_t key_size;
    size_t block_size;
    const char* key_info;
};

// указатели на функции плагина
using GetAlgorithmInfoFunc = const AlgorithmInfo* (*)();
using GetOutputSizeFunc = size_t (*)(size_t input_size, int operation_type);
using EncryptFunc = int (*)(ConstBuffer key, ConstBuffer input, MutBuffer* output);
using DecryptFunc = int (*)(ConstBuffer key, ConstBuffer input, MutBuffer* output);
using GenerateKeyFunc = int (*)(MutBuffer* key, int param);
using FreeMemoryFunc = void (*)(void* ptr);

// загруженный плагин
struct Plugin {
    std::string name;
    std::string path;
    void* handle = nullptr;
    
    GetAlgorithmInfoFunc get_info = nullptr;
    GetOutputSizeFunc get_output_size = nullptr;
    EncryptFunc encrypt = nullptr;
    DecryptFunc decrypt = nullptr;
    GenerateKeyFunc generate_key = nullptr;
    FreeMemoryFunc free_memory = nullptr;
    
    bool supports_data_encryption = true;
};

#endif
