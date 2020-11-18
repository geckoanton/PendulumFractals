
#include "../simulator/GpuInterface.h"
#include "../simulator/FractalSection.h"
#include "../simulator/Fractal.h"

#include <iostream>
#include <Windows.h>
#include <fstream>
#include <math.h>
#include <string>
#include <algorithm>

template<class T>
std::string replaceDots(T input) { 
	std::string return_string = std::to_string(input);
	std::replace(return_string.begin(), return_string.end(), '.', ',');
	return return_string;
}


int main() {
	float float_number = 10.11;
	std::cout << replaceDots(float_number) << std::endl;

	GpuInterface::init();

	FractalData::Section s = { 0.0, 0.0, 1.0, 1.0 };
	FractalData::InitialCondition ic = { 1.0, 1.0, 1.0, 1.0, 9.82 };

	std::ofstream myfile;
	myfile.open("example.txt");

	Fractal box_count_f = Fractal(4096, FractalData::flipFractal, ic);
	box_count_f.temp_char_arr = new unsigned char[4096 * 4096];
	for (int i = 0; i < 4096 * 4096; ++i) {
		box_count_f.temp_char_arr[i] = 0;
	}
	for (int x = 10; x < 64; x++) {
		for (int y = 0; y < 64; y++) {
			box_count_f.temp_char_arr[x + y * 4096] = 1;
		}
	}
	auto crossings = box_count_f.getCompassBorderCrossing();
	auto border_info = box_count_f.getBorderArea(0, 0, crossings);
	delete(box_count_f.temp_char_arr);

	/*for (int k = 1; k <= 16777216; k *= 2) {
		Fractal f(k, FractalData::flipFractal, ic);

		double delta = 1.0f / k;

		std::cout << std::endl << std::endl << -log10(delta) << "\t" << log10((double)f.countEdges(0)) << std::endl << std::endl;
		myfile << -log10(delta) << "\t" << log10((double)f.countEdges(0)) << std::endl;
	}*/

	return 0;
}
