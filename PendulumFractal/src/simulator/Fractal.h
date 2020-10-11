
#pragma once

namespace FractalData {
	enum Type {
		flipFractal,
		size
	};

	struct Section {
		float cornerX, cornerY;
		float spanX, spanY;
	};
	struct InitialCondition {
		float m1, m2;
		float l1, l2;
		float g;
	};
};

class Fractal {
public:
	Fractal(int resolution, int shaderType, FractalData::InitialCondition ic);

	unsigned long long countBoxes(char boxType);

private:
	static bool gpuInterfaceInited;
	int resolution, shaderType;
	int shaderSectionCount, lastShaderSectionSize;
	FractalData::InitialCondition ic;
};
