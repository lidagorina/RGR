#include "../headers/plugin_api.h"
#include "../headers/utilities.h"
#include "menu.h"
#include <iostream>

using namespace std;

// обработка шифрования/дешифрования текста
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
    cout << "ключ должен быть от " << minKey << " до " << maxKey << endl;
    
    int modeChoice = readInt("1 - шифрование, 2 - дешифрование: ");
    if (modeChoice != 1 && modeChoice != 2) {
        cerr << "неверный режим" << endl;
        return;
    }
    
    unsigned char key;
    string keyStr = readLine("введите ключ или Enter для генерации: ");
    
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
    
    string text = readLine("введите текст: ");
    
    const char* result;
    if (modeChoice == 1) {
        result = plugin.encrypt(text.c_str(), key);
    } else {
        result = plugin.decrypt(text.c_str(), key);
    }
    
    if (!result) {
        cerr << "ошибка выполнения операции" << endl;
        return;
    }
    
    string output(result);
    cout << "результат: " << output << endl;
    
    vector<uint8_t> bytes = stringToBytes(output);
    cout << "результат (hex): " << bytesToHex(bytes) << endl;
}

// обработка файлов
void handleFile(vector<Plugin>& plugins) {
    cout << endl << "работа с файлом" << endl;
    cout << "(функция в разработке)" << endl;
}
