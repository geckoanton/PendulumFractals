#pragma once

#include "precisionType.h"

class Vec4;

namespace FractalData {
	struct InitialCondition;
};

class Simulator {
public:
	// TODO change to Fractal::InitialCondition
	Simulator(int valueType, Vec4 state, FractalData::InitialCondition* ic);

	unsigned char generateValue();
    void iterateSnapshot(PREDEC dt);

	Vec4 getState();

	float getx1();
	float gety1();

	float getx2();
	float gety2();

private:
	int valueType;
	Vec4* state;
    FractalData::InitialCondition* ic;

	Vec4 getStateChange(Vec4 state);

	PREDEC A1();
	PREDEC B1(PREDEC a, PREDEC b);
	PREDEC C1(PREDEC a, PREDEC b, PREDEC da, PREDEC db);

	PREDEC A2(PREDEC a, PREDEC b);
	PREDEC B2();
	PREDEC C2(PREDEC a, PREDEC b, PREDEC da, PREDEC db);

	// valueTypes
	unsigned char getFlipFractalValue();

};