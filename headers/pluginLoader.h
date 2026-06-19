#ifndef PLUGIN_LOADER_H
#define PLUGIN_LOADER_H

#include "plugin_api.h"
#include <vector>
#include <string>

using namespace std;

vector<Plugin> loadPlugins(const string& folderPath);
void unloadPlugins(vector<Plugin>& plugins);

#endif

