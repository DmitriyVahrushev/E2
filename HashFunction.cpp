//
// Created by Dmitriy on 01.07.2019.
//

#include <cstdint>
#include "HashFunction.h"
//хэш функция, возвращает 128 битный хэш
//Size - длина open_data в байтах
void Hash_function(uint8_t *Open_Data, uint8_t *hash, int Size)
{
    //длина хэша в байтах
    int len_of_hash = 16;
    //длина блока данных, 16 так как E2 шифрует блоками по 116 байт
    int bytes_data_block = 16;
    //количество блоков данных
    int num_of_blocks = Size / bytes_data_block + 1;
    // если размер текста кратен размеру блока и блоков  не ноль, то +1 из верхней строчки убирается
    if(Size%bytes_data_block == 0 && num_of_blocks != 0 ) {
        num_of_blocks--;
    }
    //data temp - копия open data , но с размером кратным размеру блока (дополненный с конца 0 в случае необходимости)
    uint8_t data_temp[num_of_blocks*bytes_data_block];
    memset(data_temp, 0, sizeof(data_temp));
    for (int m = 0; m < Size; ++m) {
        data_temp[m] = Open_Data[m];
    }
    //h = h0 h0 = 0
    for (int l = 0; l < len_of_hash; ++l) {
        hash[l] = 0;
    }
    uint8_t open_data_portions[num_of_blocks][bytes_data_block];
    //хэширование прозводится по схеме 1 . А = h, B = Mi, C = Mi
    for(int i = 0; i<num_of_blocks; i++){
        uint8_t cur_portion[bytes_data_block];
        //хэшируемые данные записываются в массив open_data_portions, поделенные на 128 битные блоки
        for(int k = 0; k< bytes_data_block; k++){
            open_data_portions[i][k] = data_temp[k+bytes_data_block*i];
            cur_portion[k] = open_data_portions[i][k];
        }
        //зашифрование порции данных Mi , где в качестве ключа выступаен хэш h
        crypt(encrypt, open_data_portions[i], hash, len_of_hash);
        //сложение получившегося массива с элементами хэша h
        for (int j = 0; j < bytes_data_block ; ++j) {
            hash[j]=open_data_portions[i][j]^cur_portion[j];
        }
    }
}