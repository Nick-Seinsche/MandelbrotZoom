#pragma once
#include <stdio.h>
#include <math.h>

/*
    OVERVEW:

    ==========================================================================

    ~~ TYPES ~~

    typedef         cpx_VALUE

    typedef struct  complex                 {real: cpx_VALUE, imag: cpx_VALUE} 

    ~~ GLOBAL VARIABLES ~~

    char            cpx_STRING[25]

    ~~ FUNCTIONS ~~

    int             cpx_eq                  (complex a, complex b);

    double          cpx_abs                 (complex a);

    complex         cpx_add                 (complex a, complex b);

    complex         cpx_sub                 (complex a, complex b);

    complex         cpx_mul                 (complex a, complex b);

    complex         cpx_inv                 (complex a);

    complex         cpx_pow                 (complex a, short n);

    complex         cpx_div                 (complex a, complex b);

    complex         cpx_conj                (complex a);

    complex         cpx_polar               (complex a);

    complex         cpx_cart                (complex a);

    void            cpx_cpy                 (complex* a, complex b);

    char*           cpx_str                 (complex a);

    ==========================================================================

    Determines the datatype of real and imag part of complex type.
    If defined in your project the complex datatype will use the 
    given type to store the real and imaginary part of complex type 
    else float will be used as default.

    sizeof(complex) = 8 for float
    sizeof(complex) = 16 for double
    sizeof(complex) = 24 for long double

    #define CPX_USE_FLOAT
    #define CPX_USE_DOUBLE
    #define CPX_USE_LONG_DOUBLE
    #define CPX_NO_DEFINE

    Use CPX_NO_VALUE_DEFINE if you want to define cpx_VALUE to a custom type.
*/
#ifndef CPX_NO_VALUE_DEFINE
    #if defined(CPX_USE_LONG_DOUBLE)
        typedef long double cpx_VALUE;
    #elif defined(CPX_USE_DOUBLE)
        typedef double cpx_VALUE;
    #else
        typedef float cpx_VALUE;
    #endif
#endif


/*
    complex struct.
    Usage: complex x = {REAL, IMAGINARY};
*/
typedef struct{
    cpx_VALUE real, imag;
} complex;


/*
    String of complex will be stored here. Use cpx_str to access.
*/
char cpx_STRING[25];


/*
    Returns 1 if complex a equals complex b else returns 0.
*/
int cpx_eq(complex a, complex b){
    if (a.real == b.real && a.imag == b.imag) return 1;
    return 0;
}


/*
    Returns the absolute value of complex a.
*/
cpx_VALUE cpx_abs(complex a){
    return sqrt(a.real * a.real + a.imag * a.imag);
}


/*
    Returns the sum of complex a and complex b.
*/
complex cpx_add(complex a, complex b){
    return (complex) {a.real + b.real, a.imag + b.imag};
}


/*
    Returns the difference of complex a and complex b.
*/
complex cpx_sub(complex a, complex b){
    return (complex) {a.real - b.real, a.imag - b.imag};
}


/*
    Returns the product of complex a and complex b.
*/
complex cpx_mul(complex a, complex b){
    return (complex) {a.real * b.real - a.imag * b.imag,
                      a.real * b.imag + a.imag * b.real};
}


/*
    Returns the inverse of complex a.
*/
complex cpx_inv(complex a){
    return (complex) {a.real / (a.real * a.real + a.imag * a.imag),
                    - a.imag / (a.real * a.real + a.imag * a.imag)};
}


/*
    Raises complex a to the n-th power. Positive and negative integers are
    allowed.
*/
complex cpx_pow(complex a, short n){
    if      (n == 0) return (complex) {1, 0};
    else if (n <  0) return cpx_pow(cpx_inv(a), -n);
    else if (n == 1) return a;
    else             return cpx_mul(cpx_pow(a, n - 1), a);
}


/*
    Returns the quotient of complex a and complex b.
*/
complex cpx_div(complex a, complex b){
    return (complex) cpx_mul(a, cpx_inv(b));
}


/*
    Returns the complex conjugated of complex a.
*/
complex cpx_conj(complex a){
    return (complex) {a.real, -a.imag};
}


/*
    Returns the polar form of complex a.
*/
complex cpx_polar(complex a){
    return (complex) {cpx_abs(a), atan(a.imag / a.real)};
}


/*
    Returns the karthesian form of complex a.
*/
complex cpx_cart(complex a){
    return (complex) {a.real * cos(a.imag), a.real * sin(a.imag)};
}


/*
    Copies value of complex b to complex a address.
*/
void cpx_cpy(complex* a, complex b){
    a -> real = b.real;
    a -> imag = b.imag;
}


/*
    Returns a string that represents given complex a in the form of a+ib,
    where a,b are floats.    
*/
char* cpx_str(complex a){
    sprintf(cpx_STRING, "%f+%fi", (float) a.real, (float) a.imag);
    return cpx_STRING;
}
