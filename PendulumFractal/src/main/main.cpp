
#include "../simulator/openGL/GpuInterface.h"
#include "../simulator/openGL/FractalSection.h"
#include "../simulator/Fractal.h"

#include <iostream>
#include <Windows.h>
#include <fstream>
#include <math.h>
#include <string>
#include <algorithm>
#include <chrono>

template<class T>
std::string replaceDots(T input) { 
	std::string return_string = std::to_string(input);
	std::replace(return_string.begin(), return_string.end(), '.', ',');
	return return_string;
}

//get number of milliseconds since 1970
inline long long int getTimeSinceEpoc() {
	return(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}

int main() {
	double var = 1.0/8;

	// Where you want the output(no argument means that it does not save to file)
	PrintOutput print_output("data.txt");

	while (var <= 8) {
		// Set up the conditions
		FractalData::InitialCondition ic;
		ic.m1 = 1.0;
		ic.m2 = 1.0;
		ic.l1 = 1.0;
		ic.l2 = 1.0;
		ic.g = 9.82;

		// Set up the fractal calculation
		Fractal box_count_fractal = Fractal(4096, FractalData::flipFractal, ic);
		box_count_fractal.iteration_count = 1000;
		box_count_fractal.time_step = 0.01;

		// Print some info about simulation
		print_output << "m1: " << ic.m1 << "  m2: " << ic.m2 << "  l1:" << ic.l1 << "  l2: " << ic.l2 << "  g: " << ic.g << "\n";
		print_output << "time_step: " << box_count_fractal.time_step << "s  simulated_time: " <<
			box_count_fractal.time_step * box_count_fractal.iteration_count << "s" << "\n";
		print_output << "total size(resolution): " << box_count_fractal.getResolution() << "\n";

		print_output.printOnScreen();

		// Record when simulation starts
		long long start_time = getTimeSinceEpoc();

		// Do the calculation and print results
		box_count_fractal.getCompassDimension(print_output);

		// Print how long the calculation took
		double calcualtion_time = getTimeSinceEpoc() - start_time;

		int hours = calcualtion_time / (60 * 60 * 1000);
		int minutes = (calcualtion_time - hours * (60 * 60 * 1000)) / (60 * 1000);
		int seconds = (calcualtion_time - hours * (60 * 60 * 1000) - minutes * (60 * 1000)) / 1000;
		int milli_seconds = (calcualtion_time - hours * (60 * 60 * 1000) - minutes * (60 * 1000) - seconds * (1000));

		print_output << "calculation time: " << calcualtion_time << "ms" << "\n";
		print_output << "( " << hours << "h " << minutes << "m " << seconds << "s " << milli_seconds << "ms) " << "\n";

		print_output << "\n\n\n";

		print_output.printOnScreen();

		// Double the var variable for next iteration
		var = var * 2;
	}
	
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
