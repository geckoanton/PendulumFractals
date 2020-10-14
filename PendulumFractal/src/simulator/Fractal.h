
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

	unsigned long long countBoxes(unsigned char boxType);


private:
	int resolution, shaderType;
	int shaderSectionCount, lastShaderSectionSize;
	FractalData::InitialCondition ic;
};
