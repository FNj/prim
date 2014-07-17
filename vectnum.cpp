#include "vectnum.h"
#include <vector>
#include <iostream>
//#include <bitset>
//#include <algorithm>

using namespace std;

const unsigned char UnsSize = (sizeof(Uns)*CHAR_BIT);

const vectnum unit(1);
const vectnum zero;
const vectnum two(2);

void vectnum::stripZeros()
{
    if(!cntnr.empty()) while (cntnr.back() == 0) cntnr.pop_back();
}

void vectnum::padToSize(Uns size)
{
    cntnr.reserve(size);
    while (cntnr.size()<size) cntnr.push_back(0);
}

vectnum &vectnum::operator+=(const vectnum &rhs)
{
    if (rhs.cntnr.empty()) return *this;
    if (cntnr.empty())
    {
        cntnr = rhs.cntnr;
        return *this;
    }
    Uns maxSize = max(cntnr.size(),rhs.cntnr.size())+1;
    padToSize(maxSize);
    UnsL c = 0;
    for (Uns i = 0; i < maxSize; ++i)
    {
      c = ((UnsL)at(i)) + ((UnsL) rhs.at(i)) + *(((Uns*)&c) + 1);
      cntnr[i] = *(Uns*)&c;
    }
    stripZeros();
    return *this;
}

bool vectnum::subtract(const vectnum &rhs)
{
    if (*this < rhs) return false;
    if (rhs.cntnr.empty()) return true;
    UnsL c = 0;
    for (Uns i = 0; i < cntnr.size(); ++i)
    {
      c = ((UnsL)at(i)) - ((UnsL) rhs.at(i)) - *(((Uns*)&c) + 1);
      cntnr[i] = *(Uns*)&c;
      if (*(((Uns*)&c) + 1)) *(((Uns*)&c) + 1) = 1;
    }
    stripZeros();
    return true;
}

vectnum &vectnum::operator -=(const vectnum &rhs)
{
    if (!subtract(rhs)) cerr << "Error in subtraction; returning lefthand side." << endl;
    return *this;
}

vectnum &vectnum::operator *=(const Uns &rhs)
{
   if (rhs == 0)
   {
       cntnr.clear();
       return *this;
   }
   if (rhs == 1) return *this;
   Uns maxSize = cntnr.size()+1;
   padToSize(maxSize);
   UnsL c = 0;
   for (Uns i = 0; i < maxSize; ++i)
   {
     c = ((UnsL)at(i)) * ((UnsL) rhs) + *(((Uns*)&c) + 1);
     cntnr[i] = *(Uns*)&c;
   }
   stripZeros();
   return *this;
}

vectnum &vectnum::operator *=(const vectnum &rhs)
{
    if (rhs.cntnr.empty())
    {
        cntnr.clear();
        return *this;
    }
    if (rhs == unit) return *this;
    Uns maxSize = cntnr.size() + rhs.cntnr.size();
    vectnum output(0);
    output.padToSize(maxSize);
    for (Uns i = 0; i < maxSize; ++i)
    {
      vectnum p(((*this) * rhs.at(i)));
      p.cntnr.insert(p.cntnr.begin(), i, 0);
      output += p;
    }
    output.stripZeros();
    *this = output;
    return *this;
}

const unsigned char maxShift = UnsSize - 1;

bool vectnum::smallLeftShift(const unsigned char &shift)
{
    if (shift > maxShift) return false;
    if (shift == 0) return true;
    Uns maxSize = cntnr.size()+1;
    padToSize(maxSize);
    UnsL c = 0;
    for (Uns i = 0; i < maxSize; ++i)
    {
      c = (((UnsL)at(i)) << shift) + *(((Uns*)&c) + 1);
      cntnr[i] = *(Uns*)&c;
    }
    stripZeros();
    return true;
}

