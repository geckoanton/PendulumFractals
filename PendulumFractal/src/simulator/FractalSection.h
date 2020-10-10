
#pragma once

class Shader;

namespace FractalData {
	struct Section;
	struct InitialCondition;
};

class FractalSection {
public:
	FractalSection(Shader* shader, int width, int height);

	void generateFractal(FractalData::Section* s, FractalData::InitialCondition* ic);
private:
	Shader* shader;
	unsigned int FBO, texture;
	int width, height;
};
