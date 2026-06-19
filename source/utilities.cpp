#include "../headers/utilities.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>

using namespace std;

#ifdef _WIN32
    #include <direct.h>
    #define MKDIR(dir) _mkdir(dir)
#else
    #include <sys/stat.h>
    #define MKDIR(dir) mkdir(dir, 0755)
#endif

// строка в вектор байт
vector<uint8_t> stringToBytes(const string& str) {
    return vector<uint8_t>(str.begin(), str.end());
}

// вектор байт в строку
string bytesToString(const vector<uint8_t>& data) {
    return string(data.begin(), data.end());
}

// байты в hex строку
string bytesToHex(const vector<uint8_t>& data) {
    ostringstream oss;
    for (uint8_t byte : data) {
        oss << hex << setw(2) << setfill('0') 
            << static_cast<int>(byte);
    }
    return oss.str();
}

// hex строку в байты
vector<uint8_t> hexToBytes(const string& hex) {
    vector<uint8_t> bytes;
    if (hex.length() % 2 != 0) return bytes;
    
    for (size_t i = 0; i < hex.length(); i += 2) {
        string byteStr = hex.substr(i, 2);
        try {
            uint8_t byte = static_cast<uint8_t>(stoi(byteStr, nullptr, 16));
            bytes.push_back(byte);
        } catch (...) {
            return {};
        }
    }
    return bytes;
}

// чтение строки с консоли
string readLine(const string& prompt) {
    cout << prompt;
    string line;
    getline(cin, line);
    return line;
}

// чтение числа с проверкой
int readInt(const string& prompt) {
    cout << prompt;
    int value;
    while (!(cin >> value)) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "ошибка: введите число: ";
    }
    cin.ignore(10000, '\n');
    return value;
}

// проверка существования файла
bool fileExists(const string& path) {
    ifstream file(path);
    return file.good();
}

// проверка и создание папки для файла
bool ensureDirectoryExists(const string& filePath) {
    size_t lastSlash = filePath.find_last_of("/\\");
    if (lastSlash == string::npos) return true;
    
    string dirPath = filePath.substr(0, lastSlash);
    if (dirPath.empty()) return true;
    
    struct stat info;
    if (stat(dirPath.c_str(), &info) == 0) return true;
    
    cout << "директория \"" << dirPath << "\" не существует. создать? (1 - да, 0 - нет): ";
    int choice = readInt("");
    if (choice == 1) {
        if (MKDIR(dirPath.c_str()) == 0) {
            cout << "директория создана.\n";
            return true;
        }
        cerr << "не удалось создать директорию.\n";
        return false;
    }
    return false;
}

// ConstBuffer в вектор байт
vector<uint8_t> bufferToVector(ConstBuffer buf) {
    return vector<uint8_t>(buf.data, buf.data + buf.size);
}