vectnum &vectnum::multiplyBy2toThePower(const Uns &shift)
{
    if (shift == 0) return *this;
    Uns q = shift / maxShift;
    unsigned char r = shift % maxShift;
    for (Uns i = 0; i < q; ++i)
        if(!smallLeftShift(maxShift)) cerr << "Error in bit shifting." << endl;
    if(!smallLeftShift(r)) cerr << "Error in bit shifting." << endl;
    return *this;
}

bool vectnum::smallRightShift(const unsigned char &shift)
{
    if (shift > maxShift) return false;
    if (shift == 0) return true;
    Uns maxSize = cntnr.size();
    Uns high = 0;
    Uns low = 0;
    for (Uns i = maxSize; i > 0; --i)
    {
      high = ((at(i-1) >> shift) + low);
      low = (at(i-1) << (UnsSize-shift));
      cntnr[i-1] = high;
    }
    stripZeros();
    return true;
}

vectnum &vectnum::divideBy2toThePower(const Uns &shift)
{
    if (shift == 0) return *this;
    Uns q = shift / maxShift;
    unsigned char r = shift % maxShift;
    for (Uns i = 0; i < q; ++i)
        if(!smallRightShift(maxShift)) cerr << "Error in bit shifting." << endl;
    if(!smallRightShift(r)) cerr << "Error in bit shifting." << endl;
    stripZeros();
    return *this;
}

const Uns normalization_constant = max_Uns / 2;

unsigned char vectnum::normalize()
{
    unsigned char d = 0;
    stripZeros();
    Uns t = cntnr.back();
    while (t < normalization_constant) {
        ++d;
        t = t << 1;
    }
    if(!smallLeftShift(d)) cerr << "Error in normalization." << endl;
    return d;
}

const UnsL base = (UnsL)max_Uns + 1;

vectnum vectnum::divideBy(const vectnum &rhs) //*this is dividend; rhs is divisor
{
    if (*this < rhs) // dividend < divisor
    {
        return zero;
    }
    if (rhs==*this) //division by dividend itself
    {
        cntnr.clear();
        return unit;
    }
    vectnum divisor(rhs);
    if (divisor.cntnr.empty())
    {
        cerr << "Division by zero; outputting as if divisor is 1." << endl;
        divisor = unit;
    }
    vectnum quotient;
    if (divisor == unit) //division by 1
    {
        quotient = *this;
        cntnr.clear();
        return quotient;
    }
    unsigned char d = divisor.normalize();
    if(!smallLeftShift(d)) cerr << "Error in normalization." << endl; //both arguments normalized
    Uns n = divisor.cntnr.size();
    Uns j = cntnr.size() - n + 1; //initialize j
    quotient.padToSize(j);
    while (true)
    {
        --j;
        UnsL q = (base*at(j+n) + at(j+n-1)) / divisor.at(n-1);
        UnsL r = (base*at(j+n) + at(j+n-1)) % divisor.at(n-1);
        while ((r < base) && ((q==base) || (q*divisor.at(n-2) > (base*r + at(j+n-2)))))
        {
            --q;
            r += divisor.at(n-1);
        }
        vectnum tmp((divisor*((Uns)q)));
        tmp.cntnr.insert(tmp.cntnr.begin(), j, 0);
        if (*this < tmp)
        {
            --q;
            vectnum tmp2(divisor);
            tmp2.cntnr.insert(tmp2.cntnr.begin(), j, 0);
            tmp-=tmp2;
        }
        operator -=(tmp);
        quotient[j] = q;
        if (j == 0) break;
    }
    if(!smallRightShift(d)) cerr << "Error in bit shifting." << endl; //denormalization of remainder
    stripZeros();
    quotient.stripZeros();
    return quotient;
}

