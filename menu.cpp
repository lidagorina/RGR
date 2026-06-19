#include "../headers/plugin_api.h"
#include "../headers/utilities.h"
#include "cryptoWorker.h"
#include "keyGenerator.h"
#include <iostream>

using namespace std;

enum MainMenu {
    EXIT = 0,
    ENCRYPT_TEXT = 1,
    ENCRYPT_FILE = 2,
    GENERATE_KEY = 3
};

void showAlgorithmMenu(const vector<Plugin>& plugins) {
    cout << endl << "доступные алгоритмы:" << endl;
    for (size_t i = 0; i < plugins.size(); ++i) {
        cout << " " << (i + 1) << ". " << plugins[i].name << endl;
    }
    cout << " 0. выход" << endl;
}

void showMainMenu() {
    cout << endl << "криптографическое приложение" << endl;
    cout << " 1. шифрование/дешифрование текста" << endl;
    cout << " 2. шифрование/дешифрование файла" << endl;
    cout << " 3. генерация ключа" << endl;
    cout << " 0. выход" << endl;
}

int selectAlgorithm(const vector<Plugin>& plugins) {
    if (plugins.empty()) {
        cerr << "нет загруженных алгоритмов" << endl;
        return -1;
    }
    showAlgorithmMenu(plugins);
    int choice = readInt("выберите алгоритм: ");
    if (choice == 0) return -1;
    if (choice < 1 || choice > (int)plugins.size()) {
        cerr << "неверный номер" << endl;
        return -1;
    }
    return choice - 1;
}

void runMenu(vector<Plugin>& plugins) {
    while (true) {
        showMainMenu();
        int choice = readInt("выберите действие: ");
        
        MainMenu action = static_cast<MainMenu>(choice);
        
        switch (action) {
            case ENCRYPT_TEXT:
                handleText(plugins);
                break;
            case ENCRYPT_FILE:
                handleFile(plugins);
                break;
            case GENERATE_KEY:
                handleKeyGeneration(plugins);
                break;
            case EXIT:
                cout << "завершение работы" << endl;
                return;
            default:
                cerr << "неверный выбор" << endl;
        }
    }
}
