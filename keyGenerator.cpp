#include "../headers/plugin_api.h"
#include "../headers/utilities.h"
#include "menu.h"
#include <iostream>

using namespace std;

// генерация ключа для выбранного алгоритма
void handleKeyGeneration(vector<Plugin>& plugins) {
    int idx = selectAlgorithm(plugins);
    if (idx < 0) return;
    
    Plugin& plugin = plugins[idx];
    
    const AlgorithmInfo* info = plugin.get_info();
    if (!info) {
        cerr << "ошибка: нет информации об алгоритме" << endl;
        return;
    }
    
    cout << endl << "выбран алгоритм: " << info->name << endl;
    cout << "размер ключа: " << info->key_size << " байт" << endl;
    
    unsigned char key = plugin.generate_key();
    cout << "сгенерирован ключ: " << (int)key << endl;
}
