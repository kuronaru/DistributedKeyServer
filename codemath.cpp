#include "codemath.h"

int BinPow(int a, int b, int p){// 二进制快速幂 a^b(mod p)
    a %= p;
    int res = 1;
    while(b > 0){
        if (b & 1) res = res * a % p;
        a = a * a % p;
        b >>= 1;
    }
    return res;
}

int ExGCD(int a, int b, int &x, int &y){// 扩展GCD
    if(b == 0){
        x = 1, y = 0;
        return a;
    }
    int d = ExGCD(b, a % b, x, y), t = x;
    x = y;
    y = t - a / b * x;
    return d;
}

int ExGcdInv(int a, int b){// 扩展GCD求乘法逆元
    int x, y;
    ExGCD(a, b, x, y);
    return x;
}

int ModSqrt(int a, int n){// x^2≡a(mod n) 模素数平方根算法，判断一个数g是否存在模p平方根，若存在则返回一个根，若不存在返回-1
    int b, k, i, x;
    if(n == 2) return a % n;
    if(BinPow(a, (n - 1) / 2, n) == 1){
        if(n % 4 == 3){
            x = BinPow(a, (n + 1) / 4, n);
        }else{
            for (b = 1; BinPow(b, (n - 1) / 2, n) == 1; b++);
            i = (n - 1) / 2;
            k = 0;
            while(i % 2 == 0){
                i /= 2, k /= 2;
                if ((BinPow(a, i, n) * BinPow(b, k, n) + 1) % n == 0) k += (n - 1) / 2;
            }
            x = BinPow(a, (i + 1) / 2, n) * BinPow(b, k / 2, n) % n;
        }
        if(x * 2 > n) x = n - x;
        return x;
    }
    return -1;
}

BigNum BinPow(BigNum a, BigNum b, BigNum p){// 二进制快速幂 a^b(mod p)
    a = a % p;
    BigNum res = 1;
    while(b > 0){
        if (b % 2) res = (res % p) * (a % p) % p;
        a = (a % p) * (a % p) % p;
        b = b / 2;
    }
    return res;
}

BigNum ExGCD(BigNum a, BigNum b, BigNum &x, BigNum &y){// 扩展GCD
    if(b == 0){
        x = 1, y = 0;
        return a;
    }
    BigNum d = ExGCD(b, a % b, x, y), t = x;
    x = y;
    y = t - a / b * x;
    return d;
}

BigNum ExGcdInv(BigNum a, BigNum b){// 扩展GCD求乘法逆元
    BigNum x, y;
    ExGCD(a, b, x, y);
    return x;
}

int FermatInv(int a, int b){// 费马小定理求乘法逆元
    return BinPow(a, b - 2, b);
}

BigNum ModSqrt(BigNum a, BigNum n){// x^2≡a(mod n) 模素数平方根算法，判断一个数g是否存在模p平方根，若存在则返回一个根，若不存在返回-1
    BigNum b, k, i, x;
    if(n == 2) return a % n;
    if(BinPow(a, (n - 1) / 2, n) == 1){
        if(n % 4 == 3){
            x = BinPow(a, (n + 1) / 4, n);
        }else{
            for (b = 1; BinPow(b, (n - 1) / 2, n) == 1; b = b + 1);
            i = (n - 1) / 2;
            k = 0;
            while(i % 2 == 0){
                i = i / 2, k = k / 2;
                if ((BinPow(a, i, n) * BinPow(b, k, n) + 1) % n == 0) k = k + (n - 1) / 2;
            }
            x = BinPow(a, (i + 1) / 2, n) * BinPow(b, k / 2, n) % n;
        }
        if(x * 2 > n) x = BigNum(n) - x;
        return x;
    }
    return -1;
}