vectnum &vectnum::operator ^=(const Uns &rhs)
{
    if (rhs==0)
    {
        *this = unit;
        return *this;
    }
    if (rhs==1) return *this;
    if (rhs % 2 == 0)
    {
        operator *=(*this);
        operator ^=(rhs/2);
        return *this;
    }
    else
    {
        vectnum tmp(*this);
        operator *=(*this);
        operator ^=((rhs-1)/2);
        operator *=(tmp);
        return *this;
    }
}

vectnum &vectnum::smallPow(const Uns &exponent, const vectnum &divisor)
{
    if (exponent==0)
    {
        *this = unit;
        return *this;
    }
    if (exponent==1)
    {
        divideBy(divisor);
        return *this;
    }
    if (exponent % 2 == 0)
    {
        divideBy(divisor);
        operator *=(*this);
        divideBy(divisor);
        smallPow(exponent >> 1, divisor);
        divideBy(divisor);
        return *this;
    }
    else
    {
        divideBy(divisor);
        vectnum tmp(*this);
        operator *=(*this);
        divideBy(divisor);
        smallPow(exponent >> 1, divisor);
        divideBy(divisor);
        operator *=(tmp);
        divideBy(divisor);
        return *this;
    }
}

vectnum &vectnum::pow(const vectnum &exponent, const vectnum &divisor)
{
    if (exponent.cntnr.empty())
    {
        *this = unit;
        return *this;
    }
    if (exponent == unit)
    {
        divideBy(divisor);
        return *this;
    }
    if (exponent.cntnr.size() == 1)
    {
        smallPow(exponent.at(0), divisor);
        return *this;
    }
    if (exponent % two == 0)
    {
        divideBy(divisor);
        operator *=(*this);
        divideBy(divisor);
        pow(exponent >> 1, divisor);
        divideBy(divisor);
        return *this;
    }
    else
    {
        divideBy(divisor);
        vectnum tmp(*this);
        operator *=(*this);
        divideBy(divisor);
        pow(exponent >> 1, divisor);
        divideBy(divisor);
        operator *=(tmp);
        divideBy(divisor);
        return *this;
    }
}

default_random_engine vectnum::generator(chrono::system_clock::now().time_since_epoch().count());

vectnum vectnum::random(const vectnum &maximum)
{
    if (maximum.cntnr.empty()) return zero;
    vectnum output;
    if (maximum.cntnr.size() == 1)
    {
        uniform_int_distribution<Uns> distribution_only(2,maximum.at(maximum.cntnr.size()-1));
        output.cntnr.push_back(distribution_only(generator));
    }
    else
    {
        uniform_int_distribution<Uns> distribution_least_sig(2,max_Uns);
        uniform_int_distribution<Uns> distribution_middle(0,max_Uns);
        uniform_int_distribution<Uns> distribution_most_sig(0,maximum.at(maximum.cntnr.size()-1));
        output.cntnr.reserve(maximum.cntnr.size());
        for (Uns i = 0; i < maximum.cntnr.size(); ++i)
        {
            if(i == 0)
                output.cntnr.push_back(distribution_least_sig(generator));
            else if (i == (maximum.cntnr.size() - 1))
                output.cntnr.push_back(distribution_most_sig(generator));
            else
                output.cntnr.push_back(distribution_middle(generator));
        }
    }
    output.stripZeros();
    return output;
}

void vectnum::print()
{
    Uns i = cntnr.size()-1;
    for (vector<Uns>::reverse_iterator it = cntnr.rbegin() ; it != cntnr.rend(); ++it)
    {
      cout << *it << "*2**" << UnsSize*i;
      if(i!=0) cout << " + ";
      --i;
    }
    if (cntnr.size()==0) cout << "0";
    cout << endl;
}

bool operator <(const vectnum &lhs, const vectnum& rhs)
{
    Uns i = max(lhs.cntnr.size(),rhs.cntnr.size());
    while (i > 0)
    {
        --i;
        if (lhs.at(i)<rhs.at(i)) return true;
        if (lhs.at(i)>rhs.at(i)) return false;
    }
    return false;
}
