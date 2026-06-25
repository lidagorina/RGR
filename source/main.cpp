#include <iostream>
#include <cstring>
#include <clocale>
#include "../headers/plugin_api.h"
#include "../headers/utilities.h"
#include "menu.h"
#include "pluginLoader.h"
#include "cryptoWorker.h"

using namespace std;

void print_help() {
    cout << endl << "Криптографическая система" << endl;
    cout << "Использование: ./cryptum [опции]" << endl;
    cout << endl;
    cout << "Опции:" << endl;
    cout << "  --help, -h            показать справку" << endl;
    cout << "  --list, -l            список алгоритмов" << endl;
    cout << "  --algorithm <имя>     выбрать алгоритм" << endl;
    cout << "  --mode <enc|dec>      режим: encrypt или decrypt" << endl;
    cout << "  --key <число>         ключ (1-255)" << endl;
    cout << "  --input <файл>        входной файл" << endl;
    cout << "  --output <файл>       выходной файл" << endl;
    cout << "  --generate-key        сгенерировать ключ" << endl;
    cout << endl;
    cout << "Примеры:" << endl;
    cout << "  ./cryptum --help" << endl;
    cout << "  ./cryptum --list" << endl;
    cout << "  ./cryptum --algorithm caesar --generate-key" << endl;
    cout << "  ./cryptum --algorithm caesar --mode encrypt --key 3 --input war.txt --output war.enc" << endl;
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            print_help();
            return 0;
        }

        vector<Plugin> plugins = loadPlugins("plugins/");
        if (plugins.empty()) {
            cerr << "не найдено ни одного плагина" << endl;
            return 1;
        }

        if (strcmp(argv[1], "--list") == 0 || strcmp(argv[1], "-l") == 0) {
            cout << "Доступные алгоритмы:" << endl;
            for (const auto& p : plugins) {
                cout << "  - " << p.name << endl;
            }
            return 0;
        }

        string algo_name, mode, key_val, input_path, output_path;
        bool generate_key = false;

        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "--algorithm") == 0 && i + 1 < argc) {
                algo_name = argv[++i];
            } else if (strcmp(argv[i], "--mode") == 0 && i + 1 < argc) {
                mode = argv[++i];
            } else if (strcmp(argv[i], "--key") == 0 && i + 1 < argc) {
                key_val = argv[++i];
            } else if (strcmp(argv[i], "--input") == 0 && i + 1 < argc) {
                input_path = argv[++i];
            } else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc) {
                output_path = argv[++i];
            } else if (strcmp(argv[i], "--generate-key") == 0) {
                generate_key = true;
            }
        }

        Plugin* selected = nullptr;
        for (auto& p : plugins) {
            if (p.name == algo_name) {
                selected = &p;
                break;
            }
        }

        if (!selected) {
            cerr << "алгоритм не найден: " << algo_name << endl;
            return 1;
        }

        if (generate_key) {
            unsigned char k = selected->generate_key();
            cout << "сгенерирован ключ: " << (int)k << endl;
            unloadPlugins(plugins);
            return 0;
        }

        if (!input_path.empty() && !key_val.empty()) {
            unsigned char k = (unsigned char)stoi(key_val);
            bool is_encrypt = (mode == "encrypt" || mode == "enc");
            processFileDirect(*selected, is_encrypt, input_path, output_path, k);
            unloadPlugins(plugins);
            return 0;
        }

        cerr << "недостаточно аргументов. Используйте --help" << endl;
        unloadPlugins(plugins);
        return 1;
    }

    cout << endl << "Криптографическая система" << endl;

    string expected_password = getConfigValue("password");
    if (expected_password.empty()) {
        cerr << "не удалось найти пароль в config/settings.cfg" << endl;
        return 1;
    }

    string password = readLine("Введите пароль: ");
    if (password != expected_password) {
        cerr << "Неверный пароль! Доступ запрещён." << endl;
        return 1;
    }

    cout << "Доступ разрешён." << endl;

    vector<Plugin> plugins = loadPlugins("plugins/");
    if (plugins.empty()) {
        cerr << "не найдено ни одного плагина" << endl;
        return 1;
    }

    runMenu(plugins);
    unloadPlugins(plugins);

    return 0;
}
