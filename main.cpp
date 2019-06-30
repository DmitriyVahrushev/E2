#include <iostream>
#include <stdlib.h>
#include "BigInteger.h"
#include <string>
#include <cmath>
#include <cstring>

#define encrypt 1
#define decrypt 0
#define BYTES_OF_DATA_BLOCK 16

using namespace std;

//Таблица замен
uint8_t SBOX[256] = {225, 66, 62, 129, 78, 23, 158, 253, 180, 63, 44, 218, 49, 30, 224, 65,
        204, 243, 130, 125, 124, 18, 142, 187, 228, 88, 21, 213, 111, 233, 76, 75,
        53, 123, 90, 154, 144, 69, 188, 248, 121, 214, 27, 136, 2, 171, 207, 100,
        9, 12, 240, 1, 164, 176, 246, 147, 67, 99, 134, 220, 17, 165, 131, 139,
        201, 208, 25, 149, 106, 161, 92, 36, 110, 80, 33, 128, 47, 231, 83, 15,
        145, 34, 4, 237, 166, 72, 73, 103, 236, 247, 192, 57, 206, 242, 45, 190,
        93, 28, 227, 135, 7, 13, 122, 244, 251, 50, 245, 140, 219, 143, 37, 150,
        168, 234, 205, 51, 101, 84, 6, 141, 137, 10, 94, 217, 22, 14, 113, 108,
        11, 255, 96, 210, 46, 211, 200, 85, 194, 35, 183, 116, 226, 155, 223, 119,
        43, 185, 60, 98, 19, 229, 148, 52, 177, 39, 132, 159, 215, 81, 0, 97,
        173, 133, 115, 3, 8, 64, 239, 104, 254, 151, 31, 222, 175, 102, 232, 184,
        174 ,189, 179, 235, 198, 107, 71, 169, 216, 167, 114, 238, 29, 126, 170, 182,
        117, 203, 212, 48, 105, 32, 127, 55, 91, 157, 120, 163, 241, 118, 250, 5,
        61, 58, 68, 87, 59, 202, 199, 138, 24, 70, 156, 191, 186, 56, 86, 26,
        146, 77, 38, 41, 162, 152, 16, 153, 112, 160, 197, 40, 193, 109, 20, 172,
        249, 95, 79, 196, 195, 209, 252, 221, 178, 89, 230, 181, 54, 82, 74, 42};

//функция возвращает 64 битное число типа uint64_t из массива из 8 элементов типа uint8_t
uint64_t uint8touint64(uint8_t* v4){
    uint64_t temp1=0;
    for (int i=0; i<8; ++i)
        temp1 = (temp1 << 8) | v4[i];
    return temp1;
}

//функция записывает в d массив из 4 элементов типа uint8_t на основе 32битного числа типа uint64_t
void uint32touint8(uint32_t c, uint8_t* d){
    for (int i=0; i<4 ;++i)
        d[i] = ((uint8_t*)&c)[3-i];
}

//функция возвращает 32 битное число типа uint64_t из массива из 4 элементов типа uint8_t
uint32_t uint8touint32(uint8_t* v4){
    uint32_t i32 = v4[3] | (v4[2] << 8) | (v4[1] << 16) | (v4[0] << 24) ;
    return i32;
}

//функция записывает в res массив из 8 элементов типа uint8_t на основе 64битного числа типа uint64_t
void uint64to8uint8(uint64_t x, uint8_t *res){
    for (int i=0; i<8 ;++i)
        res[i] = ((uint8_t*)&x)[7-i];
}

//BP(X)
void BP(uint8_t *x, uint8_t *res1){
    for(int i = 0; i<16; i++){
        res1[i] = x[(5*i)%16];
    }
}

