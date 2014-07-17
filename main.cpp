#include <iostream>
#include <bitset>
#include <chrono>
#include <random>
#include "vectnum.h"

const vectnum unit(1);
const vectnum zero;
const vectnum two(2);

bool MRTest(const vectnum& n, const Uns& k) //Miller-Rabin primality test; returns false if n is composite; returns true if n is probably prime (passes k random trials).
{
    if ((n.cntnr.empty()) || (n == unit)) return false; // return false if n is 0 or 1
    Uns t = 0;
    vectnum n_minus_1(n-unit);
    vectnum m(n_minus_1);
    while ((m % 2) == 0)
    {
        ++t;
        m.smallRightShift(1);
    }
    if (t == 0) return false; //n was even
    cout << "t: " << t << ", m: "; m.print();
//    cout << "n-1: "; n_minus_1.print();
//    cout << "check: "; ((two^t)*m).print();
    if (n_minus_1 == ((two^t)*m)) cout << "yay" << endl;
    for (Uns w = 0; w < k; ++w)
    {
        vectnum a = vectnum::random(n_minus_1);
        cout << "a: "; a.print();
        vectnum x = power(a, m, n);
        cout << "x: "; x.print();
        if ((x == unit) || (x == n_minus_1))
        {
            cout << "n is pseudoprime using base: "; a.print();
            continue;
        }
        for (Uns i = 0; i < t; ++i)
        {
            if (i == (t - 1))
            {
                cout << "tried it all" << endl;
                return false;
            }
            x.smallPow(2, n);
            cout << "testing with exponent: " << i+1 << " and x: "; x.print();
            if (x == 1)
            {
                cout << "shortcut" << endl;
                return false;
            }
            if (x == n_minus_1)
            {
                cout << "n is pseudoprime using base: "; a.print();
                break;
            }
        }
    }
    return true;
}

int main()
{
    vectnum tested = (two^3217)-1; //Mersenne prime M_3217; takes a while; don't run in debug.
    cout << "tested number: "; tested.print();
    if(MRTest(tested, 5))
        cout << "Probably prime." << endl;
    else
        cout << "Composite." << endl;
    return 0;
}
