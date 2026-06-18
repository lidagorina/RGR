#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <string>

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__ ((visibility("default")))
#endif

static const uint8_t MIN_KEY = 1;
static const uint8_t MAX_KEY = 256;

extern "C" {


// название
EXPORT const char* getAlgorithmName() {
    return "XOR шифр";
}

//инфа о ключе
EXPORT const char* getKeyInfo() {
    static std::string info = 
        "Ключ: строка от 1 до 256 байт. Пример: mySecretKey123";
    return info.c_str();
}

//миниальный ключ
EXPORT size_t getMinKeySize() { 
    return MIN_KEY;
}

// максиальный клчю
EXPORT size_t getMaxKeySize() { 
    return MAX_KEY;
}

// шифрование
EXPORT int encrypt(const uint8_t* data, size_t dataSize,
                  const uint8_t* key, size_t keySize,
                  uint8_t* output, size_t* outputSize) {
    if (!data || !key || !output || !outputSize) return -1;
    if (keySize == 0) return -2;
    if (*outputSize < dataSize) return -3;
    
    for (size_t i = 0; i < dataSize; ++i) {
        output[i] = data[i] ^ key[i % keySize];
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
    
    for (size_t i = 0; i < dataSize; ++i) {
        output[i] = data[i] ^ key[i % keySize];
    }
    
    *outputSize = dataSize;
    return 0;
}

// генерация ключа
EXPORT int generateKey(uint8_t* keyBuffer, size_t* keyBufferSize, int param) {
    if (!keyBuffer || !keyBufferSize) return -1;
    
    size_t keyLen = (param > 0 && param <= 256) ? static_cast<size_t>(param) : 16;
    if (*keyBufferSize < keyLen) return -3;
    
    srand(static_cast<unsigned>(time(nullptr)));
    
    for (size_t i = 0; i < keyLen; ++i) {
        keyBuffer[i] = static_cast<uint8_t>(rand() % 256);
    }
    
    *keyBufferSize = keyLen;
    return 0;
}

}
