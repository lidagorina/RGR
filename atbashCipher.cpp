#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <ctime>
#include <string>
#include <vector>
#include <sstream>

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__((visibility("default")))
#endif

using namespace std;

static void atbashTransformByte(uint8_t* byte) {
    // Зеркальное отображение ВСЕХ байтов (0-255)
    *byte = 255 - *byte;
}

extern "C" {

EXPORT const char* getAlgorithmName() {
    return "Atbash (зеркальный шифр, универсальный)";
}

EXPORT const char* getKeyInfo() {
    return "Шифр замены на основе зеркального отображения байтов.\n"
           "Поддерживает ВСЕ байты (0-255):\n"
           "  0↔255, 1↔254, 2↔253, ...\n"
           "Инволютивный шифр (шифрование = дешифрование).\n"
           "Не требует ключа.";
}

EXPORT size_t getMinKeySize() { return 0; }

EXPORT size_t getMaxKeySize() { return 0; }

EXPORT int encrypt(const uint8_t* data, size_t dataSize,
                   const uint8_t* key, size_t keySize,
                   uint8_t* output, size_t* outputSize) {
    if (!data || !output || !outputSize) return -1;
    (void)key; (void)keySize;
    
    if (*outputSize < dataSize) return -3;
    
    // Побайтовая обработка
    for (size_t i = 0; i < dataSize; ++i) {
        uint8_t byte = data[i];
        atbashTransformByte(&byte);
        output[i] = byte;
    }
    *outputSize = dataSize;
    return 0;
}

EXPORT int decrypt(const uint8_t* data, size_t dataSize,
                   const uint8_t* key, size_t keySize,
                   uint8_t* output, size_t* outputSize) {
    // Дешифрование идентично шифрованию 
    return encrypt(data, dataSize, key, keySize, output, outputSize);
}

EXPORT int generateKey(uint8_t* keyBuffer, size_t* keyBufferSize, int param) {
    if (!keyBuffer || !keyBufferSize) return -1;
    (void)param;
    
    const char* message = "Шифр Атбаш не требует ключа";
    size_t msgLen = strlen(message);
    if (*keyBufferSize < msgLen) return -3;
    
    memcpy(keyBuffer, message, msgLen);
    *keyBufferSize = msgLen;
    return 0;
}

}
