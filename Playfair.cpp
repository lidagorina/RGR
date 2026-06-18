#include<iostream>
#include<codecvt>
#include<string>
#include <fstream>
#include<locale>
#include <vector> 
#include <set>

using namespace std;

class Playfair{
    public: 
    wchar_t matrix[10][10];
    wstring key;

    Playfair(wstring key): key(key){ //конструктор
        
        setlocale(LC_ALL, "Russian");
        locale:: global(locale(""));
        wcout.imbue(locale(""));
        
        wstring alphabet;
        wifstream fin("alpha.txt");
        getline(fin, alphabet);

        // wcout << alphabet;
        
        set<wchar_t> used_chars;
        wstring cleankey = L"";

        for(wchar_t item: key){
            if(used_chars.find(item) == used_chars.end()){
                used_chars.insert(item);
                cleankey += item;
            }
        }
        wcout << cleankey << endl;
        this -> key = key;
        make_matrix(cleankey, alphabet);
        
    }

    void make_matrix(wstring cleankey, wstring alphabet){
       
        wstring final_res = cleankey;
        
        for(wchar_t item: alphabet){
            if(final_res.find(item) == wstring::npos){
                final_res += item;
            }
        }
        for(size_t i = 0; i < 99; i++){
            matrix[i/10][i%10] = alphabet[i];
        }


        matrix[9][9] = L' ';
    }

    
    
    


    wstring crypt(wstring text){

        wstring result = L"";

        for(size_t k = 0; k < text.length(); k += 2){
            wchar_t char1 = text[k];
            wchar_t char2 = text[k+1];


            size_t row1 = 9, col1 = 9;
            size_t row2 = 9, col2 = 9;


            for (size_t i = 0; i < 10; i++) {
                for (size_t j = 0; j < 10; j++){

                    if(matrix[i][j] == char1){
                        row1 = i;
                        col1 = j;

                    }
                    if(matrix[i][j] == char2){
                        row2 = i;
                        col2 = j;

                    }

                }
            }

            if(row1 == row2){ //если в одной строчке
                result += matrix[row1][(col1+1) % 10];
                result += matrix[row2][(col2+1) % 10];


            }
            else if(col1 == col2){ // в одной столбце
                result += matrix[(row1+1) % 10][col1];
                result += matrix[(row2+1) % 10][col2];


            }
            else {// если в прямоугольнике
                result += matrix[row1][col2];
                result += matrix[row2][col1];

            }

        }  

        return result;

    }


    wstring decrypt(wstring text){
        wstring result = L"";

        for(size_t k = 0; k < text.length(); k += 2){
            wchar_t char1 = text[k];
            wchar_t char2 = text[k+1];


            size_t row1 = 9, col1 = 9;
            size_t row2 = 9, col2 = 9;


            for (size_t i = 0; i < 10; i++) {
                for (size_t j = 0; j < 10; j++){

                    if(matrix[i][j] == char1){
                        row1 = i;
                        col1 = j;

                    }
                    if(matrix[i][j] == char2){
                        row2 = i;
                        col2 = j;

                    }

                }
            }

            if(row1 == row2){ //если в одной строчке
                result += matrix[row1][(col1+9) % 10];
                result += matrix[row2][(col2+9) % 10];


            }
            else if(col1 == col2){ // в одной столбце
                result += matrix[(row1+9) % 10][col1];
                result += matrix[(row2+9) % 10][col2];


            }
            else {// если в прямоугольнике
                result += matrix[row1][col2];
                result += matrix[row2][col1];

            }

        }  

        return result;
    }


    
};





void playfair_fun(string original, int cipher_choice){
    cin.clear();
    cin.ignore(); //решение проблемы с преносом, коорый остался в буфере

    wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    wstring w_original = converter.from_bytes(original);

    if(cipher_choice == 1){
        wcout << L"Произведем шифрование сообщения: " << endl;
        wcout << L"Введите ключ шифрования: " << endl;
        wstring key;
        getline(wcin, key);
        Playfair shifr(key);
        wstring result = shifr.crypt(w_original);

        wcout << L"Зашифр сообщение" << endl;
        wcout << result;

    }
    else if(cipher_choice == 2){
        wcout << L"Произведем расшифрование сообщения: " << endl;
        wcout << L"Введите ключ шифрования: " << endl;
        wstring key;
        getline(wcin, key);
        Playfair shifr(key);

        wstring result = shifr.decrypt(w_original);

        wcout << L"расшифр.. сообщение" << endl;
        wcout << result << endl;


    }
    else{
        wcout << L"нет такого варианта! перезапустите програм" << endl;
        return;   
    }

    
}