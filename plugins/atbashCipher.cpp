#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <ctime>

using namespace std;

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

extern "C" {

EXPORT const char* getAlgorithmName() {
    return "Atbash (байтовый)";
}

EXPORT const char* getKeyInfo() {
    return "Зеркальное отображение байтов (0↔255, 1↔254, ...)\n"
           "Инволютивный шифр (шифрование = дешифрование).\n"
           "Не требует ключа.";
}

EXPORT size_t getMinKeySize() {
    return 0;
}

EXPORT size_t getMaxKeySize() {
    return 0;
}

EXPORT int encrypt(const uint8_t* data, size_t dataSize,
                   const uint8_t* key, size_t keySize,
                   uint8_t* output, size_t* outputSize) {
    if (!data || !output || !outputSize) return -1;
    (void)key; (void)keySize;

    if (*outputSize < dataSize) return -3;

    for (size_t i = 0; i < dataSize; ++i) {
        output[i] = 255 - data[i];
    }
    *outputSize = dataSize;
    return 0;
}

EXPORT int decrypt(const uint8_t* data, size_t dataSize,
                   const uint8_t* key, size_t keySize,
                   uint8_t* output, size_t* outputSize) {
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
