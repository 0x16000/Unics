#ifndef _MATH_H
#define _MATH_H 1

#include <sys/cdefs.h>

__BEGIN_DECLS

/* Constants */
#define M_E         2.7182818284590452354   /* e */
#define M_LOG2E     1.4426950408889634074   /* log_2(e) */
#define M_LOG10E    0.43429448190325182765  /* log_10(e) */
#define M_LN2       0.69314718055994530942  /* log_e(2) */
#define M_LN10      2.30258509299404568402  /* log_e(10) */
#define M_PI        3.14159265358979323846  /* pi */
#define M_PI_2      1.57079632679489661923  /* pi/2 */
#define M_PI_4      0.78539816339744830962  /* pi/4 */
#define M_1_PI      0.31830988618379067154  /* 1/pi */
#define M_2_PI      0.63661977236758134308  /* 2/pi */
#define M_2_SQRTPI  1.12837916709551257390  /* 2/sqrt(pi) */
#define M_SQRT2     1.41421356237309504880  /* sqrt(2) */
#define M_SQRT1_2   0.70710678118654752440  /* 1/sqrt(2) */

/* Infinity and NaN */
#if __GNUC_PREREQ(3,3)
    #define INFINITY __builtin_inf()
    #define NAN __builtin_nan("")
#else
    #define INFINITY (1.0/0.0)
    #define NAN (0.0/0.0)
#endif

/* Classification macros */
#define FP_INFINITE  1
#define FP_NAN      2
#define FP_NORMAL   3
#define FP_SUBNORMAL 4
#define FP_ZERO     5

/* Basic math functions */
__pure double fabs(double x) __THROW;
__pure double ceil(double x) __THROW;
__pure double floor(double x) __THROW;
__pure double fmod(double x, double y) __THROW;
__pure double sqrt(double x) __THROW;
__pure double cbrt(double x) __THROW;

/* Exponential and logarithmic functions */
__pure double exp(double x) __THROW;
__pure double exp2(double x) __THROW;
__pure double log(double x) __THROW;
__pure double log2(double x) __THROW;
__pure double log10(double x) __THROW;
__pure double pow(double x, double y) __THROW;

/* Trigonometric functions */
__pure double sin(double x) __THROW;
__pure double cos(double x) __THROW;
__pure double tan(double x) __THROW;
__pure double asin(double x) __THROW;
__pure double acos(double x) __THROW;
__pure double atan(double x) __THROW;
__pure double atan2(double y, double x) __THROW;

/* Hyperbolic functions */
__pure double sinh(double x) __THROW;
__pure double cosh(double x) __THROW;
__pure double tanh(double x) __THROW;
__pure double asinh(double x) __THROW;
__pure double acosh(double x) __THROW;
__pure double atanh(double x) __THROW;

/* Additional utility functions */
__pure double round(double x) __THROW;
__pure double trunc(double x) __THROW;
__pure double remainder(double x, double y) __THROW;
__pure double fdim(double x, double y) __THROW;
__pure double fmax(double x, double y) __THROW;
__pure double fmin(double x, double y) __THROW;

/* Classification functions */
__pure int isfinite(double x) __THROW;
__pure int isinf(double x) __THROW;
__pure int isnan(double x) __THROW;
__pure int isnormal(double x) __THROW;
__pure int signbit(double x) __THROW;

/* Float versions of core functions */
__pure float fabsf(float x) __THROW;
__pure float sqrtf(float x) __THROW;
__pure float expf(float x) __THROW;
__pure float logf(float x) __THROW;
__pure float powf(float x, float y) __THROW;
__pure float sinf(float x) __THROW;
__pure float cosf(float x) __THROW;
__pure float tanf(float x) __THROW;

/* Long double versions of core functions */
__pure long double fabsl(long double x) __THROW;
__pure long double sqrtl(long double x) __THROW;
__pure long double expl(long double x) __THROW;
__pure long double logl(long double x) __THROW;
__pure long double powl(long double x, long double y) __THROW;
__pure long double sinl(long double x) __THROW;
__pure long double cosl(long double x) __THROW;
__pure long double tanl(long double x) __THROW;

__END_DECLS

#endif /* _MATH_H */