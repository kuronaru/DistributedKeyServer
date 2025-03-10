#ifndef CODEMATH_H
#define CODEMATH_H

#include "bignum.h"

typedef long long LL;
int BinPow(int a, int b, int p);// 二进制快速幂 a^b(mod p)
int ExGCD(int a, int b, int &x, int &y);// 扩展GCD，引用参数x，y返回一对可行解
int ExGcdInv(int a, int b);// 扩展GCD求乘法逆元
int ModSqrt(int a, int n);// 模素数平方根算法 x^2≡a(mod n)，判断一个数g是否存在模p平方根，若存在则返回一个根，若不存在返回-1

BigNum BinPow(BigNum a, BigNum b, BigNum p);// 二进制快速幂 a^b(mod p)
BigNum ExGCD(BigNum a, BigNum b, BigNum &x, BigNum &y);// 扩展GCD，引用参数x，y返回一对可行解
BigNum ExGcdInv(BigNum a, BigNum b);// 扩展GCD求乘法逆元
int FermatInv(int a, int b);// 大数费马小定理求乘法逆元
BigNum ModSqrt(BigNum a, BigNum n);// 大数模素数平方根算法 x^2≡a(mod n)，判断一个数g是否存在模p平方根，若存在则返回一个根，若不存在返回-1

#endif // CODEMATH_H
