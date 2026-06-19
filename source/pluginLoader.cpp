#include "../headers/plugin_api.h"
#include "../headers/utilities.h"
#include <iostream>
#include <filesystem>
#include <vector>
#include <fstream>

using namespace std;

#ifdef _WIN32
    #include <windows.h>
    #define DLOPEN(path) LoadLibraryA(path)
    #define DLSYM(handle, symbol) GetProcAddress((HMODULE)handle, symbol)
    #define DLCLOSE(handle) FreeLibrary((HMODULE)handle)
#else
    #include <dlfcn.h>
    #define DLOPEN(path) dlopen(path, RTLD_LAZY)
    #define DLSYM(handle, symbol) dlsym(handle, symbol)
    #define DLCLOSE(handle) dlclose(handle)
#endif

// читает список плагинов из конфига
vector<string> readPluginList(const string& configPath) {
    vector<string> plugins;
    ifstream file(configPath);
    
    if (!file.is_open()) {
        cerr << "не удалось открыть конфиг: " << configPath << endl;
        return plugins;
    }
    
    string line;
    while (getline(file, line)) {
        size_t start = line.find_first_not_of(" \t");
        if (start == string::npos) continue;
        
        size_t end = line.find_last_not_of(" \t");
        line = line.substr(start, end - start + 1);
        
        if (line[0] == '#') continue;
        
        plugins.push_back(line);
    }
    
    return plugins;
}

// загружает один плагин по пути
Plugin loadSinglePlugin(const string& path) {
    Plugin plugin;
    plugin.path = path;
    plugin.handle = nullptr;
    
    #ifdef _WIN32
        size_t dot = path.find_last_of(".");
        if (dot == string::npos) return plugin;
        string ext = path.substr(dot + 1);
        if (ext != "dll") return plugin;
    #else
        size_t dot = path.find_last_of(".");
        if (dot == string::npos) return plugin;
        string ext = path.substr(dot + 1);
        if (ext != "so" && ext != "dylib") return plugin;
    #endif
    
    void* handle = DLOPEN(path.c_str());
    if (!handle) {
        #ifndef _WIN32
            cerr << "не удалось загрузить: " << path << " (" << dlerror() << ")" << endl;
        #endif
        return plugin;
    }
    
    auto get_info = (GetAlgorithmInfoFunc)DLSYM(handle, "get_algorithm_info");
    auto getMinKeySize = (GetMinKeySizeFunc)DLSYM(handle, "getMinKeySize");
    auto getMaxKeySize = (GetMaxKeySizeFunc)DLSYM(handle, "getMaxKeySize");
    auto encrypt = (EncryptFunc)DLSYM(handle, "encrypt_text");
    auto decrypt = (DecryptFunc)DLSYM(handle, "decrypt_text");
    auto generate_key = (GenerateKeyFunc)DLSYM(handle, "generate_key");
    auto free_memory = (FreeMemoryFunc)DLSYM(handle, "free_memory");
    
    if (!get_info || !getMinKeySize || !getMaxKeySize || !encrypt || !decrypt || !generate_key) {
        cerr << "библиотека " << path << " не содержит всех необходимых функций" << endl;
        DLCLOSE(handle);
        return plugin;
    }
    
    const AlgorithmInfo* info = get_info();
    if (!info) {
        cerr << "не удалось получить информацию об алгоритме из " << path << endl;
        DLCLOSE(handle);
        return plugin;
    }
    
    plugin.name = info->name;
    plugin.handle = handle;
    plugin.get_info = get_info;
    plugin.getMinKeySize = getMinKeySize;
    plugin.getMaxKeySize = getMaxKeySize;
    plugin.encrypt = encrypt;
    plugin.decrypt = decrypt;
    plugin.generate_key = generate_key;
    plugin.free_memory = free_memory;
    
    cout << "загружен плагин: " << info->name << endl;
    
    return plugin;
}

// загружает плагины из папки по списку из конфига
vector<Plugin> loadPlugins(const string& folderPath) {
    vector<Plugin> plugins;
    
    vector<string> allowedPlugins = readPluginList("config/plugins.conf");
    
    if (allowedPlugins.empty()) {
        cerr << "список плагинов в config/plugins.conf пуст или файл не найден" << endl;
        return plugins;
    }
    
    if (!filesystem::exists(folderPath)) {
        cerr << "папка с плагинами не найдена: " << folderPath << endl;
        return plugins;
    }
    
    for (const string& name : allowedPlugins) {
        string path = folderPath + "/" + name;
        
        if (!filesystem::exists(path)) {
            cerr << "плагин не найден: " << path << endl;
            continue;
        }
        
        Plugin plugin = loadSinglePlugin(path);
        if (plugin.handle) {
            plugins.push_back(plugin);
        }
    }
    
    return plugins;
}

// выгружает все загруженные плагины
void unloadPlugins(vector<Plugin>& plugins) {
    for (auto& plugin : plugins) {
        if (plugin.handle) {
            DLCLOSE(plugin.handle);
            plugin.handle = nullptr;
        }
    }
    plugins.clear();
}