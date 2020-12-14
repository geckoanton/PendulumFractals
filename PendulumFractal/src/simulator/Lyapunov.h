#pragma once

#include "precisionType.h"

class Vec4;

namespace FractalData {
	struct InitialCondition;
};

class Lyapunov {
public:
	PREDEC generate(Vec4 refStart, Vec4 deltaError, PREDEC D, int N, PREDEC dt, FractalData::InitialCondition ic);

private:
	/*FractalData::InitialCondition* ic;
	Vec4* refStart, deltaError;
	PREDEC D, N, dt;*/

};
