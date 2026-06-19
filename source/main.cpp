#include <iostream>
#include "../headers/plugin_api.h"
#include "../headers/utilities.h"
#include "menu.h"
#include "pluginLoader.h"

using namespace std;

int main() {
    vector<Plugin> plugins = loadPlugins("plugins/");
    
    if (plugins.empty()) {
        cerr << "не найдено ни одного плагина" << endl;
        return 1;
    }
    
    runMenu(plugins);
    unloadPlugins(plugins);
    
    return 0;
}
