#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <random>
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
        string byteString = s.substr(i, 2);
        char byte = (char)strtol(byteString.c_str(), nullptr, 16);
        res.push_back(byte);
    }
    return res;
}

static void quarterRound(uint32_t state[16], int a, int b, int c, int d) {
    state[a] += state[b];
    state[d] ^= state[a];
    state[d] = (state[d] << 16) | (state[d] >> 16);
    
    state[c] += state[d];
    state[b] ^= state[c];
    state[b] = (state[b] << 12) | (state[b] >> 20);
    
    state[a] += state[b];
    state[d] ^= state[a];
    state[d] = (state[d] << 8) | (state[d] >> 24);
    
    state[c] += state[d];
    state[b] ^= state[c];
    state[b] = (state[b] << 7) | (state[b] >> 25);
}

static string chacha20Crypt(const string& mess, const vector<uint32_t>& key) {
    uint32_t base_state[16];
    base_state[0] = 0x61707865;
    base_state[1] = 0x3320646e;
    base_state[2] = 0x79622d32;
    base_state[3] = 0x6b206574;
    
    for (int i = 0; i < 8; i++) {
        base_state[4 + i] = key[i];
    }
    
    base_state[12] = 0;
    base_state[13] = 0;
    base_state[14] = 0xdeadbeef;
    base_state[15] = 0xbeefdead;

    string result_text = mess;
    
    for (size_t j = 0; j < mess.length(); j += 64) {
        uint32_t state[16];
        memcpy(state, base_state, sizeof(base_state));
        state[12] = (uint32_t)(j / 64);

        uint32_t original[16];
        memcpy(original, state, sizeof(state));

        for (int i = 0; i < 10; i++) {
            quarterRound(state, 0, 4, 8, 12);
            quarterRound(state, 1, 5, 9, 13);
            quarterRound(state, 2, 6, 10, 14);
            quarterRound(state, 3, 7, 11, 15);
            
            quarterRound(state, 0, 5, 10, 15);
            quarterRound(state, 1, 6, 11, 12);
            quarterRound(state, 2, 7, 8, 13);
            quarterRound(state, 3, 4, 9, 14);
        }

        for (int i = 0; i < 16; i++) {
            state[i] += original[i];
        }

        unsigned char keystream[64];
        for (int i = 0; i < 16; i++) {
            keystream[i * 4 + 0] = (state[i] >> 0) & 0xff;
            keystream[i * 4 + 1] = (state[i] >> 8) & 0xff;
            keystream[i * 4 + 2] = (state[i] >> 16) & 0xff;
            keystream[i * 4 + 3] = (state[i] >> 24) & 0xff;
        }

        size_t block_len = (64 < mess.length() - j) ? 64 : mess.length() - j;
        for (size_t i = 0; i < block_len; i++) {
            result_text[j + i] = mess[j + i] ^ keystream[i];
        }
    }
    return result_text;
}

static vector<uint32_t> g_globalKey(8, 0x12345678);

extern "C" {

struct AlgorithmInfo {
    const char* name;
    size_t key_size;
    size_t block_size;
    const char* key_info;
};

EXPORT const AlgorithmInfo* get_algorithm_info() {
    static AlgorithmInfo info = {
        "ChaCha20",
        32,
        64,
        "32-байтовый ключ (Hex-вывод)"
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
    if (!text) return "";
    (void)key;

    string encrypted_binary = chacha20Crypt(string(text), g_globalKey);
    result = to_hex(encrypted_binary);
    return result.c_str();
}

EXPORT const char* decrypt_text(const char* text, unsigned char key) {
    static string result;
    result.clear();
    if (!text) return "";
    (void)key;

    string binary = from_hex(string(text));
    result = chacha20Crypt(binary, g_globalKey);
    return result.c_str();
}

EXPORT unsigned char generate_key() {
    random_device rd;
    for (int i = 0; i < 8; i++) {
        g_globalKey[i] = rd();
    }
    return (unsigned char)(g_globalKey[0] & 0xFF);
}

EXPORT void free_memory(void* ptr) {
    // ничего не делаем
}

}
