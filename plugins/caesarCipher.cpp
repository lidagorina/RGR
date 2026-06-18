#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdio>
#include <string>

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

// название алгоритма
EXPORT const char* getAlgorithmName() {
    return "Цезарь (сдвиг)";
}

// инфа о ключе
EXPORT const char* getKeyInfo() {
    static std::string info = 
        "Ключ: число от 1 до " + std::to_string(ALPHABET_SIZE - 1); 
    return info.c_str();
}

// минимальный ключ
EXPORT size_t getMinKeySize() { 
    return 1;
}

// максимальный ключ 
EXPORT size_t getMaxKeySize() { 
    return 5;
}

// шифрование
EXPORT int encrypt(const uint8_t* data, size_t dataSize,
                  const uint8_t* key, size_t keySize,
                  uint8_t* output, size_t* outputSize) {
    if (!data || !key || !output || !outputSize) return -1;
    if (keySize == 0) return -2;
    if (*outputSize < dataSize) return -3;
    
    int shift = 0;
    for (size_t i = 0; i < keySize && i < 5; ++i) {
        if (key[i] < '0' || key[i] > '9') return -2;
        shift = shift * 10 + (key[i] - '0');
    }
    
    shift = shift % ALPHABET_SIZE;
    if (shift == 0) shift = 3;
    
    for (size_t i = 0; i < dataSize; ++i) {
        char c = static_cast<char>(data[i]);
        int idx = findCharIndex(c);
        
        if (idx == -1) {
            output[i] = data[i];
        } else {
            int newIdx = (idx + shift) % ALPHABET_SIZE;
            output[i] = static_cast<uint8_t>(ALPHABET[newIdx]);
        }
    }
    
    *outputSize = dataSize;
    return 0;
}

// дешифрование
EXPORT int decrypt(const uint8_t* data, size_t dataSize,
                  const uint8_t* key, size_t keySize,
                  uint8_t* output, size_t* outputSize) {
    if (!data || !key || !output || !outputSize) return -1;
    if (keySize == 0) return -2;
    if (*outputSize < dataSize) return -3;
    
    int shift = 0;
    for (size_t i = 0; i < keySize && i < 5; ++i) {
        if (key[i] < '0' || key[i] > '9') return -2;
        shift = shift * 10 + (key[i] - '0');
    }
    
    shift = shift % ALPHABET_SIZE;
    if (shift == 0) shift = 3;
    
    for (size_t i = 0; i < dataSize; ++i) {
        char c = static_cast<char>(data[i]);
        int idx = findCharIndex(c);
        
        if (idx == -1) {
            output[i] = data[i];
        } else {
            int newIdx = (idx - shift + ALPHABET_SIZE) % ALPHABET_SIZE;
            output[i] = static_cast<uint8_t>(ALPHABET[newIdx]);
        }
    }
    
    *outputSize = dataSize;
    return 0;
}

// генерация ключа
EXPORT int generateKey(uint8_t* keyBuffer, size_t* keyBufferSize, int param) {
    if (!keyBuffer || !keyBufferSize) return -1;
    
    srand(static_cast<unsigned>(time(nullptr)));
    int shift = 1 + (rand() % (ALPHABET_SIZE - 1));
    
    char buf[16];
    int len = snprintf(buf, sizeof(buf), "%d", shift);
    
    if (static_cast<size_t>(len) >= *keyBufferSize) return -3;
    
    memcpy(keyBuffer, buf, len);
    *keyBufferSize = static_cast<size_t>(len);
    
    return 0;
}

} 
