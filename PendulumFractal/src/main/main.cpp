
#include "../simulator/openGL/GpuInterface.h"
#include "../simulator/openGL/FractalSection.h"
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


	/*float float_number = 10.11;
	std::cout << replaceDots(float_number) << std::endl;

	GpuInterface::init();

	FractalData::Section s = { 0.0, 0.0, 1.0, 1.0 };
	FractalData::InitialCondition ic = { 1.0, 1.0, 1.0, 1.0, 9.82 };

	std::ofstream myfile;
	myfile.open("example.txt");

	Fractal box_count_f = Fractal(1024, FractalData::flipFractal, ic);*/

	//box_count_f.countBoxes(0);

	/*box_count_f.temp_char_arr = new unsigned char[4096 * 4096];
	for (int i = 0; i < 4096 * 4096; ++i) {
		box_count_f.temp_char_arr[i] = 0;
	}
	for (int x = 10; x < 64; x++) {
		for (int y = 0; y < 64; y++) {
			box_count_f.temp_char_arr[x + y * 4096] = 1;
		}
	}*/

	//Fractal f(8192, FractalData::flipFractal, ic);

	/*auto crossings = box_count_f.getCompassBorderCrossing();
	auto border_info = box_count_f.getBorderArea(0, 0, crossings);*/
	/*BorderArea border;
	for (int y = 0; y < 8192; y += 4096) {
		for (int x = 0; x < 8192; x += 4096) {
			auto border_info = box_count_f.getBorderArea(x, y, crossings);

			for (int i = 0; i < border.box_dimension_count; i++) {
				border.boxSize[i] += border_info.boxSize[i];
			}
		}
	}*/
	/*std::cout << "start" << std::endl;
	for (int i = 0; i < border_info.box_dimension_count; i++) {
		std::cout << border_info.boxSize[i] << std::endl;
	}
	std::cout << "end" << std::endl;*/
	//delete(box_count_f.temp_char_arr);

	/*for (int k = 0; k < border_info.box_dimension_count; k++) {
		double delta = pow(2, k) / 1024;

		std::cout << -log10(delta) << "\t" << log10((double)border_info.boxSize[k]) << std::endl;;
		myfile << -log10(delta) << "\t" << log10((double)border_info.boxSize[k]) << std::endl;
	}

	myfile.close();*/

	return 0;
}
