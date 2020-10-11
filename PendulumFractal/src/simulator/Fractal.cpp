
#include "Fractal.h"
#include "GpuInterface.h"
#include "FractalSection.h"

#include <math.h>

bool Fractal::gpuInterfaceInited = false;

Fractal::Fractal(int resolution, int shaderType, FractalData::InitialCondition ic) {
	if (!gpuInterfaceInited) {
		GpuInterface::init();
		gpuInterfaceInited = true;
	}

	this->resolution = resolution;
	this->shaderType = shaderType;
	this->ic = ic;

	this->shaderSectionCount = ceil((float)resolution / (float)FractalData::MAX_FRACTAL_SECTION_SIDE);
	this->lastShaderSectionSize = resolution % FractalData::MAX_FRACTAL_SECTION_SIDE;
}

unsigned long long Fractal::countBoxes(char boxType) {
	return 0;
}