void operation1( uint8_t *x64, uint8_t *y64, uint8_t *u64){
    int num_of_bytes = 4;
    uint32_t x[num_of_bytes];
    uint32_t y[num_of_bytes];
    uint32_t u[num_of_bytes];
    for (int j = 0; j < num_of_bytes ; ++j) {
        x[j] = uint8touint32(x64+4*j);
        y[j] = uint8touint32(y64+4*j);
    }
    for (int i = 0; i < num_of_bytes; ++i) {
        uint32_t  z = y[i];
        if (z%2 == 0){
            z++;
        }
        u[i] = (uint32_t)(x[i]*z);

    }
    for (int k = 0; k < num_of_bytes; ++k) {
        uint32touint8(u[k], u64+k*4);
    }
}
//расширенный алгоритм Евклида
uint64_t gcdex(uint64_t a, uint64_t b, uint64_t &x, uint64_t &y) {
    if (b == 0) {
        x = 1;
        y = 0;
        return a;
    }
    uint64_t x1, y1;
    uint64_t d1 = gcdex(b, a % b, x1, y1);
    x = y1;
    y = x1 - (a / b) * y1;
    return d1;
}

// функция для нахождения мультипликативного обратного к  a по модулю N на основе расширенного алгоритма Евклида
uint64_t ReverseElement(uint32_t a, uint64_t N, uint64_t &result) {
    uint64_t x, y, d;
    d = gcdex(a, N, x, y);
    if (d != 1) {
        return 1;
    } else {
        result = x;
        return 0;
    }
}

void operation2(uint8_t *x64, uint8_t *y64, uint8_t *w64){
    int num_of_bytes = 4;
    uint32_t x[num_of_bytes];
    uint32_t y[num_of_bytes];
    uint32_t w[num_of_bytes];
    for (int j = 0; j < num_of_bytes ; ++j) {
        x[j] = uint8touint32(x64+4*j);
        y[j] = uint8touint32(y64+4*j);
    }

    for (int i = 0; i < num_of_bytes; ++i) {
        uint32_t  z = y[i];
        if (z%2 == 0){
            z++;
        }
        uint64_t pow_2_to_32 = 0x0000000100000000;
        uint64_t x_1;
        ReverseElement(z, pow_2_to_32, x_1);
        w[i] = (uint32_t) (x[i]*x_1);
    }
    for (int k = 0; k < num_of_bytes; ++k) {
        uint32touint8(w[k], w64+k*4);
    }
}
//BP^-1(X)
void BPInv(uint8_t *x, uint8_t *res1){
    for(int i = 0; i<16; i++){
        res1[i] = x[(13*i)%16];
    }
}
//S(X) производится подстановка из таблицы замен
uint64_t S(uint64_t X){
    uint64_t res = 0;
    uint64_t tempX = X;
    for(int i = 0; i<8; i++){
        auto first = (uint8_t)(_rotr64(tempX, 56-8*i));
        res ^= SBOX[first];
        res = _rotl64(res, 8*(7-i));
    }
    return res;
}

//P(X)
uint64_t P(uint64_t X){
    uint64_t res = 0x000000000000000;
    uint8_t y[8];
    uint8_t x[8];
    uint64to8uint8(X, x);
    y[7] =  x[7]^x[3]; y[6] = x[6]^x[2]; y[5] = x[5]^x[1]; y[4] = x[4]^x[0];
    y[3] =  x[3]^x[5]; y[2] = x[2]^x[4]; y[1] = x[1]^x[7]; y[0] = x[0]^x[6];
    y[7] = y[7]^y[2]; y[6] = y[6]^y[1]; y[5] = y[5]^y[0]; y[4] = y[4]^y[3];
    y[3] = y[3]^y[7]; y[7] = y[2]^y[6]; y[1] = y[1]^y[5]; y[0] = y[0]^y[4];
    res = uint8touint64(y);
    return res;
}
//циклический сдвиг влево на 8 бит
uint64_t lrot(uint64_t a){
    //обычный сдвиг влево на 8 бит
    uint64_t b = _rotl64(a, 8);
    //в с хранятся первые 8 бит, а сдвинуты в последние 8 разрядов
    uint64_t c = _rotr64(0xFF00000000000000&a, 56);
    return b^c;
}
//раундовая функция
uint64_t F(uint64_t R, const uint64_t *roundk){
    return lrot(S(P(S(R^roundk[0]))^roundk[1]));
}

