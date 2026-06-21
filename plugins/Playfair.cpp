#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>

using namespace std;

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

static const int MATRIX_SIZE = 16;
static const int MATRIX_TOTAL = 256;

static vector<string> splitUtf8(const string& str) {
    vector<string> chars;
    if (str.empty()) return chars;
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

static vector<string> getBaseAlphabet() {
    string base = 
        "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ"
        "абвгдеёжзийклмнопрстуфхцчшщъыьэюя"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789"
        " !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"
        "\n\r\t"
        "αβγδεζηθικλμνξοπρστυφχψωΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡΣΤΥΦΧΨΩ"
        "±≤≥≠≈∞∫√∆π∑∏μ∂¬∧∨∩∪⊂⊃⊄⊆⊇⊕⊗⊥∠°†‡¶§©®™€£¥¢¤₾"
        "░▒▓│┤╡╢╖╕╣║╗╝╜╛┐└┴┬├─┼╞╟╚╔╩╦╠═╬╧"
        "ØøÆæßÞþÐð";
    vector<string> alph = splitUtf8(base);
    while (alph.size() < 256) alph.push_back("~");
    if (alph.size() > 256) alph.resize(256);
    return alph;
}

static vector<string> buildMatrix(unsigned char key_byte) {
    vector<string> alphabet = getBaseAlphabet();
    vector<string> matrix;
    matrix.reserve(256);

    string keyChar = alphabet[key_byte % alphabet.size()];
    matrix.push_back(keyChar);
    
    for (const string& s : alphabet) {
        bool exists = false;
        for (const string& m : matrix) {
            if (m == s) { exists = true; break; }
        }
        if (!exists) matrix.push_back(s);
    }
    while (matrix.size() < 256) matrix.push_back("~");
    if (matrix.size() > 256) matrix.resize(256);
    return matrix;
}

static string processPair(const string& a, const string& b, const vector<string>& matrix, bool encrypt) {
    int idx1 = -1, idx2 = -1;
    int m_size = (int)matrix.size();

    for (int i = 0; i < m_size; ++i) {
        if (matrix[i] == a) idx1 = i;
        if (matrix[i] == b) idx2 = i;
    }
    
    if (idx1 == -1 || idx2 == -1) return a + b;
    
    int r1 = idx1 / 16, c1 = idx1 % 16;
    int r2 = idx2 / 16, c2 = idx2 % 16;
    int shift = encrypt ? 1 : 15;

    auto get_m = [&](int r, int c) {
        int final_idx = (r * 16) + c;
        if (final_idx >= 0 && final_idx < m_size) return matrix[final_idx];
        return string("?");
    };

    if (r1 == r2) {
        return get_m(r1, (c1 + shift) % 16) + get_m(r2, (c2 + shift) % 16);
    } else if (c1 == c2) {
        return get_m((r1 + shift) % 16, c1) + get_m((r2 + shift) % 16, c2);
    } else {
        return get_m(r1, c2) + get_m(r2, c1);
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

EXPORT size_t getMinKeySize() { return 1; }
EXPORT size_t getMaxKeySize() { return 255; }

EXPORT const char* encrypt_text(const char* text, unsigned char key) {
    static string result;
    result.clear();
    if (!text || strlen(text) == 0) return "";

    vector<string> matrix = buildMatrix(key);
    vector<string> chars = splitUtf8(string(text));
    
    vector<string> prepared;
    for (size_t i = 0; i < chars.size(); ++i) {
        prepared.push_back(chars[i]);
        if (prepared.size() % 2 != 0) {
            if (i + 1 < chars.size() && chars[i] == chars[i + 1]) {
                prepared.push_back("Ø");
            }
        }
    }
    if (prepared.size() % 2 != 0) prepared.push_back("Ø");
    
    for (size_t i = 0; i + 1 < prepared.size(); i += 2) {
        result += processPair(prepared[i], prepared[i + 1], matrix, true);
    }
    return result.c_str();
}

EXPORT const char* decrypt_text(const char* text, unsigned char key) {
    static string result;
    result.clear();
    if (!text || strlen(text) == 0) return "";

    vector<string> matrix = buildMatrix(key);
    vector<string> chars = splitUtf8(string(text));
    
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
