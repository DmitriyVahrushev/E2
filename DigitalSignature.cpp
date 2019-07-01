//
// Created by Dmitriy on 01.07.2019.
//

#include "DigitalSignature.h"
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