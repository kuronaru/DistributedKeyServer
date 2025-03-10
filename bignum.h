#ifndef BIGNUM_H
#define BIGNUM_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <algorithm>
using namespace std;

#define MAXN 9999
#define MAXSIZE 1000
#define DLEN 4
typedef long long LL;

class BigNum{
public:
    int a[500];// 可以控制大数的位数
    int len;// 大数长度

public:
    BigNum();// 构造函数
    BigNum(const int);// 将一个int类型的变量转化为大数
    BigNum(const char*, int = 10);// 将一个字符串类型的变量转化为大数
    BigNum(const BigNum&);// 拷贝构造函数

    BigNum& operator=(const BigNum&);// 重载赋值运算符，大数之间进行赋值运算
    friend istream& operator>>(istream&, BigNum&);// 重载输入运算符
    friend ostream& operator<<(ostream&, BigNum&);// 重载输出运算符

    BigNum operator+(const BigNum&) const;// 重载加法运算符，两个大数之间的相加运算
    BigNum operator-(const BigNum&) const;// 重载减法运算符，两个大数之间的相减运算
    BigNum operator*(const BigNum&) const;// 重载乘法运算符，两个大数之间的相乘运算
    BigNum operator/(const BigNum&) const;// 重载除法运算符，两个大数之间的相除运算
    BigNum operator%(const BigNum&) const;// 重载取模运算符，两个大数之间的取模运算
    BigNum operator/(const int&) const;// 重载除法运算符，大数对一个整数进行相除运算
    BigNum operator^(const int&) const;// 大数的n次方运算
    int operator%(const int&) const;// 大数对一个int类型的变量进行取模运算

    bool operator>(const BigNum&) const;// 大数和另一个大数的大小比较
    bool operator<(const BigNum&) const;
    bool operator>(const int&) const;// 大数和一个int类型的变量的大小比较
    bool operator<(const int&) const;
    bool operator==(const BigNum&) const;// 两个大数之间的相等比较
    bool operator==(const int&) const;// 大数和一个int类型的变量的相等比较

    static BigNum random(int bytes = 8);// 生成BYTES位的大随机数

    void print();// 输出大数
    string toString();// 大数转换字符串
};

#endif // BIGNUM_H
