
#include "Fractal.h"
#include "GpuInterface.h"
#include "FractalSection.h"

#include <math.h>
#include <iostream>

Fractal::Fractal(int resolution, int shaderType, FractalData::InitialCondition ic) {
	this->currentSections = new FractalSection * [CURRENT_SECTIONS_SIDE * CURRENT_SECTIONS_SIDE];
	for (int i = 0; i < CURRENT_SECTIONS_SIDE * CURRENT_SECTIONS_SIDE; i++)
		this->currentSections[i] = nullptr;

	this->resolution = resolution;
	this->resolutionSectionCount = (int)ceil((float)resolution / (float)FractalData::MAX_FRACTAL_SECTION_SIDE);
	this->shaderType = shaderType;
	this->ic = ic;
}

void Fractal::loadCurrentSection(int xIn, int yIn, unsigned char* generate) {

	int cornerX = xIn / FractalData::MAX_FRACTAL_SECTION_SIDE;
	int cornerY = yIn / FractalData::MAX_FRACTAL_SECTION_SIDE;
	if (xIn < 0 && xIn % FractalData::MAX_FRACTAL_SECTION_SIDE != 0)
		cornerX--;
	if (yIn < 0 && yIn % FractalData::MAX_FRACTAL_SECTION_SIDE != 0)
		cornerY--;

	cornerX *= FractalData::MAX_FRACTAL_SECTION_SIDE;
	cornerY *= FractalData::MAX_FRACTAL_SECTION_SIDE;

	if (cornerX != currentCornerX || cornerY != currentCornerY) {
		shiftCurretSections((currentCornerX - cornerX) / FractalData::MAX_FRACTAL_SECTION_SIDE, (currentCornerY - cornerY) / FractalData::MAX_FRACTAL_SECTION_SIDE);
	}

	this->currentCornerX = cornerX;
	this->currentCornerY = cornerY;

	for (int y = -(CURRENT_SECTIONS_SIDE / 2); y <= (CURRENT_SECTIONS_SIDE / 2); y++) {
		for (int x = -(CURRENT_SECTIONS_SIDE / 2); x <= (CURRENT_SECTIONS_SIDE / 2); x++) {
			if (generate[(y + (CURRENT_SECTIONS_SIDE / 2)) * CURRENT_SECTIONS_SIDE + (x + (CURRENT_SECTIONS_SIDE / 2))] == 0)
				continue;

			FractalData::Section s = { ((float)x * (float)FractalData::MAX_FRACTAL_SECTION_SIDE + cornerX) / (float)resolution,
				((float)y * (float)FractalData::MAX_FRACTAL_SECTION_SIDE + cornerY) / (float)resolution,
				(float)FractalData::MAX_FRACTAL_SECTION_SIDE / (float)resolution, (float)FractalData::MAX_FRACTAL_SECTION_SIDE / (float)resolution };

			currentSections[(y + (CURRENT_SECTIONS_SIDE / 2)) * CURRENT_SECTIONS_SIDE + (x + (CURRENT_SECTIONS_SIDE / 2))] = new FractalSection(GpuInterface::shaders[shaderType],
				FractalData::MAX_FRACTAL_SECTION_SIDE, FractalData::MAX_FRACTAL_SECTION_SIDE,
				&s, &ic);
		}
	}
}
unsigned char Fractal::getCharArr(int x, int y) {
	int computeX = (x - currentCornerX) + FractalData::MAX_FRACTAL_SECTION_SIDE;
	int computeY = (y - currentCornerY) + FractalData::MAX_FRACTAL_SECTION_SIDE;

	if (computeX >= CURRENT_SECTIONS_SIDE * FractalData::MAX_FRACTAL_SECTION_SIDE ||
		computeY >= CURRENT_SECTIONS_SIDE * FractalData::MAX_FRACTAL_SECTION_SIDE ||
		computeX < 0 || computeY < 0) {
		unsigned char* generate = new unsigned char[CURRENT_SECTIONS_SIDE * CURRENT_SECTIONS_SIDE];
		for (int i = 0; i < CURRENT_SECTIONS_SIDE * CURRENT_SECTIONS_SIDE; i++)
			generate[i] = 0;
		generate[CURRENT_SECTIONS_SIDE * (CURRENT_SECTIONS_SIDE / 2) + (CURRENT_SECTIONS_SIDE / 2 + 1)] = 1;

		loadCurrentSection(x, y, generate);

		std::cout << x << " | " << y << std::endl;

		computeX = (x - currentCornerX) + FractalData::MAX_FRACTAL_SECTION_SIDE;
		computeY = (y - currentCornerY) + FractalData::MAX_FRACTAL_SECTION_SIDE;
	}

	int cellX = computeX % FractalData::MAX_FRACTAL_SECTION_SIDE;
	int cellY = computeY % FractalData::MAX_FRACTAL_SECTION_SIDE;

	int sectionX = computeX / FractalData::MAX_FRACTAL_SECTION_SIDE;
	int sectionY = computeY / FractalData::MAX_FRACTAL_SECTION_SIDE;

	if (currentSections[sectionY * CURRENT_SECTIONS_SIDE + sectionX] == nullptr) {
		unsigned char* generate = new unsigned char[CURRENT_SECTIONS_SIDE * CURRENT_SECTIONS_SIDE];
		for (int i = 0; i < CURRENT_SECTIONS_SIDE * CURRENT_SECTIONS_SIDE; i++)
			generate[i] = 0;
		generate[sectionY * CURRENT_SECTIONS_SIDE + sectionX] = 1;
		loadCurrentSection(this->currentCornerX, this->currentCornerY, generate);
	}

	return currentSections[sectionY * CURRENT_SECTIONS_SIDE + sectionX]->getBuffer()[cellY * FractalData::MAX_FRACTAL_SECTION_SIDE + cellX];
}

