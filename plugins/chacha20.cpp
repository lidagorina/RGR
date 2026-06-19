#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <random>
#include <sstream>

using namespace std;

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

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

static vector<uint32_t> generateChaCha20Key() {
    vector<uint32_t> key;
    random_device rd;
    
    for (int i = 0; i < 8; i++) {
        key.push_back(rd());
    }
    return key;
}

static string keyToString(const vector<uint32_t>& key) {
    stringstream ss;
    for (uint32_t k : key) {
        ss << k << " ";
    }
    return ss.str();
}

static string chacha20Crypt(const string& mess, const vector<uint32_t>& key) {
    int counter = 0;
    string result_text = mess;
    
    uint32_t base_state[16];
    base_state[0] = 0x61707865;
    base_state[1] = 0x3320646e;
    base_state[2] = 0x79622d32;
    base_state[3] = 0x6b206574;
    
    for (int i = 0; i < 8; i++) {
        base_state[4 + i] = key[i];
    }
    
    base_state[12] = static_cast<uint32_t>(counter);
    base_state[13] = 0;
    base_state[14] = 0;
    base_state[15] = 0;
    
    for (size_t j = 0; j < mess.length(); j += 64) {
        uint32_t state[16];
        for (size_t i = 0; i < 16; i++) {
            state[i] = base_state[i];
        }
        
        state[12] = static_cast<uint32_t>(counter);
        
        uint32_t original[16];
        for (size_t i = 0; i < 16; i++) {
            original[i] = state[i];
        }
        
        for (size_t i = 0; i < 10; i++) {
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
        counter++;
        
        vector<unsigned char> state_bytes;
        for (int i = 0; i < 16; i++) {
            state_bytes.push_back(static_cast<unsigned char>(state[i]));
            state_bytes.push_back(static_cast<unsigned char>(state[i] >> 8));
            state_bytes.push_back(static_cast<unsigned char>(state[i] >> 16));
            state_bytes.push_back(static_cast<unsigned char>(state[i] >> 24));
        }
        
        size_t block_size = (64 < mess.length() - j ? 64 : mess.length() - j);
        for (size_t i = 0; i < block_size; i++) {
            result_text[i + j] = mess[i + j] ^ state_bytes[i];
        }
    }
    
    return result_text;
}

static vector<uint32_t> g_globalKey;

extern "C" {

struct AlgorithmInfo {
    const char* name;
    size_t key_size;
    size_t block_size;
    const char* key_info;
};

// информация об алгоритме
EXPORT const AlgorithmInfo* get_algorithm_info() {
    static AlgorithmInfo info = {
        "ChaCha20",
        32,
        64,
        "32-байтовый ключ (генерируется автоматически)"
    };
    return &info;
}

// минимальный и максимальный размер ключа
EXPORT size_t getMinKeySize() { 
    return 0;
}

EXPORT size_t getMaxKeySize() { 
    return 0;
}

// шифрование
EXPORT const char* encrypt_text(const char* text, unsigned char key) {
    static string result;
    result.clear();
    (void)key;
    
    string input(text);
    result = chacha20Crypt(input, g_globalKey);
    return result.c_str();
}

// дешифрование (ChaCha20 симметричен)
EXPORT const char*
decrypt_text(const char* text, unsigned char key) {
    return encrypt_text(text, key);
}

// генерация ключа
EXPORT unsigned char generate_key() {
    g_globalKey = generateChaCha20Key();
    return static_cast<unsigned char>(g_globalKey[0]);
}

// получить ключ в виде строки
EXPORT const char* get_key_string() {
    static string keyStr;
    keyStr = keyToString(g_globalKey);
    return keyStr.c_str();
}

EXPORT void free_memory(void* ptr) {
    // ничего не делаем
}

}