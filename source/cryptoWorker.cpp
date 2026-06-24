#include "../headers/plugin_api.h"
#include "../headers/utilities.h"
#include "menu.h"
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

enum CryptoMode {
    ENCRYPT = 1,
    DECRYPT = 2
};

void handleText(vector<Plugin>& plugins) {
    int idx = selectAlgorithm(plugins);
    if (idx < 0) return;
    
    Plugin& plugin = plugins[idx];
    
    const AlgorithmInfo* info = plugin.get_info();
    if (!info) {
        cerr << "ошибка: нет информации об алгоритме" << endl;
        return;
    }
    
    size_t minKey = plugin.getMinKeySize();
    size_t maxKey = plugin.getMaxKeySize();
    
    cout << endl << "выбран алгоритм: " << info->name << endl;
    cout << "размер ключа: " << info->key_size << " байт" << endl;
    if (minKey != 0 || maxKey != 0) {
        cout << "ключ должен быть от " << minKey << " до " << maxKey << endl;
    } else {
        cout << "ключ не требуется" << endl;
    }
    
    int modeChoice = readInt("1 - шифрование, 2 - дешифрование: ");
    if (modeChoice != ENCRYPT && modeChoice != DECRYPT) {
        cerr << "неверный режим" << endl;
        return;
    }
    
    CryptoMode mode = static_cast<CryptoMode>(modeChoice);
    
    unsigned char key = 0;
    string keyStr;
    
    if (minKey != 0 || maxKey != 0) {
        keyStr = readLine("введите ключ или Enter для генерации: ");
        if (keyStr.empty()) {
            key = plugin.generate_key();
            cout << "сгенерирован ключ: " << (int)key << endl;
        } else {
            int val = stoi(keyStr);
            if (val < (int)minKey || val > (int)maxKey) {
                cerr << "ключ должен быть от " << minKey << " до " << maxKey << endl;
                return;
            }
            key = static_cast<unsigned char>(val);
        }
    }
    
    string text = readLine("введите текст: ");
    
    const char* result;
    if (mode == ENCRYPT) {
        result = plugin.encrypt(text.c_str(), key);
    } else {
        result = plugin.decrypt(text.c_str(), key);
    }
    
    if (!result) {
        cerr << "ошибка выполнения операции" << endl;
        return;
    }
    
    string output(result);
    if (plugin.name == "XOR" || plugin.name == "ChaCha20") {
    cout << "результат (hex): " << output << endl;
} else {
    cout << "результат: " << output << endl;
    vector<uint8_t> bytes = stringToBytes(output);
    cout << "результат (hex): " << bytesToHex(bytes) << endl;
}
}

void handleFile(vector<Plugin>& plugins) {
    int idx = selectAlgorithm(plugins);
    if (idx < 0) return;
    
    Plugin& plugin = plugins[idx];
    
    const AlgorithmInfo* info = plugin.get_info();
    if (!info) {
        cerr << "ошибка: нет информации об алгоритме" << endl;
        return;
    }
    
    size_t minKey = plugin.getMinKeySize();
    size_t maxKey = plugin.getMaxKeySize();
    
    cout << endl << "выбран алгоритм: " << info->name << endl;
    cout << "размер ключа: " << info->key_size << " байт" << endl;
    if (minKey != 0 || maxKey != 0) {
        cout << "ключ должен быть от " << minKey << " до " << maxKey << endl;
    } else {
        cout << "ключ не требуется" << endl;
    }
    
    int modeChoice = readInt("1 - шифрование, 2 - дешифрование: ");
    if (modeChoice != ENCRYPT && modeChoice != DECRYPT) {
        cerr << "неверный режим" << endl;
        return;
    }
    
    CryptoMode mode = static_cast<CryptoMode>(modeChoice);
    
    unsigned char key = 0;
    string keyStr;
    
    if (minKey != 0 || maxKey != 0) {
        keyStr = readLine("введите ключ или Enter для генерации: ");
        if (keyStr.empty()) {
            key = plugin.generate_key();
            cout << "сгенерирован ключ: " << (int)key << endl;
        } else {
            int val = stoi(keyStr);
            if (val < (int)minKey || val > (int)maxKey) {
                cerr << "ключ должен быть от " << minKey << " до " << maxKey << endl;
                return;
            }
            key = static_cast<unsigned char>(val);
        }
    }
    
    string inputPath = readLine("введите путь к входному файлу: ");
    if (!fileExists(inputPath)) {
        cerr << "файл не найден: " << inputPath << endl;
        return;
    }
    
    string outputPath = readLine("введите путь к выходному файлу: ");
    if (!ensureDirectoryExists(outputPath)) {
        cerr << "не удалось создать папку для выходного файла" << endl;
        return;
    }
    
    ifstream inFile(inputPath, ios::binary);
    if (!inFile.is_open()) {
        cerr << "не удалось открыть файл для чтения" << endl;
        return;
    }
    
    vector<unsigned char> data((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();
    
    if (data.empty()) {
        cerr << "файл пуст" << endl;
        return;
    }
    
    cout << "размер файла: " << data.size() << " байт" << endl;
    
    string inputData(data.begin(), data.end());
    string result;
    
    if (mode == ENCRYPT) {
        result = plugin.encrypt(inputData.c_str(), key);
    } else {
        result = plugin.decrypt(inputData.c_str(), key);
    }
    
    ofstream outFile(outputPath, ios::binary);
    if (!outFile.is_open()) {
        cerr << "не удалось создать выходной файл" << endl;
        return;
    }
    
    outFile.write(result.c_str(), result.size());
    outFile.close();
    
    cout << "результат сохранён в: " << outputPath << endl;
}

void processFileDirect(const Plugin& plugin, bool encrypt, const string& inPath, const string& outPath, unsigned char key) {
    ifstream inFile(inPath, ios::binary);
    if (!inFile) {
        cerr << "файл не найден: " << inPath << endl;
        return;
    }

    vector<char> buffer((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();

    string content(buffer.begin(), buffer.end());
    const char* resultData;
    if (encrypt) {
        resultData = plugin.encrypt(content.c_str(), key);
    } else {
        resultData = plugin.decrypt(content.c_str(), key);
    }

    string out = outPath.empty() ? inPath + (encrypt ? ".enc" : ".dec") : outPath;

    ofstream outFile(out, ios::binary);
    outFile.write(resultData, strlen(resultData));
    outFile.close();
    cout << "результат сохранён в: " << out << endl;
}
