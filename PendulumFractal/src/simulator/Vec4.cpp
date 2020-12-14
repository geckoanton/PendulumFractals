#include "Vec4.h"
#include "precisionType.h"

Vec4::Vec4(PREDEC x, PREDEC y, PREDEC z, PREDEC w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

void Vec4::update(PREDEC x, PREDEC y, PREDEC z, PREDEC w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Vec4 Vec4::add(Vec4 vec) {
	return Vec4(x + vec.getx(), y + vec.gety(), z + vec.getz(), w + vec.getw());
}
Vec4 Vec4::sub(Vec4 vec) {
	return Vec4(x - vec.getx(), y - vec.gety(), z - vec.getz(), w - vec.getw());
}
Vec4 Vec4::mul(PREDEC m) {
	return Vec4(x * m, y * m, z * m, w * m);
}
Vec4 Vec4::div(PREDEC d) {
	return Vec4(x / d, y / d, z / d, w / d);
}

PREDEC Vec4::abs() {
	return PREsqrt(x*x + y*y + z*z + w*w);
}

PREDEC Vec4::getx() {
	return x;
}
PREDEC Vec4::gety() {
	return y;
}
PREDEC Vec4::getz() {
	return z;
}
PREDEC Vec4::getw() {
	return w;
}
