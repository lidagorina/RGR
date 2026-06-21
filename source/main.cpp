#include <iostream>
#include "../headers/plugin_api.h"
#include "../headers/utilities.h"
#include "menu.h"
#include "pluginLoader.h"

using namespace std;

int main() {
    cout << "     Криптографическая система" << endl;
    string expectedPassword = getConfigValue("password");
    if (expectedPassword.empty()) {
        cerr << "не удалось найти пароль в config/settings.cfg" << endl;
        return 1;
    }
   
    string password = readLine("Введите пароль: ");
    if (password != expectedPassword) {
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
