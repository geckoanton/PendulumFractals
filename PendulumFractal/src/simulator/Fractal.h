
#pragma once

namespace FractalData {
	enum Type {
		flipFractal,
		size
	};

	struct Section {
		double cornerX, cornerY;
		double spanX, spanY;
	};
	struct InitialCondition {
		double m1, m2;
		double l1, l2;
		double g;
	};
};

class Fractal {
public:
	Fractal(int resolution, int shaderType, FractalData::InitialCondition ic);

private:
	const int MAX_FRACTAL_SECTION_SIDE = 256;

	static bool gpuInterfaceInited;
};
