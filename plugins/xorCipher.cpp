#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <string>

using namespace std;

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__ ((visibility("default")))
#endif

extern "C" {

struct AlgorithmInfo {
    const char* name;
    size_t key_size;
    size_t block_size;
    const char* key_info;
};

// название алгоритма
EXPORT const AlgorithmInfo* get_algorithm_info() {
    static AlgorithmInfo info = {
        "XOR",
        1,
        0,
        "число от 1 до 255"
    };
    return &info;
}

EXPORT size_t getMinKeySize() { 
    return 1;
}

EXPORT size_t getMaxKeySize() { 
    return 255;
}


// шифрование
EXPORT const char* encrypt_text(const char* text, unsigned char key) {
    static string result;
    result.clear();
    
    size_t len = strlen(text);
    for (size_t i = 0; i < len; ++i) {
        result += static_cast<char>(text[i] ^ key);
    }
    return result.c_str();
}

// дешифрование (XOR симметричен)
EXPORT const char* decrypt_text(const char* text, unsigned char key) {
    return encrypt_text(text, key);
}

// генерация ключа
EXPORT unsigned char generate_key() {
    srand(static_cast<unsigned>(time(nullptr)));
    return static_cast<unsigned char>(1 + rand() % 255);
}

EXPORT void free_memory(void* ptr) {
}

}
