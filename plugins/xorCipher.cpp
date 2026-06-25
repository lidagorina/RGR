#include <cstdlib>
#include <cstring>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

static string g_result;

static string to_hex(const string& s) {
    ostringstream oss;
    for (unsigned char c : s) {
        oss << hex << setw(2) << setfill('0') << (int)c;
    }
    return oss.str();
}

static string from_hex(const string& s) {
    string res;
    for (size_t i = 0; i + 1 < s.length(); i += 2) {
        string byteStr = s.substr(i, 2);
        char byte = (char)strtol(byteStr.c_str(), nullptr, 16);
        res.push_back(byte);
    }
    return res;
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
    string input(text);
    g_result = input;
    for (size_t i = 0; i < g_result.size(); ++i) {
        g_result[i] = static_cast<char>(static_cast<unsigned char>(g_result[i]) ^ key);
    }
    g_result = to_hex(g_result);
    return g_result.c_str();
}

EXPORT const char* decrypt_text(const char* text, unsigned char key) {
    string input = from_hex(string(text));
    g_result = input;
    for (size_t i = 0; i < g_result.size(); ++i) {
        g_result[i] = static_cast<char>(static_cast<unsigned char>(g_result[i]) ^ key);
    }
    return g_result.c_str();
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
