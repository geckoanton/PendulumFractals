#pragma once

/*#include <boost/multiprecision/cpp_dec_float.hpp>

#define PREDEC boost::multiprecision::cpp_dec_float_100
#define PREsin boost::multiprecision::sin
#define PREcos boost::multiprecision::cos
#define PREfmod boost::multiprecision::fmod*/

#include <iostream>
#include <math.h>

inline double pMod(double num, double mod) {
	return fmod(fmod(num, mod) + mod, mod);
}

#define PREDEC double
#define PREsin std::sin
#define PREcos std::cos
//#define PREfmod std::fmod
#define PREfmod pMod
#define PREsqrt std::sqrt
#define PRElog std::log
