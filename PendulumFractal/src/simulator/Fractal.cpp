
#include "Fractal.h"
#include "GpuInterface.h"
#include "FractalSection.h"

#include <math.h>
#include <iostream>

Fractal::Fractal(int resolution, int shaderType, FractalData::InitialCondition ic) {
	this->resolution = resolution;
	this->shaderType = shaderType;
	this->ic = ic;

	this->shaderSectionCount = ceil((float)resolution / (float)FractalData::MAX_FRACTAL_SECTION_SIDE);
	this->lastShaderSectionSize = resolution % FractalData::MAX_FRACTAL_SECTION_SIDE;
}

unsigned long long Fractal::countBoxes(unsigned char boxType) {
	unsigned long long result = 0;

	for (int y = 0; y < shaderSectionCount; y++) {
		for (int x = 0; x < shaderSectionCount; x++) {

			int width = FractalData::MAX_FRACTAL_SECTION_SIDE;
			float sectionWidth = (float) FractalData::MAX_FRACTAL_SECTION_SIDE / (float) resolution;
			if (x == shaderSectionCount - 1 && lastShaderSectionSize != 0) {
				width = lastShaderSectionSize;
				sectionWidth = (float) lastShaderSectionSize / (float) resolution;
			}

			int height = FractalData::MAX_FRACTAL_SECTION_SIDE;
			float sectionHeight = (float) FractalData::MAX_FRACTAL_SECTION_SIDE / (float) resolution;
			if (y == shaderSectionCount - 1 && lastShaderSectionSize != 0) {
				height = lastShaderSectionSize;
				sectionHeight = (float) lastShaderSectionSize / (float) resolution;
			}


			FractalData::Section s = {((float) x * (float) FractalData::MAX_FRACTAL_SECTION_SIDE) / (float) resolution, ((float) y * (float) FractalData::MAX_FRACTAL_SECTION_SIDE) / (float) resolution, sectionWidth, sectionHeight};

			FractalSection fs(GpuInterface::shaders[shaderType], width, height, &s, &ic);

			for (int i = 0; i < fs.getSize(); i++) {
				if(fs.getBuffer()[i] == boxType)
					result++;
			}

			/*for (int i = 0; i < fs.getHeight(); i++) {
				for (int j = 0; j < fs.getWidth(); j++)
					(fs.getBuffer()[fs.getWidth() * i + j] == 0) ? (std::cout << "_") : (std::cout << "#");
				std::cout << std::endl;
			}

			std::cout << std::endl << std::endl << std::endl;*/
		}
	}

	return result;
}
