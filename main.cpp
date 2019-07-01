
#include "E2.h"


//функция преобразующая массив char в массив uint8_t
void from_char_to_uint8_t(char* text, uint8_t* res, int size){
    for(int i = 0; i<size; i++){
        res[i] = (uint8_t)text[i];
    }
}

int main() {
    //открытый текст
    char* text = (char *)("Hello how are you todayrteyj76");
    const int  open_text_len = 29;
    uint8_t open[open_text_len];
    from_char_to_uint8_t(text, open, sizeof(open));
    const int key_length  = 32;
    //256 битный ключ
    uint8_t main_key[key_length] = {0,0,0,0,0,0,7,4,4,2,1,1,2,3,4,5, 0,0,0,0,0,0,7,4,4,2,1,1,2,3,4,5};

    cout<<"Task 1. Cypher E2"<<endl;
    cout<<"Open text "<<endl;
    for (int i = 0; i <open_text_len ; ++i) {
        cout<<(char)open[i];
    }
    cout<<endl;
    crypt(encrypt, open, main_key, key_length);
    cout<<"Encrypted text (Encrypts first 128bits of text) "<<endl;
    for (int i = 0; i <open_text_len ; ++i) {
        cout<<(char)open[i];
    }
    cout<<endl;
    crypt(decrypt, open, main_key, key_length);
    cout<<"Decrypted text "<<endl;
    for (int i = 0; i <open_text_len ; ++i) {
        cout<<(char)open[i];
    }
    cout<<"\n\n";

    cout<<"Task 2. Hash function"<<endl;
    uint8_t temp[16] = {0,0,0,0,0,0,7,4,4,2,1,1,2,3,4,5};
    uint8_t *hash = temp;
    Hash_function(open, hash, sizeof(open));
    for (int j = 0; j < 16; ++j) {
        cout<<(int)hash[j];
    }
    cout<<"\n\n";

    cout<<"Task 3. Electronic signature"<<endl;
    BigInteger s[2] = {2,1};
    BigInteger* signature = s;
    BigInteger p = BigInteger("131");
    BigInteger q = BigInteger("13");
    BigInteger theta = BigInteger("23");
    BigInteger y = 0;
    create_signature(p, q, theta, signature, hash, y);
    cout<<"The signature is r="<<signature[0].getNumber()<<" s="<<signature[1].getNumber()<<endl;
    check_signature(signature[0], signature[1], p, q, y, hash, theta);
    cout<<"\n\n";

    cout<<"Task 4. Encryption regime"<<endl;
    cout<<"Text from Task 1 with  digital signature at the end: \n";
    //преобразование цифровой подписи в массив uint8_t
    uint8_t signature_array[signature[0].getNumber().length() + signature[1].getNumber().length()];
    for(int i = 0; i< sizeof(signature_array) ; i++){
        if(i<signature[0].getNumber().length()){
            signature_array[i] = (uint8_t)signature[0].getNumber()[i];} else{
            signature_array[i] = (uint8_t)signature[1].getNumber()[i-signature[0].getNumber().length()];
        }
    }
    //обьявление массивов, куда будут записаны зашифровавнные и расшифрованные данные
    int array_len_for_regime = open_text_len + sizeof(signature_array);
    uint8_t open_text_2[array_len_for_regime];
    uint8_t *encrypted_with_mode = open_text_2;
    uint8_t open_text_5[array_len_for_regime];
    uint8_t *decrypted_with_mode = open_text_5;
    uint8_t intit[16] = {0,0,0,0,0,0,7,4,4,2,1,1,2,3,4,5};
    uint8_t *intit_vector = intit;
    //обьявление и заполнение массива элементами исходного текста и , затем, цифровой подписи, после вывод его в консоль
    uint8_t encrypt_text_regime[array_len_for_regime];
    for (int j = 0; j < sizeof(encrypt_text_regime); ++j) {
        if(j < open_text_len){
            encrypt_text_regime[j] = open[j];
        } else{
            encrypt_text_regime[j] = signature_array[j-open_text_len];
        }
    }
    for (int k = 0; k< sizeof(encrypt_text_regime); k++){
        std::cout<<(char)encrypt_text_regime[k];
    }
    std::cout<<std::endl;
    //шифрование
    Block_chaining_mode(encrypt, encrypt_text_regime, main_key, intit_vector, array_len_for_regime, encrypted_with_mode, key_length);
    std::cout<<"Encrypted: \n";
    for (int k = 0; k<open_text_len+ sizeof(signature_array); k++){
        std::cout<<(char)encrypted_with_mode[k];
    }
    std::cout<<std::endl;
    //расшифрование
    Block_chaining_mode(decrypt, encrypted_with_mode, main_key, intit_vector, array_len_for_regime, decrypted_with_mode, key_length);
    std::cout<<"Decrypted text \n";
    for (int k = 0; k<open_text_len+ sizeof(signature_array); k++){
        std::cout<<(char)decrypted_with_mode[k];
    }
    cout<<endl;
    return 0;
}