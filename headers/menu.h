#ifndef MENU_H
#define MENU_H

#include "../headers/plugin_api.h"
#include <vector>

using namespace std;

void runMenu(vector<Plugin>& plugins);
int selectAlgorithm(const vector<Plugin>& plugins);

#endif
