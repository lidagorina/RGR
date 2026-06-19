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

static const string ALPHABET_STR = 
    "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ"
    "абвгдеёжзийклмнопрстуфхцчшщъыьэюя"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    " !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";

static vector<string> splitUtf8(const string& str) {
    vector<string> chars;
    for (size_t i = 0; i < str.length(); ) {
        size_t len = 1;
        unsigned char c = (unsigned char)str[i];
        if (c >= 0xF0) len = 4;
        else if (c >= 0xE0) len = 3;
        else if (c >= 0xC0) len = 2;
        if (i + len > str.length()) len = str.length() - i;
        chars.push_back(str.substr(i, len));
        i += len;
    }
    return chars;
}

extern "C" {

struct AlgorithmInfo {
    const char* name;
    size_t key_size;
    size_t block_size;
    const char* key_info;
};

EXPORT const AlgorithmInfo* get_algorithm_info() {
    static AlgorithmInfo info = {
        "Цезарь",
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

EXPORT const char* encrypt_text(const char* text, unsigned char key) {
    static string result;
    result.clear();

    vector<string> alphabet = splitUtf8(ALPHABET_STR);
    vector<string> chars = splitUtf8(string(text));

    int shift = key % alphabet.size();
    if (shift == 0) shift = 3;

    for (const string& c : chars) {
        int index = -1;
        for (size_t i = 0; i < alphabet.size(); ++i) {
            if (alphabet[i] == c) {
                index = (int)i;
                break;
            }
        }

        if (index == -1) {
            result += c;
        } else {
            int newIndex = (index + shift) % (int)alphabet.size();
            result += alphabet[newIndex];
        }
    }
    return result.c_str();
}

EXPORT const char* decrypt_text(const char* text, unsigned char key) {
    static string result;
    result.clear();

    vector<string> alphabet = splitUtf8(ALPHABET_STR);
    vector<string> chars = splitUtf8(string(text));

    int shift = key % alphabet.size();
    if (shift == 0) shift = 3;

    for (const string& c : chars) {
        int index = -1;
        for (size_t i = 0; i < alphabet.size(); ++i) {
            if (alphabet[i] == c) {
                index = (int)i;
                break;
            }
        }

        if (index == -1) {
            result += c;
        } else {
            int newIndex = (index - shift + (int)alphabet.size()) % (int)alphabet.size();
            result += alphabet[newIndex];
        }
    }
    return result.c_str();
}

EXPORT unsigned char generate_key() {
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned>(time(nullptr)));
        seeded = true;
    }
    return static_cast<unsigned char>(1 + rand() % 255);
}

EXPORT void free_memory(void* ptr) {
    // ничего не делаем
}

}
