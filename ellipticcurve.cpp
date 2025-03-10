#include "ellipticcurve.h"

EllipticCurve::EllipticCurve(int p){
    this->p = p;
    // 随机产生小于p-1的正整数a，b，满足4a^3+27b^2!=0(mod p)
    srand(time(0));
    int a, b;
    a = rand() % (p - 2) + 1;
    b = rand() % (p - 2) + 1;
    while((BinPow(a, 3, p) * 4 + BinPow(b, 2, p) * 27) % p == 0){
        b = rand() % (p - 2) + 1;
    }
    this->a = a;
    this->b = b;
    // 随机产生合法的基点G(x,y)
    int alpha, x, y;
    while(1){
        x = rand() % (p - 1);
        alpha = (BinPow(x, 3, p) + a * x + b) % p;
        if(alpha == 0){
            y = 0;
            break;
        }
        int root = ModSqrt(alpha, p);
        if(!(root == -1)){
            y = root;
            break;
        }
    }
    this->G.x = x;
    this->G.y = y;
}

EllipticCurve::EllipticCurve(int a, int b, int p, Point G){
    this->a = a;
    this->b = b;
    this->p = p;
    this->G = G;
}

EllipticCurve::~EllipticCurve(){
}

Point EllipticCurve::plusPoint(Point &pa, Point &pb){
    int k;
    int x, y;
    if(pa.x == 0 && pa.y == 0) return pb;
    if(pb.x == 0 && pb.y == 0) return pa;
    if(pa.x == pb.x && (pa.y + pb.y) % p == 0) return Point(0, 0);
    if(pa == pb){// 倍点公式
        k = ((pa.x * pa.x * 3 + a) * FermatInv(pa.y * 2, p)) % p;
        // k = (3x1^2+a)/2y1
        x = (k * k - pa.x * 2) % p;
        y = (k * (pa.x - x) - pa.y) % p;
    }else{
        k = ((pb.y + (p - (pa.y % p))) * FermatInv(pb.x + (p - (pa.x % p)), p)) % p;
        // k = (y2-y1)/(x2-x1)
        x = (k * k - pa.x - pb.x) % p;
        y = (k * (pa.x - x) - pa.y) % p;
    }
    return Point(x, y);
}

Point EllipticCurve::minusPoint(Point &pa, Point &pb){
    // Q=P1-P2=P1+(-P2)
    Point pc(pb.x, 0 - pb.y);
    return plusPoint(pa, pc);
}

Point EllipticCurve::multiplyPoint(Point &pa, int k){
    int a[500];
    int a_count = 0;
    while(k > 0) {
        a[a_count++] = k % 2;
        k = k / 2;
    }
    Point res(0, 0);
    while(a_count){
        res = plusPoint(res, res);
        if(a[--a_count]){
            res = plusPoint(res, pa);
        }
    }
    return res;
}

QStringList EllipticCurve::getParam(){
    QStringList param;
    param << QString::number(a) << QString::number(b) << QString::number(p)
          << QString::number(G.x) << QString::number(G.y);
    return  param;
}

Point EllipticCurve::getPointG(){
    return G;
}
