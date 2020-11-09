
#pragma once

class FractalSection;

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
	unsigned long long countEdges(unsigned char boxType);


private:
	void loadCurrentSection(int xIn, int yIn, unsigned char* generate);
	unsigned char getCharArr(int x, int y);

	void shiftCurretSections(int shiftX, int shiftY);

	const int CURRENT_SECTIONS_SIDE = 5;
	FractalSection** currentSections;

	int resolution, resolutionSectionCount, shaderType;
	int currentCornerX = 0, currentCornerY = 0;
	FractalData::InitialCondition ic;
};
