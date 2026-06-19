#include <cstdlib>
#include <cstring>
#include <string>

using namespace std;

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

// зеркальное отображение байта (0↔255, 1↔254, ...)
static uint8_t atbashByte(uint8_t byte) {
    return 255 - byte;
}

extern "C" {

struct AlgorithmInfo {
    const char* name;
    size_t key_size;
    size_t block_size;
    const char* key_info;
};

// информация об алгоритме
EXPORT const AlgorithmInfo* get_algorithm_info() {
    static AlgorithmInfo info = {
        "Atbash",
        0,
        0,
        "зеркальное отображение байтов (0↔255), ключ не требуется"
    };
    return &info;
}

// минимальный и максимальный размер ключа (ключ не нужен)
EXPORT size_t getMinKeySize() { 
    return 0;
}

EXPORT size_t getMaxKeySize() { 
    return 0;
}

// шифрование
EXPORT const char* encrypt_text(const char* text, unsigned char key) {
    static string result;
    result.clear();
    (void)key;
    
    size_t len = strlen(text);
    for (size_t i = 0; i < len; ++i) {
        uint8_t byte = static_cast<uint8_t>(text[i]);
        result += static_cast<char>(atbashByte(byte));
    }
    return result.c_str();
}

// дешифрование (Atbash симметричен)
EXPORT const char* decrypt_text(const char* text, unsigned char key) {
    return encrypt_text(text, key);
}

// генерация ключа (Atbash не требует ключа)
EXPORT unsigned char generate_key() {
    return 0;
}

EXPORT void free_memory(void* ptr) {
    // ничего не делаем
}

}
