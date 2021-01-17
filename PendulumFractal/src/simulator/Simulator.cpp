#include <iostream>
#include <math.h>
#include <algorithm>

#include "Simulator.h"
#include "Vec4.h"
#include "Fractal.h"
#include "precisionType.h"

Simulator::Simulator(int valueType, Vec4 state, FractalData::InitialCondition* ic) {
	this->valueType = valueType;

	this->state = new Vec4(state.getx(), state.gety(), state.getz(), state.getw()); // causes memoryleak

	this->ic = ic;
}

unsigned char Simulator::generateValue() {
	switch(valueType) {
		case FractalData::flipFractal:
			return getFlipFractalValue();
		default:
			return 0;
	}
}

void Simulator::iterateSnapshot(PREDEC dt) {
	Vec4 k1 = getStateChange(*this->state);
	Vec4 k2 = getStateChange(this->state->add(k1.mul(dt / 2)));
	Vec4 k3 = getStateChange(this->state->add(k2.mul(dt / 2)));
	Vec4 k4 = getStateChange(this->state->add(k3.mul(dt)));

	Vec4 stateChange = k1.add(k2.mul(2).add(k3.mul(2).add(k4))).mul(dt / 6);

	PREDEC a = PREfmod(state->getx() + stateChange.getx(), 6.28318530718);
	PREDEC b = PREfmod(state->gety() + stateChange.gety(), 6.28318530718);
	PREDEC da = state->getz() + stateChange.getz();
	PREDEC db = state->getw() + stateChange.getw();

	this->state->update(a, b, da, db);
}

Vec4 Simulator::getState() {
	return Vec4(state->getx(), state->gety(), state->getz(), state->getw());
}

float Simulator::getx1() {
	return -((float)ic->l1 * std::sin((float)state->getx()));
}
float Simulator::gety1() {
	return -((float)ic->l1 * std::cos((float)state->getx()));
}

float Simulator::getx2() {
	return -((float)ic->l1 * std::sin((float)state->getx())) - ((float)ic->l2 * std::sin((float)state->gety()));
}
float Simulator::gety2() {
	return -((float)ic->l1 * std::cos((float)state->getx())) - ((float)ic->l2 * std::cos((float)state->gety()));
}

Vec4 Simulator::getStateChange(Vec4 state) {
	PREDEC a1 = A1();
	PREDEC b1 = B1(state.getx(), state.gety());
	PREDEC c1 = C1(state.getx(), state.gety(), state.getz(), state.getw());

	PREDEC a2 = A2(state.getx(), state.gety());
	PREDEC b2 = B2();
	PREDEC c2 = C2(state.getx(), state.gety(), state.getz(), state.getw());

	PREDEC ddb = (a2 * c1 - a1 * c2) / (a2 * b1 - a1 * b2);
	PREDEC dda = (c1 - b1 * ddb) / a1;

	return Vec4(state.getz(), state.getw(), dda, ddb);
}

PREDEC Simulator::A1() {
	return (ic->m1 + ic->m2) * ic->l1;
}
PREDEC Simulator::B1(PREDEC a, PREDEC b) {
	return ic->m2 * ic->l2 * PREcos(a - b);
}
PREDEC Simulator::C1(PREDEC a, PREDEC b, PREDEC da, PREDEC db) {
	return -(db * db * ic->l2 * ic->m2 * PREsin(a - b)) - (ic->g * (ic->m1 + ic->m2)) * PREsin(a);
}

PREDEC Simulator::A2(PREDEC a, PREDEC b) {
	return ic->m2 * ic->l1 * PREcos(a - b);
}
PREDEC Simulator::B2() {
	return ic->m2 * ic->l2;
}
PREDEC Simulator::C2(PREDEC a, PREDEC b, PREDEC da, PREDEC db) {
	return (da * da * ic->l1 * ic->m2 * PREsin(a - b)) - (ic->g * ic->m2) * PREsin(b);
}

unsigned char Simulator::getFlipFractalValue() {
	const PREDEC PI = 3.1415926535897932384626433832795028841971693993751058209749445923078164062;

	iterateSnapshot(0.01f);

	for(int i = 0; i < iteration_count; i++) {
		PREDEC prevB = state->gety();
		iterateSnapshot(time_step);
		PREDEC currentB = state->gety();

		if ((currentB < PI && prevB > PI && currentB < 5 && prevB < 5) || (currentB > PI && prevB < PI && currentB < 5 && prevB < 5)) {
			return 255;
		}
	}

	return 0;
}