void G( uint64_t *X, uint64_t U, uint64_t *L, uint64_t *Y, uint64_t V){
    for (int i = 0; i < 4; ++i) {
        Y[i] = P(S(X[i]));
     //   cout<<X[i]<<" "<<Y[i]<<" "<<S(X[i])<<endl;
    }
    //cout<<endl;
    L[0] = Y[0] ^ P(S(U));
    for (int j = 1; j < 4; ++j) {
        L[j] = P(S(L[j-1]));

    }
    V = L[3];


}
//генерирует раундовые ключи
void generate_round_keys( uint8_t *key,  uint8_t round_keys[16][16], int key_size_bytes){
    uint64_t g = 0x0123456789abcdef;
    uint64_t K[4];
    K[0] = uint8touint64(key); K[1] = uint8touint64(key+4);
    if(key_size_bytes == 16){
        K[2] = S(S(S(g))); K[3] = S(K[2]);
    } else if(key_size_bytes == 24){
        K[2] = uint8touint64(key+8); K[3] = S(S(S(S(g))));
    } else if(key_size_bytes == 32){
        K[2] = uint8touint64(key+8); K[3] = uint8touint64(key+12);
    }
    uint64_t L[4] = {0, 0, 0, 0};
    uint64_t Y[4] = {0, 0, 0, 0};
    uint64_t  U = g;
    uint64_t new_U = U;
    G(K, U, L, Y, new_U);
    U = new_U;
    int p = 0;
    uint8_t q[32][8];
    for(int i = 0; i<8; i++){
        uint64_t Y_new[4] = {0, 0, 0, 0};
        G(Y, U, L, Y_new, new_U);
        for (int k = 0; k < 4; ++k) {
            Y[k] = Y_new[k];
        }
        U = new_U;
        for (int j = 0; j < 4; ++j) {
            uint64to8uint8( L[j], q[4*i+j]);
        }
    }
    for(int i = 0; i<8; i++){
        for (int k = 0; k < 16; ++k) {
            round_keys[2*i+1][k] = q[2*k][p];
            round_keys[2*i][k] = q[2*k+1][p];
        }
        p++;
    }
}
//функция осуществляющая шифрование/расшифрование 128 блока данных с данным ключом
void crypt(int mode, uint8_t *input_text, uint8_t *key, int key_size_bytes){
    const int key_bytes = 16;
    const int num_of_keys = 16;
    //генерация раундовых ключей
    uint8_t round_keys[num_of_keys][key_bytes];
    uint8_t round_keys_decr[num_of_keys][key_bytes];
    generate_round_keys(key, round_keys,  key_size_bytes);
    //вывод раундовых ключей в консоль
//    for (int m = 0; m < num_of_keys; ++m) {
//        for (int i = 0; i < key_bytes; ++i) {
//            cout<<round_keys[m][i]<<" ";
//        }
//        cout<<endl;
//    }
    if(mode == decrypt){
        //в случае расшифрования - другая последательность раундовых ключей
        for (int i = 0; i < 12 ; ++i) {
            for (int j = 0; j < key_bytes; ++j) {
                round_keys_decr[i][j] = round_keys[11-i][j];
            }
        }
        for (int i = 12; i < 16 ; ++i) {
            for (int j = 0; j < key_bytes; ++j) {
                round_keys_decr[i][j] = round_keys[15-(i-12)][j];
            }
        }
        for (int i = 0; i < num_of_keys ; ++i) {
            for (int j = 0; j <key_bytes ; ++j) {
                round_keys[i][j] = round_keys_decr[i][j];
            }
        }
    }
    //Начальное преобразовавие (M = BP((M+K13)op2 K14))
    uint8_t tempM[BYTES_OF_DATA_BLOCK];
    for (int k = 0; k < key_bytes; ++k) {
        tempM[k] = input_text[k]^round_keys[12][k];
    }
    operation1(tempM, round_keys[13], tempM);
    uint8_t tempM2[16];
    BP(tempM, tempM2);
    //12 раундов шифрования по схеме Фейстеля
    uint64_t L = uint8touint64(tempM2);
    uint64_t R = uint8touint64(tempM2+8);
    for (int i = 0; i < 11; ++i) {
        uint64_t cur_key[2];
        cur_key[0] = uint8touint64(round_keys[i]);
        cur_key[1] = uint8touint64(round_keys[i]+8);
        L ^= F(R, cur_key);
        swap(L,R);
    }
    uint64_t cur_key[2];
    cur_key[0] = uint8touint64(round_keys[11]);
    cur_key[1] = uint8touint64(round_keys[11]+8);
    L ^= F(R, cur_key);
    //Заключительное преобразование
    uint64to8uint8(L, tempM2);
    uint64to8uint8(R, tempM2+8);
    uint8_t tempM3[16];
    BPInv(tempM2, tempM3);
    operation2(tempM3, round_keys[14], tempM3);
    for (int l = 0; l < BYTES_OF_DATA_BLOCK; ++l) {
        tempM3[l] ^= round_keys[15][l];
    }
    //запись результата в выходной массив
    for (int l = 0; l < BYTES_OF_DATA_BLOCK; ++l) {
        input_text[l] = tempM3[l];
    }
}

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

