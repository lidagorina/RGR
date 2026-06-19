#ifndef CRYPTO_WORKER_H
#define CRYPTO_WORKER_H

#include "../headers/plugin_api.h"
#include <vector>

using namespace std;

void handleText(vector<Plugin>& plugins);
void handleFile(vector<Plugin>& plugins);

#endif