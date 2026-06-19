#ifndef UTILITIES_H
#define UTILITIES_H

#include "plugin_api.h"
#include <string>
#include <vector>

std::vector<uint8_t> stringToBytes(const std::string& str);
std::string bytesToString(const std::vector<uint8_t>& data);
std::string bytesToHex(const std::vector<uint8_t>& data);
std::vector<uint8_t> hexToBytes(const std::string& hex);

std::string readLine(const std::string& prompt);
int readInt(const std::string& prompt);

bool fileExists(const std::string& path);
bool ensureDirectoryExists(const std::string& filePath);


#endif
