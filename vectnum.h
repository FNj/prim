#ifndef VECTNUM_H
#define VECTNUM_H
#include <vector>
#include <limits>
#include <chrono>
#include <random>
#include <stdint.h>

typedef uint32_t Uns;
typedef uint64_t UnsL;

using namespace std;

const Uns min_Uns = numeric_limits<Uns>::min();
const Uns max_Uns = numeric_limits<Uns>::max();

class vectnum //Little-endian representation of a natural number as a vector of unsigned integers (Uns for short). I.e. myVectnum[0] returns the least significant Uns.
{
public:
    vector<Uns> cntnr;
    inline vectnum() : cntnr(vector<Uns>()) {}
    inline vectnum(Uns filler) : cntnr(vector<Uns>()) {if(filler) cntnr.push_back(filler);}
    inline Uns& operator[](Uns i) {return cntnr.at(i);}
    inline Uns at(Uns i) const {if(i < cntnr.size()) return cntnr.at(i); else return 0;}
    inline void stripZeros();
    inline void padToSize(Uns size);
    vectnum &operator +=(const vectnum& rhs);
    bool subtract(const vectnum &rhs); //Returns false if *this < rhs; else it subtracts lhs from *this and returns true.
    vectnum &operator -=(const vectnum& rhs);
    vectnum &operator *=(const Uns& rhs);
    vectnum &operator *=(const vectnum& rhs);
    bool smallLeftShift(const unsigned char& shift);
    vectnum &multiplyBy2toThePower(const Uns &shift);
    bool smallRightShift(const unsigned char& shift);
    vectnum &divideBy2toThePower(const Uns &shift);
    unsigned char normalize(); //Returns normalization multiplier as number of bit shifts to the left applied.
    vectnum divideBy(const vectnum& rhs); //After application, *this is the remainder and return value is the quotient.
    vectnum &operator ^=(const Uns& rhs);
    vectnum &smallPow(const Uns& exponent, const vectnum& divisor); //exponention modulo divisor (like Python pow(*this,exponent,divisor) function)
    vectnum &pow(const vectnum& exponent, const vectnum& divisor); //exponention modulo divisor (like Python (*this,exponent,divisor) function)
    static default_random_engine generator;
    static vectnum random(const vectnum& maximum);
    void print();
};

inline bool operator ==(const vectnum &lhs, const vectnum &rhs){return (lhs.cntnr == rhs.cntnr);}
inline bool operator !=(const vectnum &lhs, const vectnum &rhs){return (lhs.cntnr != rhs.cntnr);}

       bool operator <(const vectnum& lhs, const vectnum& rhs);
inline bool operator >(const vectnum& lhs, const vectnum& rhs) {return rhs < lhs;}
inline bool operator <=(const vectnum& lhs, const vectnum& rhs) {return !(lhs > rhs);}
inline bool operator >=(const vectnum& lhs, const vectnum& rhs) {return !(lhs < rhs);}

inline vectnum operator +(vectnum lhs, const vectnum& rhs) {return lhs += rhs;}
inline vectnum operator -(vectnum lhs, const vectnum& rhs) {return lhs -= rhs;}
inline vectnum operator *(vectnum lhs, const Uns& rhs) {return lhs *= rhs;}
inline vectnum operator *(vectnum lhs, const vectnum& rhs) {return lhs *= rhs;}
inline vectnum operator <<(vectnum lhs, const Uns& rhs) {return lhs.multiplyBy2toThePower(rhs);}
inline vectnum operator >>(vectnum lhs, const Uns& rhs) {return lhs.divideBy2toThePower(rhs);}
inline vectnum operator /(vectnum lhs, const vectnum& rhs) {return lhs.divideBy(rhs);}
inline vectnum operator %(vectnum lhs, const vectnum& rhs) {lhs.divideBy(rhs); return lhs;}
inline vectnum operator ^(vectnum lhs, const Uns& rhs) {return lhs ^= rhs;}
inline vectnum smallPower(vectnum base, const Uns& exponent, const vectnum& divisor) {return base.smallPow(exponent, divisor);}
inline vectnum power(vectnum base, const vectnum& exponent, const vectnum& divisor) {return base.pow(exponent, divisor);}
#endif // VECTNUM_H

//TODO: multiplication, exp. by squaring, limited subtraction
