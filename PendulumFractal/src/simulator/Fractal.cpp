
#include "Fractal.h"
#include "GpuInterface.h"

bool Fractal::gpuInterfaceInited = false;

Fractal::Fractal(int resolution, int shaderType, FractalData::InitialCondition ic) {
	if (!gpuInterfaceInited) {
		GpuInterface::init();
		gpuInterfaceInited = true;
	}


}
