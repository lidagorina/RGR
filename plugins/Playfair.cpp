#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <set>

using namespace std;

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

// алфавит для матрицы 10x10 (100 символов)
static const string ALPHABET = 
    "абвгдежзийклмнопрстуфхцчшщъыьэюя"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "~!@#$%^&*()_+-=[]{}|;:'\",.<>?/";

static const int MATRIX_SIZE = 10;

// класс Playfair (адаптированный под string)
class Playfair {
private:
    char matrix[MATRIX_SIZE][MATRIX_SIZE];
    string key;

    void makeMatrix(const string& cleanKey) {
        string finalRes = cleanKey;
        
        for (char c : ALPHABET) {
            if (finalRes.find(c) == string::npos) {
                finalRes += c;
            }
        }
        
        for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
            matrix[i / MATRIX_SIZE][i % MATRIX_SIZE] = finalRes[i];
        }
    }

public:
    Playfair(const string& key) : key(key) {
        // очищаем ключ от повторяющихся символов
        set<char> usedChars;
        string cleanKey;
        
        for (char c : key) {
            if (usedChars.find(c) == usedChars.end()) {
                usedChars.insert(c);
                cleanKey += c;
            }
        }
        
        makeMatrix(cleanKey);
    }

    string encrypt(const string& text) {
        string result;
        
        for (size_t k = 0; k < text.length(); k += 2) {
            char char1 = text[k];
            char char2 = (k + 1 < text.length()) ? text[k + 1] : ' ';
            
            int row1 = -1, col1 = -1;
            int row2 = -1, col2 = -1;
            
            for (int i = 0; i < MATRIX_SIZE; i++) {
                for (int j = 0; j < MATRIX_SIZE; j++) {
                    if (matrix[i][j] == char1) {
                        row1 = i; col1 = j;
                    }
                    if (matrix[i][j] == char2) {
                        row2 = i; col2 = j;
                    }
                }
            }
            
            if (row1 == -1 || row2 == -1) {
                result += char1;
                if (char2 != ' ') result += char2;
                continue;
            }
            
            if (row1 == row2) {
                result += matrix[row1][(col1 + 1) % MATRIX_SIZE];
                result += matrix[row2][(col2 + 1) % MATRIX_SIZE];
            } else if (col1 == col2) {
                result += matrix[(row1 + 1) % MATRIX_SIZE][col1];
                result += matrix[(row2 + 1) % MATRIX_SIZE][col2];
            } else {
                result += matrix[row1][col2];
                result += matrix[row2][col1];
            }
        }
        
        return result;
    }

    string decrypt(const string& text) {
        string result;
        
        for (size_t k = 0; k < text.length(); k += 2) {
            char char1 = text[k];
            char char2 = (k + 1 < text.length()) ? text[k + 1] : ' ';
            
            int row1 = -1, col1 = -1;
            int row2 = -1, col2 = -1;
            
            for (int i = 0; i < MATRIX_SIZE; i++) {
                for (int j = 0; j < MATRIX_SIZE; j++) {
                    if (matrix[i][j] == char1) {
                        row1 = i; col1 = j;
                    }
                    if (matrix[i][j] == char2) {
                        row2 = i; col2 = j;
                    }
                }
            }
            
            if (row1 == -1 || row2 == -1) {
                result += char1;
                if (char2 != ' ') result += char2;
                continue;
            }
            
            if (row1 == row2) {
                result += matrix[row1][(col1 + MATRIX_SIZE - 1) % MATRIX_SIZE];
                result += matrix[row2][(col2 + MATRIX_SIZE - 1) % MATRIX_SIZE];
            } else if (col1 == col2) {
                result += matrix[(row1 + MATRIX_SIZE - 1) % MATRIX_SIZE][col1];
                result += matrix[(row2 + MATRIX_SIZE - 1) % MATRIX_SIZE][col2];

}
else {
                result += matrix[row1][col2];
                result += matrix[row2][col1];
            }
        }
        
        return result;
    }
};

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
        "Playfair",
        1,
        2,
        "ключ: строка (например, SECRET)"
    };
    return &info;
}

// минимальный и максимальный размер ключа
EXPORT size_t getMinKeySize() { 
    return 1;
}

EXPORT size_t getMaxKeySize() { 
    return 255;
}

// шифрование
EXPORT const char* encrypt_text(const char* text, unsigned char key) {
    static string result;
    result.clear();
    
    // преобразуем ключ в строку
    string keyStr = string(1, static_cast<char>(key));
    
    Playfair cipher(keyStr);
    string input(text);
    result = cipher.encrypt(input);
    return result.c_str();
}

// дешифрование
EXPORT const char* decrypt_text(const char* text, unsigned char key) {
    static string result;
    result.clear();
    
    string keyStr = string(1, static_cast<char>(key));
    
    Playfair cipher(keyStr);
    string input(text);
    result = cipher.decrypt(input);
    return result.c_str();
}

// генерация ключа
EXPORT unsigned char generate_key() {
    srand(static_cast<unsigned>(time(nullptr)));
    return static_cast<unsigned char>(33 + rand() % 94);
}

EXPORT void free_memory(void* ptr) {
    // ничего не делаем
}

}