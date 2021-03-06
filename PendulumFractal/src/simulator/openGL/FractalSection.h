
#pragma once

class Shader;

namespace FractalData {
	struct Section;
	struct InitialCondition;

	const int MAX_FRACTAL_SECTION_SIDE = 1024;	// must be power of 2
};

class FractalSection {
public:
	FractalSection(Shader* shader, int width, int height, FractalData::Section* s, FractalData::InitialCondition* ic);
	~FractalSection();

	unsigned char* getBuffer();

	int getSize();
	int getWidth();
	int getHeight();

private:
	const double MAX_FBO_WAIT = 10.0;

	static bool gpuInterfaceInited;

	Shader* shader;
	unsigned int FBO, texture;
	int width, height;
	unsigned char* buffer;

	void generate(FractalData::Section* s, FractalData::InitialCondition* ic);
	void createDataArray();
};
