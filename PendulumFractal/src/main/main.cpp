
#include "../simulator/GpuInterface.h"
#include "../simulator/FractalSection.h"
#include "../simulator/Fractal.h"

#include <iostream>
#include <Windows.h>
#include <fstream>
#include <math.h>

int main() {
	GpuInterface::init();

	FractalData::Section s = { 0.0, 0.0, 1.0, 1.0 };
	FractalData::InitialCondition ic = { 1.0, 1.0, 1.0, 1.0, 9.82 };

	std::ofstream myfile;
	myfile.open("example.txt");

	for (int k = 1; k <= 16777216; k *= 2) {
		Fractal f(k, FractalData::flipFractal, ic);

		double delta = 1.0f / k;

		std::cout << std::endl << std::endl << -log10(delta) << "\t" << log10((double)f.countEdges(0)) << std::endl << std::endl;
		myfile << -log10(delta) << "\t" << log10((double)f.countEdges(0)) << std::endl;
	}

	return 0;
}
