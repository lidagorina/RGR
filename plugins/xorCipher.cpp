#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

static string to_hex(const string& s) {
    ostringstream oss;
    for (unsigned char c : s) {
        oss << hex << setw(2) << setfill('0') << (int)c;
    }
    return oss.str();
}

static string from_hex(const string& s) {
    string res;
    for (size_t i = 0; i < s.length(); i += 2) {
        if (i + 2 > s.length()) break;
        string byteString = s.substr(i, 2);
        char byte = (char)strtol(byteString.c_str(), nullptr, 16);
        res.push_back(byte);
    }
    return res;
}

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

static vector<string> getFullAlphabet() {
    static vector<string> alphabet;
    if (!alphabet.empty()) return alphabet;

    string base = 
        "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ"
        "абвгдеёжзийклмнопрстуфхцчшщъыьэюя"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789"
        " !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";

    alphabet = splitUtf8(base);
    
    string extra = "αβγδεζηθικλμνξοπρστυφχψωΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡΣΤΥΦΧΨΩ±≤≥≠≈∞∫√∆π∑∏μ∂¬∧∨∩∪⊂⊃⊄⊆⊇⊕⊗⊥∠°†‡¶§©®™€£¥¢¤₾░▒▓│┤╡╢";
    
    vector<string> extraChars = splitUtf8(extra);
    for (const string& s : extraChars) {
        if (alphabet.size() < 256) alphabet.push_back(s);
    }
    
    while (alphabet.size() < 256) {
        alphabet.push_back("?");
    }
    if (alphabet.size() > 256) alphabet.resize(256);

    return alphabet;
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
        "XOR",
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
    if (!text) return "";

    vector<string> alphabet = getFullAlphabet();
    vector<string> chars = splitUtf8(string(text));
    unsigned char k = key % 256;

    for (const string& c : chars) {
        int idx = -1;
        for (int i = 0; i < 256; ++i) {
            if (alphabet[i] == c) {
                idx = i;
                break;
            }
        }

        if (idx == -1) {
            result += c;
        } else {
            int newIdx = idx ^ k;
            result += alphabet[newIdx];
        }
    }
    
    result = to_hex(result);
    return result.c_str();
}

EXPORT const char* decrypt_text(const char* text, unsigned char key) {
    static string result;
    result.clear();
    if (!text) return "";
    
    string raw_data = from_hex(string(text));
    
    vector<string> alphabet = getFullAlphabet();
    vector<string> chars = splitUtf8(raw_data);
    unsigned char k = key % 256;

    for (const string& c : chars) {
        int idx = -1;
        for (int i = 0; i < 256; ++i) {
            if (alphabet[i] == c) {
                idx = i;
                break;
            }
        }

        if (idx == -1) {
            result += c;
        } else {
            int newIdx = idx ^ k;
            result += alphabet[newIdx];
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
