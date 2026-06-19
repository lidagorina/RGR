#include <cstdlib>
#include <cstring>
#include <string>
#include <ctime>
#include <vector>

using namespace std;

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

static string g_result;

static vector<string> splitUtf8(const string& str) {
    vector<string> chars;
    for (size_t i = 0; i < str.length(); ) {
        size_t len = 1;
        unsigned char c = (unsigned char)str[i];
        if (c >= 0x80) {
            if ((c & 0xE0) == 0xC0) len = 2;
            else if ((c & 0xF0) == 0xE0) len = 3;
            else if ((c & 0xF8) == 0xF0) len = 4;
        }
        if (i + len > str.length()) len = str.length() - i;
        chars.push_back(str.substr(i, len));
        i += len;
    }
    return chars;
}

static const string ALPHABET_STR = 
    "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ"
    "абвгдеёжзийклмнопрстуфхцчшщъыьэюя"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789";

extern "C" {

struct AlgorithmInfo {
    const char* name;
    size_t key_size;
    size_t block_size;
    const char* key_info;
};

EXPORT const AlgorithmInfo* get_algorithm_info() {
    static AlgorithmInfo info = {
        "Atbash",
        0,
        0,
        "ключ не требуется"
    };
    return &info;
}

EXPORT size_t getMinKeySize() { 
    return 0;
}

EXPORT size_t getMaxKeySize() { 
    return 0;
}

EXPORT const char* encrypt_text(const char* text, unsigned char key) {
    static string result;
    result.clear();
    (void)key;

    vector<string> alphabet = splitUtf8(ALPHABET_STR);
    vector<string> chars = splitUtf8(string(text));
    int size = (int)alphabet.size();

    for (const string& c : chars) {
        int idx = -1;
        for (int i = 0; i < size; ++i) {
            if (alphabet[i] == c) {
                idx = i;
                break;
            }
        }

        if (idx == -1) {
            result += c;
        } else {
            int revIdx = size - 1 - idx;
            result += alphabet[revIdx];
        }
    }
    return result.c_str();
}

EXPORT const char* decrypt_text(const char* text, unsigned char key) {
    return encrypt_text(text, key);
}

EXPORT unsigned char generate_key() {
    return 0;
}

EXPORT void free_memory(void* ptr) {
    // ничего не делаем
}

}
