#include<iostream>
#include<random>
#include<algorithm>


using namespace std;


static void quarterRound(uint32_t state[16], int a, int b, int c, int d) 
		{
		    state[a] += state[b]; state[d] ^= state[a]; state[d]=(state[d] << 16)|(state[d] >> 16);
		    state[c] += state[d]; state[b] ^= state[c]; state[b]=(state[b] << 12)|(state[b] >> 20);
		    state[a] += state[b]; state[d] ^= state[a]; state[d]=(state[d] << 8)|(state[d] >> 24);
		    state[c] += state[d]; state[b] ^= state[c]; state[b]=(state[b] << 7)|(state[b] >> 25);
		}


void keys(vector<uint32_t> &key){
    random_device rd;
    

    for(int i = 0; i < 8; i++){
        uint32_t k = rd();
        key.push_back(static_cast<uint32_t>(k));
    }

}

string chacha20 (string mess, vector<uint32_t> &key){
    random_device rd;
    
    int counter = 0;
    string result_text = mess ;

    
    uint32_t base_state[16];
    base_state[0] = 0x61707865;
	base_state[1] = 0x3320646e;
	base_state[2] = 0x79622d32;
	base_state[3] = 0x6b206574;

    for(int i = 0; i < 8; i++){
        base_state[4+i] = key[i];
    }

    
    base_state[12] = static_cast<uint32_t>(counter);

    base_state[13] = 0;
    base_state[14] = 0;
    base_state[15] = 0;

    for(size_t j = 0; j < mess.length(); j+=64){
        uint32_t state[16];
        for(size_t i = 0; i < 16; i++){
            state[i] = base_state[i];

        }

        state[12] = static_cast<uint32_t>(counter);
        uint32_t original[16];
        for(size_t i =0; i < 16; i++){
            original[i] = state[i];
        }

        for(size_t i = 0; i < 10; i++){
            quarterRound(state, 0, 4,  8, 12);
            quarterRound(state, 1, 5,  9, 13);
            quarterRound(state, 2, 6, 10, 14);
            quarterRound(state, 3, 7, 11, 15);
            
            quarterRound(state, 0, 5, 10, 15);
            quarterRound(state, 1, 6, 11, 12);
            quarterRound(state, 2, 7,  8, 13);
            quarterRound(state, 3, 4,  9, 14);



            //  0  1  2  3 
            //  4  5  6  7
            //  8  9 10 11
            // 12 13 14 15
            
            
        }


        for(int i = 0; i < 16; i++){
            state[i] += original[i];
        }
        counter ++;
        vector<unsigned char> state_bytes;

        for(int i = 0; i < 16; i++){
            state_bytes.push_back(static_cast<unsigned char>(state[i]));
            state_bytes.push_back(static_cast<unsigned char>(state[i] >> 8));
            state_bytes.push_back(static_cast<unsigned char>(state[i] >> 16));
            state_bytes.push_back(static_cast<unsigned char>(state[i] >> 24));
        }

        size_t block_size =(64 < mess.length()-j ? 64 : mess.length()-j);
        for(size_t i = 0; i < block_size; i++){
            result_text[i+j] = mess[i+j] ^ state_bytes[i];
        }


        
    }

   
    return result_text;
    

}


void ChaCha20_fun(string original, int cipher_choice){
    
   

    string mess = original;
   
    if(cipher_choice == 1){
        vector<uint32_t> key;
        keys(key);

        cout << "Ваш ключ: ";
        for(uint32_t k: key){
            cout<< static_cast<uint32_t>(k) << " ";
        }
        cout << endl;

        string result = chacha20(mess, key);   

        for(uint32_t res: result){
            cout<< static_cast<uint32_t>(res) << " ";
        }
        cout << endl;
        
        cout << " result > " << result << endl;
        string result2 = chacha20(result, key);
        cout << " result2 > " << result2 << endl;

    }
    else if(cipher_choice == 2){
        cout << "введите ключ: " ;
        vector<uint32_t> key;

        for(int i = 0; i < 8; i++){
            uint32_t key_element;
            cin >> key_element;
            key.push_back(key_element);
        }
        
        
        string result2 = chacha20(original, key);

        cout << "Ваш результат: ";
        for(uint32_t res: result2){
            cout<< static_cast<uint32_t>(res) << " ";
        }
        cout << endl;
        cout << " result > " << result2 << endl;



    }
    else{
        cout << "такого варианта нет!" << endl;
        return;
    }
    

    
}


