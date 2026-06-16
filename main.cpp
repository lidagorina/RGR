#include<iostream>
#include<fstream>
//подключить свои h файлы
using namespace std;

enum Choice{
    Playfair = 1,
    ChaCha20 = 2,

    Caesar = 3,
    FeatShamir = 4,

    Atbash = 5,
    Rot13 = 6

};

int main(){
    int shifr_choice;
    cout << "выберите шифр " << endl;
    cout << "1 - Шифр Плейфера" << endl;
    cout << "2 - ChaCha20" << endl;
    cout << "3 - Шифр Цезаря" << endl;
    cout << "4 - Фиат-Шамир" << endl;
    cout << "5 - Атбаш" << endl;
    cout << "6 - ROT13" << endl;
    cout << "Ваш выбор: ";
    cin >> shifr_choice;
    cout << endl;

    Choice user_choice = static_cast<Choice>(shifr_choice);

    cout << endl;
    cout << "Зашифровать-1 расшифровать-2" << endl;
    int cipher_choice = -1;
    cin >> cipher_choice;

    cout << "1 - терминал 2 - файл" << endl;
    int input_choice;
    cin >> input_choice;
    string original;

    if(input_choice == 1){
        cout << "текст " << endl;
        string line;
        while(getline(cin, line)){
        original += line + "\n";

        }

        

    }

    else{
        cout << endl << "ввод происходеит из файла tekst.txt " << endl << endl;
        ifstream fin("tekst.txt");
        string line;
        

        while(getline(fin, line)){
        original += line + "\n";
 
        }
    
    }


    switch (user_choice)
    {
    case Playfair:
        cout << endl << "выбран шифр Плейфер" << endl;
        //запуск функции вида F(string original, int cipher_choice)
        // cipher_choice 1 или 2
        
        break;

    case ChaCha20:
        cout << "выбран шифр ChaCha20" << endl;
        
        break;
    


    case Caesar:
    //
        break;

    case FeatShamir:
        //
        break;


    case Atbash:
        //
        break;
    case Rot13:
        //
        break;

    
    default:
        cout << "такого варианта нет! перезапустите программу" << endl;
        break;
    }


    return 0;
}