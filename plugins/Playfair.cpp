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

static const int M_SIZE = 13;
static const int M_TOTAL = 169;

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
    "0123456789"
    " !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";

static vector<string> buildMatrix(unsigned char key_byte) {
    vector<string> alphabet = splitUtf8(ALPHABET_STR);
    vector<string> matrix;
    
    string keyChar = alphabet[key_byte % alphabet.size()];
    matrix.push_back(keyChar);
    
    for (const string& c : alphabet) {
        bool exists = false;
        for (const string& m : matrix) {
            if (m == c) {
                exists = true;
                break;
            }
        }
        if (!exists) matrix.push_back(c);
    }
    
    if (matrix.size() > M_TOTAL) matrix.resize(M_TOTAL);
    while (matrix.size() < M_TOTAL) matrix.push_back(" ");
    
    return matrix;
}

static string processPair(const string& a, const string& b, const vector<string>& matrix, bool encrypt) {
    int idx1 = -1, idx2 = -1;
    for (int i = 0; i < M_TOTAL; ++i) {
        if (matrix[i] == a) idx1 = i;
        if (matrix[i] == b) idx2 = i;
    }
    
    if (idx1 == -1 || idx2 == -1) return a + b;
    
    int r1 = idx1 / M_SIZE, c1 = idx1 % M_SIZE;
    int r2 = idx2 / M_SIZE, c2 = idx2 % M_SIZE;
    int shift = encrypt ? 1 : (M_SIZE - 1);

    if (r1 == r2) {
        return matrix[r1 * M_SIZE + (c1 + shift) % M_SIZE] + matrix[r2 * M_SIZE + (c2 + shift) % M_SIZE];
    } else if (c1 == c2) {
        return matrix[((r1 + shift) % M_SIZE) * M_SIZE + c1] + matrix[((r2 + shift) % M_SIZE) * M_SIZE + c2];
    } else {
        return matrix[r1 * M_SIZE + c2] + matrix[r2 * M_SIZE + c1];
    }
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
        "Playfair",
        1,
        2,
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
    if (!text) return "";
    
    vector<string> matrix = buildMatrix(key);
    vector<string> chars = splitUtf8(text);
    
    if (chars.size() % 2 != 0) chars.push_back(" ");
    
    for (size_t i = 0; i < chars.size(); i += 2) {
        result += processPair(chars[i], chars[i + 1], matrix, true);
    }
    return result.c_str();
}

EXPORT const char* decrypt_text(const char* text, unsigned char key) {
    static string result;
    result.clear();
    if (!text) return "";
    
    vector<string> matrix = buildMatrix(key);
    vector<string> chars = splitUtf8(text);
    
    for (size_t i = 0; i + 1 < chars.size(); i += 2) {
        result += processPair(chars[i], chars[i + 1], matrix, false);
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