//функция для вовзедения в степень по модулю. Используются "длинные" числа
BigInteger powmod(BigInteger a, BigInteger d, BigInteger n)
{
    BigInteger b = 1;
    while (d>0)
    {
        if (d % 2 == 0)
        {
            d /= 2;
            a = (a * a) % n;
        }
        else
        {
            d--;
            b = (b * a) % n;
        }
    }
    return b;
}

//возведение в степень
BigInteger binpow (BigInteger a, int n) {
    if (n == 0) return 1;
    if (n % 2 == 1) return binpow (a, n-1) * a;
    else {
        BigInteger b = binpow (a, n/2);
        return b * b;
    }
}
//преобразование хэща в большое число
BigInteger hash_to_bigint(const uint8_t* hash, int len_of_hash){
    int power = 0;
    BigInteger base16 = 16;
    BigInteger res = 0;
    for (int i = 0; i <  len_of_hash; ++i) {
        int low =  (hash[len_of_hash-1 - i]&0x0F);//младшие биты
        BigInteger lowbits = BigInteger(low);
        int hi =  ((hash[len_of_hash-1 - i]&0xF0)>>4);//старшие биты
        BigInteger hibits = BigInteger(hi);
        res += lowbits*binpow(base16,power) + hibits*binpow(base16, power+1 );
        power += 2;
    }
    return res;
}

//функция для создания подписи
void create_signature(BigInteger p, BigInteger q, BigInteger theta, BigInteger* signature, const uint8_t* hash, BigInteger &y)
{
    BigInteger g = powmod(theta, (p-1)/q, p);
    //Преобразвание хэша к типу BigInteger
    BigInteger temp = hash_to_bigint(hash, 16);
    //инициализируются x, k
    BigInteger x = BigInteger("8");
    BigInteger k = BigInteger("7");
    y= powmod(g, x,  p);
    //Нахождение r, s . Запись их в выходной массив signature. Для нахождения s взята 6-ая схема цифровой подписи
    BigInteger r = powmod(g, k,p);
    BigInteger pi = r%q;
    BigInteger s = (temp*k - ((pi*x)))%q+q;
    s = s%q;
    signature[0] = r;
    signature[1] = s;
}