void Fractal::shiftCurretSections(int shiftX, int shiftY) {
	FractalSection** newSections = new FractalSection * [CURRENT_SECTIONS_SIDE * CURRENT_SECTIONS_SIDE];

	for (int y = 0; y < CURRENT_SECTIONS_SIDE; y++) {
		for (int x = 0; x < CURRENT_SECTIONS_SIDE; x++) {
			int getX = x - shiftX;
			int getY = y - shiftY;


			if (getX < 0 || getX >= CURRENT_SECTIONS_SIDE || getY < 0 || getY >= CURRENT_SECTIONS_SIDE) {
				delete currentSections[(CURRENT_SECTIONS_SIDE - 1 - y) * CURRENT_SECTIONS_SIDE + (CURRENT_SECTIONS_SIDE - 1 - x)];
				newSections[y * CURRENT_SECTIONS_SIDE + x] = nullptr;
			}
			else {
				newSections[y * CURRENT_SECTIONS_SIDE + x] = currentSections[getY * CURRENT_SECTIONS_SIDE + getX];
			}
		}
	}

	delete currentSections;
	currentSections = newSections;
}

unsigned long long Fractal::countBoxes(unsigned char boxType) {
	for (int a = 0; a < FractalData::MAX_FRACTAL_SECTION_SIDE * 3; a++) {
		for (int b = 0; b < FractalData::MAX_FRACTAL_SECTION_SIDE * 3; b++) {
			if (getCharArr(b - FractalData::MAX_FRACTAL_SECTION_SIDE, a - FractalData::MAX_FRACTAL_SECTION_SIDE) == 0)
				std::cout << "-";
			else
				std::cout << "#";
		}
		std::cout << std::endl;
	}
	return 0;
}
unsigned long long Fractal::countEdges(unsigned char boxType) {
	long long edgeBoxes = 0;


	std::cout << resolutionSectionCount << std::endl;

	for (int sectionY = 0; sectionY < resolutionSectionCount; sectionY++) {
		for (int sectionX = 0; sectionX < resolutionSectionCount; sectionX++) {

			//std::cout << "generating: " << sectionX << " | " << sectionY << std::endl;
			//std::cout << "sections: " << sectionX << " | " << sectionY << std::endl;

			for (int inY = 0; inY < FractalData::MAX_FRACTAL_SECTION_SIDE; inY++) {
				for (int inX = 0; inX < FractalData::MAX_FRACTAL_SECTION_SIDE; inX++) {
					int x = (sectionX * FractalData::MAX_FRACTAL_SECTION_SIDE) + inX;
					int y = (sectionY * FractalData::MAX_FRACTAL_SECTION_SIDE) + inY;

					if (y >= resolution)
						goto CONTINUE_LARGE_LOOP;
					if (x >= resolution)
						break;

					//getCharArr(x, y);

					for (int smallY = -1; smallY < 2; smallY++) {
						for (int smallX = -1; smallX < 2; smallX++) {
							if (smallX == 0 && smallY == 0)
								continue;

							if (getCharArr(x + smallX, y + smallY) != boxType) {
								if (getCharArr(x, y) == boxType) {
									edgeBoxes++;
									goto BOX_COUNTED;
								}
							}
						}
					}
				BOX_COUNTED:
					inX = inX * 1;
					//std::cout << "box counted" << std::endl;

				}
			}

			//std::cout << "outside generation" << std::endl;

		CONTINUE_LARGE_LOOP:
			sectionX = sectionX * 1;
			//std::cout << "continue large loop" << std::endl;
		}
	}


	return edgeBoxes;
}
