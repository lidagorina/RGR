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

static const char ALPHABET[] = 
    "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ"
    "абвгдеёжзийклмнопрстуфхцчшщъыьэюя"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    " !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";

static const size_t ALPHABET_SIZE = sizeof(ALPHABET) - 1;

static int findCharIndex(char c) {
    for (size_t i = 0; i < ALPHABET_SIZE; ++i) {
        if (ALPHABET[i] == c) return static_cast<int>(i);
    }
    return -1;
}

extern "C" {

struct AlgorithmInfo {
    const char* name;
    size_t key_size;
    size_t block_size;
    const char* key_info;
};

EXPORT size_t getMinKeySize() { 
    return 1;
}

EXPORT size_t getMaxKeySize() { 
    return 255;
}

// название алгоритма
EXPORT const AlgorithmInfo* get_algorithm_info() {
    static AlgorithmInfo info = {
        "Цезарь",
        1,
        0,
        "число от 1 до 255"
    };
    return &info;
}

// шифрование
EXPORT const char* encrypt_text(const char* text, unsigned char key) {
    static string result;
    result.clear();
    
    int shift = key % ALPHABET_SIZE;
    if (shift == 0) shift = 3;
    
    size_t len = strlen(text);
    for (size_t i = 0; i < len; ++i) {
        char c = text[i];
        int idx = findCharIndex(c);
        if (idx == -1) {
            result += c;
        } else {
            int newIdx = (idx + shift) % ALPHABET_SIZE;
            result += ALPHABET[newIdx];
        }
    }
    return result.c_str();
}

// дешифрование
EXPORT const char* decrypt_text(const char* text, unsigned char key) {
    static string result;
    result.clear();
    
    int shift = key % ALPHABET_SIZE;
    if (shift == 0) shift = 3;
    
    size_t len = strlen(text);
    for (size_t i = 0; i < len; ++i) {
        char c = text[i];
        int idx = findCharIndex(c);
        if (idx == -1) {
            result += c;
        } else {
            int newIdx = (idx - shift + ALPHABET_SIZE) % ALPHABET_SIZE;
            result += ALPHABET[newIdx];
        }
    }
    return result.c_str();
}

// генерация ключа
EXPORT unsigned char generate_key() {
    srand(static_cast<unsigned>(time(nullptr)));
    return static_cast<unsigned char>(1 + rand() % 255);
}

EXPORT void free_memory(void* ptr) {
   
}

}
