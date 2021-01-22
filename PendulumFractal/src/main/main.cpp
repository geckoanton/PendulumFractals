
#include "../simulator/openGL/GpuInterface.h"
#include "../simulator/openGL/FractalSection.h"
#include "../simulator/Fractal.h"
#include "../util/display.h"
#include "../simulator/Vec4.h"
#include "../simulator/Simulator.h"

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

void drawFractalEdge(Fractal& fractal) {
	// Create a window
	DisplayWindow::init(600, 600);

	// Create a texture to draw on
	SDL_Texture* texture = DisplayWindow::createTextureRGBA(fractal.getResolution(), fractal.getResolution());

	// Set render target(where it will draw) to the texture
	DisplayWindow::setRenderTarget(texture);

	// Fill the screen with a color
	DisplayWindow::fill(110, 160, 255, 255);

	FractalData::InitialCondition ic = fractal.getIc();

	CompassStruct compass_info;

	// Set the compass starting pos
	bool start_left = ic.l1 > 1.0 / (ic.m1 + 1);
	if (start_left) {
		// Start on the left side of the fractal
		compass_info.current_x = fractal.getResolution() / 2;
		compass_info.current_y = 0;
	}
	else {
		// Start at the top of the fractal
		compass_info.current_x = 0;
		compass_info.current_y = fractal.getResolution() / 2;
	}
	compass_info.grid_height = fractal.getResolution();
	compass_info.grid_width = fractal.getResolution();

	compass_info.getNextPoint(&fractal);

	int start_x = compass_info.current_x;
	int start_y = compass_info.current_y;

	// Get the next point, walking around the fractal counter clockwise
	compass_info.getNextPoint(&fractal);

	// Iterate while the compass has not gone around the fractal
	while (!(start_x == compass_info.current_x &&
		start_y == compass_info.current_y)) {

		// Get the next point, walking around the fractal counter clockwise
		compass_info.getNextPoint(&fractal);

		// Set render target(where it will draw) to the texture
		DisplayWindow::setRenderTarget(texture);

		// Draw a black square on that position
		DisplayWindow::drawRectangle(compass_info.current_x, compass_info.current_y, 1, 1, 0, 0, 0, 255);

		// Set render target(where it will draw) to the window
		DisplayWindow::setRenderTarget(nullptr);

		// Draw the texture on the window
		DisplayWindow::copyTextureToRenderTarget(texture, nullptr, nullptr);

		// Update what is one the window(to show what has been drawn)
		DisplayWindow::flip();

		// Make sure the window stays alive by updating it(also updates button presses etc.)
		DisplayWindow::update();
	}

	// Save the texture to file
	DisplayWindow::saveTextureToFile("fractal_edge_img.png", texture);

	// Run until the window is closed by user
	while (DisplayWindow::isRunning()) {
		DisplayWindow::update();
	}

	DisplayWindow::setRenderTarget(nullptr);
	DisplayWindow::destroyTexture(texture);

	// Exit function
	return;
}

void drawFractal(Fractal& fractal) {
	// Create a window
	DisplayWindow::init(600, 600);

	// Create a texture to draw on
	SDL_Texture* texture = DisplayWindow::createTextureRGBA(fractal.getResolution(), fractal.getResolution());

	// Set render target(where it will draw) to the texture
	DisplayWindow::setRenderTarget(texture);

	// Fill the screen with a color
	DisplayWindow::fill(110, 160, 255, 255);

	// Iterate through the fracal
	for (int x = 0; x < fractal.getResolution(); x++) {
		// Set render target(where it will draw) to the texture
		DisplayWindow::setRenderTarget(texture);
		for (int y = 0; y < fractal.getResolution(); y++) {
			if (fractal.getChar(x, y) == 0) {
				// Draw a black square if it does not flip
				DisplayWindow::drawRectangle(x, y, 1, 1, 0, 0, 0, 255);
			}
			else {
				// Draw a white square if it flips
				DisplayWindow::drawRectangle(x, y, 1, 1, 255, 255, 255, 255);
			}
		}
		// Set render target(where it will draw) to the window
		DisplayWindow::setRenderTarget(nullptr);

		// Draw the texture on the window
		DisplayWindow::copyTextureToRenderTarget(texture, nullptr, nullptr);

		// Update what is one the window(to show what has been drawn)
		DisplayWindow::flip();

		// Make sure the window stays alive by updating it(also updates button presses etc.)
		DisplayWindow::update();

	}

	// Save the texture to file
	DisplayWindow::saveTextureToFile("fractal_img.png", texture);

	// Run until the window is closed by user
	while (DisplayWindow::isRunning()) {
		DisplayWindow::update();
	}

	DisplayWindow::setRenderTarget(nullptr);
	DisplayWindow::destroyTexture(texture);

	// Exit function
	return;
}

int wmain() {
	double var = 8;

	// Where you want the output(no argument means that it does not save to file)
	PrintOutput print_output("data.txt");

	while (var <= 8) {
		// Set up the conditions
		FractalData::InitialCondition ic;
		ic.m1 = 1;
		ic.m2 = 16.0;
		ic.l1 = 1.0;
		ic.l2 = 1.0;
		ic.g = 9.82;

		// Set up the fractal calculation
		Fractal box_count_fractal = Fractal(1024, FractalData::flipFractal, ic);
		box_count_fractal.iteration_count = 1000;
		box_count_fractal.time_step = 0.01;

		// Draw the fractal on the screen(comment it using // while gathering data)
		//drawFractal(box_count_fractal);

		// Draw the edge of the fractal on the screen(comment it using // while gathering data)
		//drawFractalEdge(box_count_fractal);

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

	return 0;
}
