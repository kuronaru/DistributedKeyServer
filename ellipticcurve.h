#ifndef ELLIPTICCURVE_H
#define ELLIPTICCURVE_H

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <QStringList>
#include "point.h"
#include "codemath.h"

class EllipticCurve {
private:
    // y^2 = x^3 + ax + b, a, b ∈ Fp, (4a^3 + 27b^2) mod p != 0
    int a;
    int b;
    int p;// 大素数 例：68,719,476,736
    Point G;// 基点G
public:
    explicit EllipticCurve(int p);// 生成新的椭圆曲线
    explicit EllipticCurve(int a, int b, int p, Point G);// 构造椭圆曲线
    ~EllipticCurve();
    Point plusPoint(Point &pa, Point &pb);// Fp（素域）上的椭圆曲线群加法 Q=P1+P2
    Point minusPoint(Point &pa, Point &pb);// Fp（素域）上的椭圆曲线群减法 Q=P1-P2
    Point multiplyPoint(Point &pa, int k);// Fp（素域）上的椭圆曲线群乘法 Q=[k]P
    QStringList getParam(void);
    Point getPointG(void);
};

#endif // ELLIPTICCURVE_H