//функция для проверки подписи
void check_signature(BigInteger r, BigInteger s, BigInteger p, BigInteger q, BigInteger &y, const uint8_t* hash, BigInteger theta)
{
    BigInteger pi = r%q;
    //Преобразвание хэша к типу BigInteger
    BigInteger temp = hash_to_bigint(hash, 16);
    BigInteger g = powmod(theta, (p-1)/q, p);
    //Вычисление левой и правой частей уравнения проверки.
    BigInteger right = powmod(r, temp, p);
    BigInteger left = (powmod(g, s, p)* powmod(y,pi,p))%p;
    cout<<"right = "<<right.getNumber()<<"  left = "<<left.getNumber()<<endl;
    //Сравнение двух частей. В случае их совпадения - хэш соответствует подписи.
    if ( right == left ){
        std::cout<<"Successful \n";
    } else{
        std::cout<<"Unsuccessful \n";
    }
}

//Функция реализующая режим шифрования - режим сцепления блоков .
void Block_chaining_mode(int mode, uint8_t* data, uint8_t* key, uint8_t* init_vector, int Size, uint8_t* result, int len_of_key_bytes){
    //длина блока данных
    int bytes_data_block = BYTES_OF_DATA_BLOCK;
    uint8_t F[bytes_data_block];
    for (int m = 0; m < bytes_data_block; ++m) {
        F[m] = init_vector[m];
    }
    //количество блоков данных
    int num_of_blocks = Size / bytes_data_block + 1;
    // если размер текста кратен размеру блока и блоков  не ноль
    if(Size%bytes_data_block == 0 && num_of_blocks != 0 ) {
        num_of_blocks--;
    }
    //data temp - копия data , но с размером кратным размеру блока (дополненный с конца 0 в случае необходимости)
    uint8_t data_temp[num_of_blocks*bytes_data_block];
    memset(data_temp, 0, sizeof(data_temp));
    for (int m = 0; m <= Size; ++m) {
        if (m!= Size){
        data_temp[m] = data[m];} else{
            //добавление битовой еденицы после окончания сообщения
            data_temp[m] = 0x80;
        }
    }
    //массив содержащий элементы из data, разбитые  на 128 битные  блоки
    uint8_t temp_res[num_of_blocks][bytes_data_block];
    if (mode == encrypt){
        for (int i = 0; i < num_of_blocks; i++) {
            //Pi+Fi (сложение по модулю 2) Pi - блок открытого текста , здесь data.
            for(int k = 0; k< bytes_data_block; k++){
                temp_res[i][k] = data_temp[k+bytes_data_block*i]^F[k];
            }
            //шифрование - Ci = crypt(Pi+Fi)
            crypt(encrypt, temp_res[i], key, len_of_key_bytes);
            //обновление F. Fi = F(i-1)+C(i-1)(сложение по модулю 2)
            for (int j = 0; j < bytes_data_block; j++) {
                F[j] ^= temp_res[i][j];
            }
        }
    } else{//расшифрование

        for (int i = 0; i < num_of_blocks; i++) {
            uint8_t temp_res_2[bytes_data_block];
            //заполение temp_res зашифрованными данными из data
            for(int k = 0; k< bytes_data_block; k++){
                temp_res[i][k] = data_temp[k+bytes_data_block*i];
                //записывается копия Ci
                temp_res_2[k] = temp_res[i][k];
            }
            //расшифрование temp_res , т.е. D(Ci)
            crypt(decrypt, temp_res[i], key, len_of_key_bytes);
            //Pi = D(Ci)+Fi (сложение по модулю 2)
            for (int l = 0; l < bytes_data_block; l++) {
                temp_res[i][l] ^=F[l];
            }
            //обновление F. Fi = F(i-1)+C(i-1)(сложение по модулю 2)
            for (int j = 0; j < bytes_data_block; j++) {
                F[j] ^= temp_res_2[j];
            }
        }
    }
    //запись полученных значений в выходный массив result
    for (int i = 0; i < num_of_blocks; ++i) {
        for (int k = 0; k <bytes_data_block ; ++k) {
            if(i*bytes_data_block + k < Size) {
                result[k + bytes_data_block * i] = temp_res[i][k];
            }
        }
    }
}

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