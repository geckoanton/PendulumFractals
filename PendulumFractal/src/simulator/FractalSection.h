
#pragma once

class Shader;

namespace FractalData {
	struct Section;
	struct InitialCondition;

	const int MAX_FRACTAL_SECTION_SIDE = 256;
};

class FractalSection {
public:
	FractalSection(Shader* shader, int width, int height, FractalData::Section* s, FractalData::InitialCondition* ic);
	~FractalSection();

	char* get();
	int size();

private:
	Shader* shader;
	unsigned int FBO, texture;
	int width, height;
	char* dataArray;

	void generate(FractalData::Section* s, FractalData::InitialCondition* ic);
	void createDataArray();
};
