
#include "../simulator/openGL/GpuInterface.h"
#include "../simulator/openGL/FractalSection.h"
#include "../simulator/Fractal.h"
#include "../simulator/Lyapunov.h"
#include "../util/display.h"
#include "../simulator/Vec4.h"
#include "../simulator/Simulator.h"
#include "../simulator/openGL/Shader.h"
#include "../util/util.h"

#include <iostream>
#include <Windows.h>
#include <fstream>
#include <math.h>
#include <string>
#include <algorithm>
#include <chrono>
#include <thread>
#include <vector>
#include <limits>
#include <mutex>
#include <future>

class Mandelbrot : public Fractal {
public:
	Mandelbrot(int t_resolution) {
		resolution = t_resolution;
	}

	int mandelbrot(double startReal, double startImag, int maximum) {
		int counter = 0;
		double zReal = startReal;
		double zImag = startImag;
		double nextRe;

		while (pow(zReal, 2.0) + pow(zImag, 2.0) <= 4.0 && counter <= maximum) {
			nextRe = pow(zReal, 2.0) - pow(zImag, 2.0) + startReal;
			zImag = 2.0 * zReal * zImag + startImag;
			zReal = nextRe;
			if (zReal == startReal && zImag == startImag) { // A repetition indicates that the point is in the Mandelbrot set
				// Inside Mandelbrot set
				return -1;
			}
			counter += 1;
		}

		if (counter >= maximum) {
			// Inside Mandelbrot set
			return -1;
		}
		else {
			// Returning the iteration count(can be used for coloring)
			return counter;
		}
	}

	// Returns true if inside Mandelbrot set
	bool isInsideMandelbrot(int x, int y, int resolution, double offsetX = -0.7, double offsetY = 0.0, int MAX = 127) {
		// Convert x and y to the appropriate complex number
		double real = 3 * (x - (resolution / 2.0)) / resolution + offsetX;
		double imag = 3 * (y - (resolution / 2.0)) / resolution + offsetY;
		return (mandelbrot(real, imag, MAX) == -1);
	}

	unsigned char getChar(int x, int y) override {
		return(!isInsideMandelbrot(x, y, resolution));
	}

	virtual void getCompassStartPos(CompassStruct& compass_info) override {
		compass_info.grid_height = getResolution();
		compass_info.grid_width = getResolution();
		compass_info.current_x = resolution;
		compass_info.current_y = resolution / 2;

		while (getChar(compass_info.current_x, compass_info.current_y)) {
			compass_info.current_x--;
		}

		compass_info.direction = compass_info.POINTING_BOTTOM;

		compass_info.getNextPoint(this);
	}
};

void drawFractalEdge(Fractal& fractal) {
	// Create a window
	DisplayWindow::init(600, 600);

	// Create a texture to draw on
	SDL_Texture* texture = DisplayWindow::createTextureRGBA(fractal.getResolution(), fractal.getResolution());

	// Set render target(where it will draw) to the texture
	DisplayWindow::setRenderTarget(texture);

	// Fill the screen with a color
	DisplayWindow::fill(255, 255, 255, 255);

	FractalData::InitialCondition ic = fractal.getIc();

	CompassStruct compass_info;

	fractal.getCompassStartPos(compass_info);

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
		DisplayWindow::drawRectangle(compass_info.current_x, fractal.getResolution() - compass_info.current_y - 1, 1, 1, 0, 0, 0, 255);

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

	// Set render target(where it will draw) to the texture
	DisplayWindow::setRenderTarget(texture);

	// Draw a big red cross
	DisplayWindow::drawRectangle(fractal.getResolution() / 2, 0, 1, fractal.getResolution(), 255, 0, 0, 255);
	DisplayWindow::drawRectangle(0, fractal.getResolution() / 2, fractal.getResolution(), 1, 255, 0, 0, 255);

	// Set render target(where it will draw) to the window
	DisplayWindow::setRenderTarget(nullptr);

	// Draw the texture on the window
	DisplayWindow::copyTextureToRenderTarget(texture, nullptr, nullptr);

	// Update what is one the window(to show what has been drawn)
	DisplayWindow::flip();

	// Make sure the window stays alive by updating it(also updates button presses etc.)
	DisplayWindow::update();

	// Save the texture to file
	DisplayWindow::saveTextureToFile("fractal_edge_img_cross.png", texture);

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
	//for (int x = 0; x < fractal.getResolution() / 2; x++) {
	for (int x = 0; x < fractal.getResolution(); x++) {
		// Set render target(where it will draw) to the texture
		DisplayWindow::setRenderTarget(texture);
		for (int y = 0; y < fractal.getResolution(); y++) {
			if (fractal.getChar(x, y) == 0) {
				// Draw a black square if it does not flip
				DisplayWindow::drawRectangle(x, fractal.getResolution() - y - 1, 1, 1, 0, 0, 0, 255);
				//DisplayWindow::drawRectangle(fractal.getResolution() - x - 1, y, 1, 1, 0, 0, 0, 255);
			}
			/*else if (fractal.getChar(x, y) == 254) {
				//DisplayWindow::drawRectangle(x, fractal.getResolution() - y - 1, 1, 1, 0, 255, 255, 255);
				DisplayWindow::drawRectangle(x, fractal.getResolution() - y - 1, 1, 1, 255, 255, 255, 255);
			}*/
			else {
				// Draw a white square if it flips
				DisplayWindow::drawRectangle(x, fractal.getResolution() - y - 1, 1, 1, 255, 255, 255, 255);
				//DisplayWindow::drawRectangle(fractal.getResolution() - x - 1, y, 1, 1, 255, 255, 255, 255);
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

	std::string filename = "fractal_img";
	filename += "_m_";
	filename += std::to_string(fractal.getIc().m1);
	filename += "_l_";
	filename += std::to_string(fractal.getIc().l1);
	std::replace(filename.begin(), filename.end(), '.', ',');
	filename += ".png";

	std::cout << "filename: " << filename << std::endl;

	// Save the texture to file
	DisplayWindow::saveTextureToFile(filename.c_str(), texture);

	// Run until the window is closed by user
	while (DisplayWindow::isRunning()) {
		DisplayWindow::update();
	}

	DisplayWindow::setRenderTarget(nullptr);
	DisplayWindow::destroyTexture(texture);

	DisplayWindow::deinit();

	// Exit function
	return;
}

// Where start_x etc. is the fractal of the fractal 0 - 1
void drawFractalPart(Fractal& fractal, double t_start_x, double t_start_y, double t_size) {
	int img_resolution = fractal.getResolution() * t_size;
	int start_x = fractal.getResolution() * t_start_x;
	int start_y = fractal.getResolution() * t_start_y;
	int end_x = start_x + img_resolution;
	int end_y = start_y + img_resolution;
	
	// Create a window
	DisplayWindow::init(600, 600);

	// Create a texture to draw on
	SDL_Texture* texture = DisplayWindow::createTextureRGBA(img_resolution, img_resolution);

	// Set render target(where it will draw) to the texture
	DisplayWindow::setRenderTarget(texture);

	// Fill the screen with a color
	DisplayWindow::fill(110, 160, 255, 255);

	// Iterate through the fracal
	for (int x = 0; x < img_resolution; x++) {
		// Set render target(where it will draw) to the texture
		DisplayWindow::setRenderTarget(texture);
		for (int y = 0; y < img_resolution; y++) {
			if (fractal.getChar(x + start_x, y + start_y) == 0) {
				// Draw a black square if it does not flip
				DisplayWindow::drawRectangle(x, img_resolution - y - 1, 1, 1, 0, 0, 0, 255);
			}
			/*else if (fractal.getChar(x, y) == 254) {
				//DisplayWindow::drawRectangle(x, fractal.getResolution() - y - 1, 1, 1, 0, 255, 255, 255);
				DisplayWindow::drawRectangle(x, fractal.getResolution() - y - 1, 1, 1, 255, 255, 255, 255);
			}*/
			else {
				// Draw a white square if it flips
				DisplayWindow::drawRectangle(x, img_resolution - y - 1, 1, 1, 255, 255, 255, 255);
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

	std::string filename = "fractal_part_img.png";

	// Save the texture to file
	DisplayWindow::saveTextureToFile(filename.c_str(), texture);

	DisplayWindow::setRenderTarget(nullptr);
	DisplayWindow::destroyTexture(texture);

	DisplayWindow::deinit();

	// Exit function
	return;
}

void generateFlipFractalGraph() {
	int img_resolution;
	int iteration_count;
	double time_step;
	FractalData::InitialCondition ic;
	std::string user_input;
	user_input = askUser(
		"Write the corresponding number and hit Enter: \n" \
		"1: Generate a basic graph(m1=1, m2=1, l1=1, l2=1, gravity=9.82, time_step = 0.01, total_time = 10, resolution = 1024, entire graph shown)\n" \
		"2: Generate graph with custom parameters\n" \
	);
	if (user_input == "1") {
		ic.m1 = 1;
		ic.m2 = 1.0;
		ic.l1 = 1;
		ic.l2 = 1.0;
		ic.g = 9.82;
		time_step = 0.01;
		iteration_count = 1000;
		img_resolution = 512;
	}
	else if (user_input == "2") {
		ic.m1 = std::stod(askUser("mass 1: "));
		ic.m2 = std::stod(askUser("mass 2: "));
		ic.l1 = std::stod(askUser("length 1: "));
		ic.l2 = std::stod(askUser("length 2: "));
		ic.g = std::stod(askUser("gravity: "));
		time_step = std::stod(askUser("time step: "));
		iteration_count = std::stoi(askUser("iteration count: "));
		img_resolution = std::stoi(askUser("image resolution: "));
	}
	else {
		return;
	}

	// Set up the fractal calculation
	Fractal box_count_fractal = Fractal(img_resolution, FractalData::flipFractal, ic);
	box_count_fractal.iteration_count = iteration_count;
	box_count_fractal.time_step = time_step;
	drawFractal(box_count_fractal);
	std::cout << "You can now look at the finished image by opening the file(same directory as the .exe)\n";
}

void generateFlipFractalEdge() {
	int img_resolution;
	FractalData::InitialCondition ic;
	int iteration_count;
	double time_step;

	// Get the parameters
	std::string user_input = askUser(
		"Write the corresponding number and hit Enter: \n" \
		"1: Generate a basic edge(m1=1, m2=1, l1=1, l2=1, gravity=9.82, time_step = 0.01, iteration count = 1000, resolution = 512)\n" \
		"2: Generate custom graph\n"
	);
	if (user_input == "1") {
		ic.m1 = 1;
		ic.m2 = 1.0;
		ic.l1 = 1;
		ic.l2 = 1.0;
		ic.g = 9.82;
		time_step = 0.01;
		iteration_count = 1000;
		img_resolution = 512;
	}
	else if (user_input == "2") {
		ic.m1 = std::stod(askUser("mass 1: "));
		ic.m2 = std::stod(askUser("mass 2: "));
		ic.l1 = std::stod(askUser("length 1: "));
		ic.l2 = std::stod(askUser("length 2: "));
		ic.g = std::stod(askUser("gravity: "));
		time_step = std::stod(askUser("time step: "));
		iteration_count = std::stoi(askUser("iteration count: "));
		img_resolution = std::stoi(askUser("image resolution: "));
	}
	else {
		return;
	}

	// Set up the fractal calculation
	Fractal box_count_fractal = Fractal(img_resolution, FractalData::flipFractal, ic);
	box_count_fractal.iteration_count = 1000;
	box_count_fractal.time_step = 0.01;

	// Draw the fractal edge
	drawFractalEdge(box_count_fractal);
}

void pendulumSimulator(bool light_mode = true) {
	FractalData::InitialCondition ic;
	std::string user_input;
	double time_step;
	double angle_apha;
	double angle_beta;
	int fps;
	// Get the start parameters
	user_input = askUser("Write the corresponding number and hit Enter: \n" \
		"1: Default pendulum(m1=1, m2=1, l1=1, l2=1, gravity=9.82, time_step = 0.005, alpha=150dgr, beta=150dgr, fps=200)\n" \
		"2: Set custom parameters\n" \
	);
	if (user_input == "1") {
		ic.m1 = 1;
		ic.m2 = 1;
		ic.l1 = 1;
		ic.l2 = 1;
		ic.g = 9.82; // Gravitational constant(Earth's gravity)
		time_step = 0.005;
		angle_apha = (150 * 2 * PI) / 360;
		angle_beta = (150 * 2 * PI) / 360;
		fps = 200;
	}
	else if (user_input == "2") {
		ic.m1 = std::stod(askUser("mass 1: "));
		ic.m2 = std::stod(askUser("mass 2: "));
		ic.l1 = std::stod(askUser("length 1: "));
		ic.l2 = std::stod(askUser("length 2: "));
		ic.g = std::stod(askUser("gravity: "));
		time_step = std::stod(askUser("time step: "));
		angle_apha = (std::stod(askUser("alpha angle(degrees): ")) * 2 * PI) / 360;
		angle_beta = (std::stod(askUser("beta angle(degrees): ")) * 2 * PI) / 360;
		fps = std::stod(askUser("FPS(frames per second): "));
	}
	else {
		return;
	}

	// Set up the simulation
	Vec4 simulation_start(angle_apha, angle_beta, 0, 0);
	Simulator simulation(FractalData::flipFractal, simulation_start, &ic);
	simulation.time_step = time_step;
	DisplayWindow::init(600, 600);
	FpsTracker fps_tracker = FpsTracker(fps);

	int saved_resolution = pow(2, 10); 
	int length_factor = (saved_resolution*0.9) / (2*(ic.l1 + ic.l2));
	int ball_width = 20;
	auto texture = DisplayWindow::createTextureRGBA(saved_resolution, saved_resolution);
	auto texture2 = DisplayWindow::createTextureRGBA(saved_resolution, saved_resolution);

	// Run simulation
	while (DisplayWindow::isRunning()) {
		// Iterate pendulum simulation
		simulation.iterateSnapshot(time_step);

		// Get the pendulum coordinates
		int x1 = simulation.getx1() * length_factor + saved_resolution / 2;
		int y1 = -simulation.gety1() * length_factor + saved_resolution / 2;
		int x2 = simulation.getx2() * length_factor + saved_resolution / 2;
		int y2 = -simulation.gety2() * length_factor + saved_resolution / 2;
		
		if (light_mode) {
			// Draw tails
			DisplayWindow::setRenderTarget(texture);
			SDL_SetRenderDrawBlendMode(DisplayWindow::renderer, SDL_BLENDMODE_ADD);
			DisplayWindow::drawRectangle(0, 0, saved_resolution, saved_resolution, 255, 255, 255, 2);
			SDL_SetRenderDrawBlendMode(DisplayWindow::renderer, SDL_BLENDMODE_BLEND);
			DisplayWindow::drawRectangle(x1 - ball_width / 2, y1 - ball_width / 2, ball_width, ball_width, 0, 0, 255, 100);
			DisplayWindow::drawRectangle(x2 - ball_width / 2, y2 - ball_width / 2, ball_width, ball_width, 255, 0, 0, 100);

			// Draw black pendulum
			DisplayWindow::setRenderTarget(texture2);
			DisplayWindow::fill(255, 255, 255, 255);
			DisplayWindow::copyTextureToRenderTarget(texture, nullptr, nullptr);

			// Draw Grid
			for (int x = saved_resolution / 32; x < saved_resolution; x += saved_resolution / 16) {
				DisplayWindow::drawRectangle(x, 0, 2, saved_resolution, 200, 200, 200, 200);
			}

			for (int y = saved_resolution / 32; y < saved_resolution; y += saved_resolution / 16) {
				DisplayWindow::drawRectangle(0, y, saved_resolution, 2, 200, 200, 200, 200);
			}

			DisplayWindow::setRenderDrawColour(0, 0, 0, 255);
			SDL_RenderDrawLine(DisplayWindow::renderer, x1, y1, x2, y2);
			SDL_RenderDrawLine(DisplayWindow::renderer, x1, y1, saved_resolution/2, saved_resolution/2);
			DisplayWindow::drawRectangle(x1 - ball_width / 2, y1 - ball_width / 2, ball_width, ball_width, 0, 0, 0, 255);
			DisplayWindow::drawRectangle(x2 - ball_width / 2, y2 - ball_width / 2, ball_width, ball_width, 0, 0, 0, 255);
			DisplayWindow::drawRectangle((saved_resolution - ball_width) / 2, (saved_resolution - ball_width) / 2, ball_width, ball_width, 0, 0, 0, 255);
		}
		else {
			// Draw tails
			DisplayWindow::setRenderTarget(texture);
			SDL_SetRenderDrawBlendMode(DisplayWindow::renderer, SDL_BLENDMODE_MOD);
			DisplayWindow::drawRectangle(0, 0, saved_resolution, saved_resolution, 247, 255, 247, 255);
			SDL_SetRenderDrawBlendMode(DisplayWindow::renderer, SDL_BLENDMODE_BLEND);
			DisplayWindow::drawRectangle(x1 - ball_width / 2, y1 - ball_width / 2, ball_width, ball_width, 0, 0, 255, 100);
			DisplayWindow::drawRectangle(x2 - ball_width / 2, y2 - ball_width / 2, ball_width, ball_width, 255, 0, 0, 100);

			DisplayWindow::setRenderTarget(texture2);
			DisplayWindow::fill(0, 0, 0, 255);

			// Draw pendulum
			DisplayWindow::copyTextureToRenderTarget(texture, nullptr, nullptr);

			// Draw Grid
			for (int x = saved_resolution / 32; x < saved_resolution; x += saved_resolution / 16) {
				DisplayWindow::drawRectangle(x, 0, 2, saved_resolution, 0, 255, 0, 200);
			}

			for (int y = saved_resolution / 32; y < saved_resolution; y += saved_resolution / 16) {
				DisplayWindow::drawRectangle(0, y, saved_resolution, 2, 0, 255, 0, 200);
			}

			DisplayWindow::setRenderDrawColour(255, 255, 255, 255);
			SDL_RenderDrawLine(DisplayWindow::renderer, x1, y1, x2, y2);
			SDL_RenderDrawLine(DisplayWindow::renderer, x1, y1, (saved_resolution) / 2, (saved_resolution) / 2);
			DisplayWindow::drawRectangle(x1- ball_width / 2, y1- ball_width / 2, ball_width, ball_width, 0, 0, 255, 255);
			DisplayWindow::drawRectangle(x2- ball_width / 2, y2- ball_width / 2, ball_width, ball_width, 255, 0, 0, 255);
			DisplayWindow::drawRectangle((saved_resolution - ball_width) / 2, (saved_resolution - ball_width) / 2, ball_width, ball_width, 255, 255, 255, 255);
		}

		DisplayWindow::setRenderTarget(nullptr);
		DisplayWindow::fill(0, 0, 0, 255);

		// Draw pendulum
		DisplayWindow::copyTextureToRenderTarget(texture2, nullptr, nullptr);

		// Update the window
		DisplayWindow::flip();
		DisplayWindow::update();
		while (!fps_tracker.isUpdateTime()) {
			sleep(1);
		}
		fps_tracker.update_count++;

		/*// Save image to file
		if (!(fps_tracker.update_count % 40)) {
			std::cout << fps_tracker.update_count / 40 << std::endl;
			DisplayWindow::saveTextureToFile(std::string("animation/pendulum" + std::to_string(fps_tracker.update_count / 40) + ".png").c_str(), texture2);
		}*/
	}
}

void calculateCompassAndBoxDimention(Fractal& box_count_fractal) {
	// Allows for printing in console and in file at the same time (can also handle multiple threads)
	PrintOutput print_output("data.txt");

	FractalData::InitialCondition ic = box_count_fractal.getIc();

	// Print some info about simulation
	print_output << "m1: " << ic.m1 << "  m2: " << ic.m2 << "  l1:" << ic.l1 << "  l2: " << ic.l2 << "  g: " << ic.g << "\n";
	print_output << "time_step: " << box_count_fractal.time_step << "s  simulated_time: " <<
		box_count_fractal.time_step * box_count_fractal.iteration_count << "s" << "\n";
	print_output << "total size(resolution): " << box_count_fractal.getResolution() << "\n";

	// Record when simulation starts
	long long start_time = getTimeSinceEpoc();

	//print_output.printOnScreen();

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
}

// Results are saved to file
void setupCompassAndBoxDimentionCalculation() {
	std::string user_input;

	user_input = askUser("Write the corresponding number and hit Enter\n" \
		"1 : Keep old data\n" \
		"2 : Flush old data\n" \
	);

	if (user_input == "2") {
		// Overwrite old data
		std::ofstream outfile;
		outfile.open("data.txt", std::ios_base::out);
		outfile << "";
	}

	user_input = askUser("Write the corresponding number and hit Enter: \n" \
		"1: Default calculation(m1=1, m2=1, l1=1, l2=1, gravity=9.82, time_step = 0.01, iterations = 1000, resolution = 1024)\n" \
		"2: Calculation with custom parameters\n" \
		//"3: Multiple calculations with custom parameters\n"
	);

	double min_mass1;
	double max_mass1;
	double min_mass2;
	double max_mass2;
	//double mass_multiplier;
	double min_length1;
	double max_length1;
	double min_length2;
	double max_length2;
	//double length_multiplier;
	double min_gravity;
	double max_gravity;
	//double gravity_multiplier;
	double min_resolution;
	double max_resolution;
	//double resolution_multiplier;
	double min_time_step;
	double max_time_step;
	//double time_step_multiplier;
	double min_iteration_count;
	double max_iteration_count;
	//double iteration_count_multiplier;

	if (user_input == "1") {
		min_mass1 = 1;
		min_mass2 = 1;
		min_length1 = 1;
		min_length2 = 1;
		min_gravity = 9.82;
		min_time_step = 0.01;
		min_iteration_count = 1000;
		min_resolution = 1024;
	}
	if (user_input == "2") {
		min_mass1 = stod(askUser("Mass1: "));
		min_mass2 = stod(askUser("Mass2: "));
		min_length1 = stod(askUser("Length1: "));
		min_length2 = stod(askUser("Length2: "));
		min_gravity = stod(askUser("Gravity: "));
		min_time_step = stod(askUser("Time step: "));
		min_iteration_count = stoi(askUser("Iteration count: "));
		min_resolution = stoi(askUser("Resolution: "));
	}
	if (user_input == "1" || user_input == "2") {
		max_mass1 = min_mass1;
		max_mass2 = min_mass2;
		//mass_multiplier = 2;
		max_length1 = min_length1;
		max_length2 = min_length2;
		//length_multiplier = 2;
		max_gravity = min_gravity;
		//gravity_multiplier = 2;
		max_resolution = min_resolution;
		//resolution_multiplier = 2;
		max_time_step = min_time_step;
		//time_step_multiplier = 2;
		max_iteration_count = min_iteration_count;
		//iteration_count_multiplier = 2;
	}


	std::cout << "\nThe data will also be saved to data.txt\n";
	std::cout << "Calculating...\n\n";


	int mass1 = min_mass1;
	while (mass1 <= max_mass1) {
		int mass2 = min_mass2;
		while (mass2 <= max_mass2) {
			int length1 = min_length1;
			while (length1 <= max_length1) {
				int length2 = min_length2;
				while (length2 <= max_length2) {
					int resolution = min_resolution;
					int iteration_count = min_iteration_count;
					double time_step = min_time_step;

					FractalData::InitialCondition ic;
					ic.m1 = mass1;
					ic.m2 = mass2;
					ic.l1 = length1;
					ic.l2 = length2;
					ic.g = min_gravity;

					// Set up the fractal calculation
					Fractal box_count_fractal = Fractal(resolution, FractalData::flipFractal, ic);
					box_count_fractal.iteration_count = iteration_count;
					box_count_fractal.time_step = time_step;

					calculateCompassAndBoxDimention(box_count_fractal);
					length2 *= 2;
				}
				length1 *= 2;
			}
			mass2 *= 2;
		}
		mass1 *= 2;
	}
}

void drawMandelbrot() {
	std::string user_input = askUser(
		"Write the corresponding number and hit Enter: \n" \
		"1: use Default(resolution = 1024)\n" \
		"2: custom parameters\n" \
	);
	int resolution;
	if (user_input == "1") {
		resolution = 1024;
	}
	else if(user_input == "2")
	{
		resolution = stoi(askUser("Resolution: "));
	}
	else {
		return;
	}
	Mandelbrot mandelbrot = Mandelbrot(resolution);
	drawFractal(mandelbrot);
}

void drawMandelbrotEdge() {
	std::string user_input = askUser(
		"Write the corresponding number and hit Enter: \n" \
		"1: use Default(resolution = 1024)\n" \
		"2: custom parameters\n" \
	);
	int resolution;
	if (user_input == "1") {
		resolution = 1024;
	}
	else if (user_input == "2")
	{
		resolution = stoi(askUser("Resolution: "));
	}
	else {
		return;
	}
	Mandelbrot mandelbrot = Mandelbrot(resolution);
	drawFractalEdge(mandelbrot);
}

void calculateMandelbrotDimension() {
	std::string user_input = askUser(
		"Write the corresponding number and hit Enter: \n" \
		"1: use Default(resolution = 1024)\n" \
		"2: custom parameters\n" \
	);
	int resolution;
	if (user_input == "1") {
		resolution = 1024;
	}
	else if (user_input == "2")
	{
		resolution = stoi(askUser("Resolution: "));
	}
	else {
		return;
	}
	Mandelbrot mandelbrot = Mandelbrot(resolution);
	calculateCompassAndBoxDimention(mandelbrot);
}

void lyaFile(int i, int k, int gridSize) {
	Lyapunov l;

	Vec4 deltaError(0.0000001, 0, 0, 0);
	PREDEC D = 0.0001;
	int N = 100;
	PREDEC dt = 0.001;

	typedef std::numeric_limits< double > dbl;

	double m1Overm2 = pow(sqrt(2), i);
	double l1Overl2 = pow(sqrt(2), k);

	std::ofstream outdata;
	outdata.precision(dbl::max_digits10);
	std::string filename = "outdata/outdata_" + std::to_string(i) + "_" + std::to_string(k) + ".txt";
	outdata.open(filename);

	FractalData::InitialCondition ic = { m1Overm2, 1, l1Overl2, 1, 9.82 };

	outdata << "m1/m2" << "\t" << m1Overm2 << "\t" << "l1/l2" << "\t" << l1Overl2 << std::endl;

	double halfOffset = 0.5 / (double) gridSize;

	for (int b = 0; b < gridSize; b++) {
		for (int a = 0; a < gridSize; a++) {
			double alpha = 6.28318530718 * (((double)a / (double)gridSize) + halfOffset) - 3.14159265359;
			double beta = 6.28318530718 * (((double)(gridSize - 1 - b) / (double)gridSize) + halfOffset) - 3.14159265359;
			Vec4 refStart(alpha, beta, 0, 0);
			double generatedValue = l.generate(refStart, deltaError, D, N, dt, ic);
			outdata << generatedValue << "\t";
		}
		outdata << std::endl;
	}

	outdata << std::endl;
	outdata.close();
}
void generateLyapunovGrids() {
	std::cout << "Ouput files will be created in 'outdata' directory." << std::endl;
	std::cout << "Remember to turn off the screen saver." << std::endl;

	int minSpan;
	std::cout << "minSpan: ";
	std::cin >> minSpan;
	int maxSpan;
	std::cout << "maxSpan: ";
	std::cin >> maxSpan;

	int iteratorStart;
	std::cout << "iteratorStart: ";
	std::cin >> iteratorStart;
	int iteratorEnd;
	std::cout << "iteratorEnd: ";
	std::cin >> iteratorEnd;

	int gridSize;
	std::cout << "grid size: ";
	std::cin >> gridSize;

	const int MAX_THREADS = std::thread::hardware_concurrency();
	std::cout << "thread count: " << MAX_THREADS << std::endl;
	std::vector<std::thread*> threadObjs;

	std::cout << "running: { " << iteratorStart << " <= iterator < " << iteratorEnd << " }" << std::endl;

	for (int iterator = iteratorStart; iterator < iteratorEnd; iterator++) {
		int k = (iterator / ((maxSpan - minSpan) + 1)) + minSpan;
		int i = (iterator % ((maxSpan - minSpan) + 1)) + minSpan;

		if (threadObjs.size() == MAX_THREADS) {
			for (int a = 0; a < threadObjs.size(); a++) {
				threadObjs[a]->join();
				delete threadObjs[a];
			}
			threadObjs.clear();
		}
		threadObjs.push_back(new std::thread(lyaFile, i, k, gridSize));
	}

	for (int a = 0; a < threadObjs.size(); a++) {
		threadObjs[a]->join();
		delete threadObjs[a];
	}
	threadObjs.clear();

	std::cout << "Lyapunov grids generated." << std::endl;
}

void consoleApplication() {
	std::string user_input = askUser("Write the corresponding number and hit Enter\n" \
		"1 : Simulate pendulum(light mode)\n" \
		"2 : Simulate pendulum(dark mode)\n" \
		"3 : Generate flip-fractal graph\n" \
		"4 : Generate flip-fractal coastline\n" \
		"5 : Calculate compass and box dimention\n" \
		"6 : Generate Lyaponov grids\n" \
		"7 : Draw Mandelbrot-set\n"\
		"8 : Draw Mandelbrot-set coastline\n"\
		"9 : Calculate Mandelbrot-set dimension\n"\
	);
	while (true) {
		if (user_input == "1") {
			pendulumSimulator();
			return;
		}
		if (user_input == "2") {
			pendulumSimulator(false);
			return;
		}
		else if (user_input == "3") {
			generateFlipFractalGraph();
			return;
		}
		else if (user_input == "4") {
			generateFlipFractalEdge();
			return;
		}
		else if (user_input == "5") {
			setupCompassAndBoxDimentionCalculation();
			return;
		}
		else if (user_input == "6") {
			generateLyapunovGrids();
			return;
		}
		else if (user_input == "7") {
			drawMandelbrot();
			return;
		}
		else if (user_input == "8") {
			drawMandelbrotEdge();
			return;
		}
		else if (user_input == "9") {
			calculateMandelbrotDimension();
			return;
		}
		else {
			user_input = askUser("Invalid number!\n");
		}
	}
	
}

int wmain() {
	/*rand__seedUsingCurrentTime;
	while (true) {
		//added_angle_x = ((rand_::thread_rng() % pow(2, 13)) * PI) / 100000;
		//added_angle_y = ((rand_::thread_rng() % 100000) * PI) / 100000;

		FractalData::InitialCondition ic;
		ic.m1 = 1;
		ic.m2 = 1;
		ic.l1 = 1;
		ic.l2 = 1;
		ic.g = 9.82;

		int resolution = pow(2, 13);

		shifted_graph_x = ((rand_::thread_rng() % resolution) + resolution) % resolution;
		shifted_graph_y = ((rand_::thread_rng() % resolution) + resolution) % resolution;
		std::cout << "shift_x: " << shifted_graph_x << std::endl;
		std::cout << "shift_y: " << shifted_graph_y << std::endl;

		// Set up the fractal calculation
		Fractal box_count_fractal = Fractal(resolution, FractalData::flipFractal, ic);
		box_count_fractal.iteration_count = 1000;
		box_count_fractal.time_step = 0.01;

		calculateCompassAndBoxDimention(box_count_fractal);
	}*/
	//drawMandelbrot();
	while (true) {
		consoleApplication();
		std::cout << "\n\n\n\n\n\n\n\n\n";
	}
	return 0;
}

void setPixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
	Uint32* target_pixel = (Uint32*) ((Uint8*)surface->pixels + y * surface->pitch +
		x * sizeof * target_pixel);
	*target_pixel = pixel;
}

/*int wmain() {
	GpuInterface::init();

	const int STEPS = 300;
	const int SPAN_X = 6;
	const int SPAN_Y = 6;
	const float PI = 3.1415926535897932384626433832795028841971693993751058209749445923078164062;

	float angle = 0;
	float radius = 0;

	for (int i = 0; i < STEPS; i++) {
		float cordX = (cos(angle) * radius) * SPAN_X;
		float cordY = (sin(angle) * radius) * SPAN_Y;

		float m1Overm2 = pow(sqrt(2), cordX);
		float l1Overl2 = pow(sqrt(2), cordY);

		FractalData::Section* se = new FractalData::Section;
		se->cornerX = 0; se->cornerY = 0; se->spanX = 1; se->spanY = 1;
		FractalData::InitialCondition* ic = new FractalData::InitialCondition;
		ic->m1 = m1Overm2; ic->m2 = 1; ic->l1 = l1Overl2; ic->l2 = 1; ic->g = 9.82;
		FractalSection fs(new Shader("src/openGL/shaders/image-flip.glsl"), FractalData::MAX_FRACTAL_SECTION_SIDE, FractalData::MAX_FRACTAL_SECTION_SIDE,
			se, ic);


		int resolution = FractalData::MAX_FRACTAL_SECTION_SIDE;
		SDL_Surface* surface = SDL_CreateRGBSurface(0, resolution, resolution, 32, 0, 256, 256 * 256, 256 * 256 * 256);

		for (int y = 0; y < fs.getHeight(); y++) {
			for (int x = 0; x < fs.getWidth(); x++) {
				if (fs.getBuffer()[y * fs.getWidth() + x] == 0) {
					setPixel(surface, x, y, 255 * 256 * 256 * 256);
					//std::cout << "-";
				}
				else {
					setPixel(surface, x, y, 256*256*256*256 - 1);
					//std::cout << "#";
				}
			}
			//std::cout << std::endl;
		}
		//std::cout << std::endl;

		// Save the image to file
		std::cout << "working " << i << std::endl;
		IMG_SavePNG(surface, std::string("animation/cool_picture" + std::to_string(i) + ".png").c_str());

		delete se; delete ic;

		angle += (2 * PI) / (float)STEPS;
		radius += 1.0f / (float)STEPS;
	}*/

	/*Lyapunov l;

	Vec4 deltaError(0.0000001, 0, 0, 0);
	PREDEC D = 0.0001;
	int N = 4;
	PREDEC dt = 0.001;

	//std::cout << "m: " << m1Overm2 << " | l: " << l1Overl2 << std::endl;

	FractalData::InitialCondition ic = { 1, 1, 1, 1, 9.82 };
	Vec4 refStart(1.5, 1.5, 0, 0);
	l.generate(refStart, deltaError, D, N, dt, ic);*/

	/*Lyapunov l;

	Vec4 deltaError(0.0000001, 0, 0, 0);
	PREDEC D = 0.0001;
	int N = 100;
	PREDEC dt = 0.001;

	//std::cout << "m: " << m1Overm2 << " | l: " << l1Overl2 << std::endl;

	FractalData::InitialCondition ic = { 1, 1, 1, 1, 9.82 };

	double alpha = 1.5;
	double beta = 1.5;

	Vec4 refStart(alpha, beta, 0, 0);
	l.generate(refStart, deltaError, D, N, dt, ic);*/

	/*GpuInterface::init();

	FractalData::Section* s = new FractalData::Section;
	s->cornerX = 0.0f; s->cornerY = 0.0f; s->spanX = 1.0f; s->spanY = 1.0f;
	FractalData::InitialCondition* ic = new FractalData::InitialCondition;
	ic->m1 = 0.25f; ic->m2 = 1.0f; ic->l1 = 0.25f; ic->l2 = 1.0f; ic->g = 9.82f;

	FractalSection fs(new Shader("src/simulator/openGL/shaders/lyapunov-fragment.glsl"), FractalData::MAX_FRACTAL_SECTION_SIDE, FractalData::MAX_FRACTAL_SECTION_SIDE, s, ic);
	for (int y = 0; y < fs.getHeight(); y++) {
		for (int x = 0; x < fs.getWidth(); x++) {
			std::cout << (int) fs.getBuffer()[y * fs.getWidth() + x] << std::endl;
			if (fs.getBuffer()[y * fs.getWidth() + x] == 0) {
				std::cout << "-";
			}
			else {
				std::cout << "#";
			}
		}
		std::cout << std::endl;
	}*/

	/*Lyapunov l;

	Vec4 refStart(1.5, 1.5, 0, 0);
	Vec4 deltaError(0.000000001, 0, 0, 0);
	PREDEC D = 0.0001;
	int N = 100;
	PREDEC dt = 0.001;
	FractalData::InitialCondition ic = { 1, 1, 1, 1, 9.82 };

	l.generate(refStart, deltaError, D, N, dt, ic);*/

	/*typedef std::numeric_limits< double > dbl;

	int outerMinSpan, outerMaxSpan;
	std::cout << "outer min span: ";
	std::cin >> outerMinSpan;
	std::cout << "outer max span: ";
	std::cin >> outerMaxSpan;

	int innerMinSpan, innerMaxSpan;
	std::cout << "inner min span: ";
	std::cin >> innerMinSpan;
	std::cout << "inner max span: ";
	std::cin >> innerMaxSpan;

	int gridSize;
	std::cout << "grid size: ";
	std::cin >> gridSize;

	const int MAX_THREADS = std::thread::hardware_concurrency();
	std::cout << "thread count: " << MAX_THREADS << std::endl;
	std::vector<std::thread*> threadObjs;

	std::cout << "running: { " << outerMinSpan << " <= k <= " << outerMaxSpan << " }, { " << innerMinSpan << " <= i <= " << innerMaxSpan << " }" << std::endl;

	for (int k = outerMinSpan; k <= outerMaxSpan; k++) {
		for (int i = innerMinSpan; i <= innerMaxSpan; i++) {
			if (threadObjs.size() == MAX_THREADS) {
				for (int a = 0; a < threadObjs.size(); a++) {
					threadObjs[a]->join();
					delete threadObjs[a];
				}
				threadObjs.clear();
			}
			threadObjs.push_back(new std::thread(lyaFile, i, k, gridSize));
		}
	}

	for (int a = 0; a < threadObjs.size(); a++) {
		threadObjs[a]->join();
		delete threadObjs[a];
	}
	threadObjs.clear();

	std::cout << "lyapunov generated" << std::endl;*/

	/*for (int i = -8; i <= 8; i++) {

		double l1Overl2 = pow(sqrt(2), i);

		std::cout << "lIteration: " << l1Overl2 << std::endl;

		FractalData::InitialCondition ic = { 1, 1, l1Overl2, 1, 9.82 };

		outdata << "m1/m2" << "\t" << 1 << "\t" << "l1/l2" << "\t" << l1Overl2 << std::endl;

		for (int b = 0; b < GRID_SIZE; b++) {
			for (int a = 0; a < GRID_SIZE; a++) {
				Vec4 refStart(6.28318530718 * ((double)a / (double)GRID_SIZE) - 3.14159265359, 6.28318530718 * ((double)(GRID_SIZE - 1 - b) / (double)GRID_SIZE) - 3.14159265359, 0, 0);
				outdata << l.generate(refStart, deltaError, D, N, dt, ic) << "\t";
			}
			outdata << std::endl;
		}

		outdata << std::endl << std::endl;
	}

	outdata.close();*/


	/*std::vector<PREDEC> values;
	std::string line;
	std::ifstream myfile("data.txt");
	if (myfile.is_open()) {
		while (getline(myfile, line)) {
			if (line.substr(0, 1) != "#" && line != "")
				values.push_back(std::stod(line));
		}
		myfile.close();
	}
	else {
		std::cout << "Kunde inte oppna filen 'data.txt' :(" << std::endl;
		std::cin.get();
		return 1;
	}*/

	/*for (int i = 0; i < values.size(); i++) {
		std::cout << values[i] << std::endl;
	}*/

	/*FractalData::InitialCondition ic = { values[0], values[1], values[2], values[3], values[4] };
	Vec4 refStart(values[5], values[6], values[7], values[8]);
	Vec4 deltaError(values[9], values[10], values[11], values[12]);
	Lyapunov lya;
	std::cout << "lyaponov is: " << std::endl;
	std::cout << lya.generate(refStart, deltaError, values[13], values[14], values[15], ic) << std::endl;

	std::cin.get();*/

	//generate(Vec4 refStart, Vec4 deltaError, PREDEC D, int N, PREDEC dt, FractalData::InitialCondition ic)

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

	/*return 0;
}*/



/*//DisplayWindow::init(600, 600);

	// Where you want the output(no argument means that it does not save to file)
	PrintOutput print_output("data.txt");

	int resolution = pow(2, 10);

	for (double m = 0; m < 1; m++) {
		for (double l = 0; l < 1; l++) {
			// Set up the conditions
			FractalData::InitialCondition ic;
			ic.m1 = 8;
			ic.m2 = 1.0;
			ic.l1 = 8;
			ic.l2 = 1.0;
			ic.g = 9.82;

			// Set up the fractal calculation
			Fractal box_count_fractal = Fractal(resolution, FractalData::flipFractal, ic);
			box_count_fractal.iteration_count = 10000;
			box_count_fractal.time_step = 0.001;

			// Print some info about simulation
			print_output << "m1: " << ic.m1 << "  m2: " << ic.m2 << "  l1:" << ic.l1 << "  l2: " << ic.l2 << "  g: " << ic.g << "\n";
			print_output << "time_step: " << box_count_fractal.time_step << "s  simulated_time: " <<
				box_count_fractal.time_step * box_count_fractal.iteration_count << "s" << "\n";
			print_output << "total size(resolution): " << box_count_fractal.getResolution() << "\n";

			// Record when simulation starts
			long long start_time = getTimeSinceEpoc();

			// Draw the fractal on the screen(comment it using // while gathering data)
			drawFractal(box_count_fractal);

			//drawFractalPart(box_count_fractal, 3068.0 / 4096, 1851.0 / 4096, 1.0 / 8);

			// Draw the edge of the fractal on the screen(comment it using // while gathering data)
			//drawFractalEdge(box_count_fractal);

			print_output.printOnScreen();

			// Do the calculation and print results
			//box_count_fractal.getCompassDimension(print_output);

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
		}
	}

	//drawCompleteImage(resolution);*/


/*void drawCompleteImage(int resolution) {
	DisplayWindow::init(600, 600);

	SDL_Surface* surface = SDL_CreateRGBSurface(0, resolution * 13, resolution * 13, 32, 0, 256, 256*256, 256*256*256);
	//SDL_Texture* texture = DisplayWindow::createTextureRGBA(resolution *13, resolution*13);

	// Set render target(where it will draw) to the texture
	//DisplayWindow::setRenderTarget(texture);

	// Fill the screen with a color
	DisplayWindow::fill(110, 160, 255, 255);

	for (double m = 0; m < 13; m ++) {
		for (double l = 0; l < 13; l ++) {

			std::string filename = "fractal_img";
			filename += "_m_";
			filename += std::to_string(pow(2, m * 0.5 - 3));
			filename += "_l_";
			filename += std::to_string(pow(2, l * 0.5 - 3));
			std::replace(filename.begin(), filename.end(), '.', ',');
			filename += ".png";

			std::cout << filename << std::endl;

			//SDL_Texture* loaded_texture = DisplayWindow::loadTexture(filename);
			SDL_Surface* loaded_surface = DisplayWindow::loadSurface(filename);

			auto rect = DisplayWindow::createSDLRect(m * resolution, l * resolution,
				resolution, resolution);

			SDL_BlitSurface(loaded_surface, NULL, surface, &rect);

			// Draw the texture on the window
			//DisplayWindow::copyTextureToRenderTarget(loaded_texture, nullptr, &rect);

			DisplayWindow::destroySurface(loaded_surface);
		}
	}

	// Set render target(where it will draw) to the window
	DisplayWindow::setRenderTarget(nullptr);

	// Draw the texture on the window
	//DisplayWindow::copyTextureToRenderTarget(texture, nullptr, nullptr);

	// Update what is one the window(to show what has been drawn)
	DisplayWindow::flip();

	// Make sure the window stays alive by updating it(also updates button presses etc.)
	DisplayWindow::update();

	// Save the texture to file
	//DisplayWindow::saveTextureToFile("complete_texture.png", texture);
	IMG_SavePNG(surface, "complete_texture.png");


	// Set render target(where it will draw) to the texture
	DisplayWindow::setRenderTarget(texture);

	// Draw a big red cross
	DisplayWindow::drawRectangle(fractal.getResolution() / 2, 0, 1, fractal.getResolution(), 255, 0, 0, 255);
	DisplayWindow::drawRectangle(0, fractal.getResolution() / 2, fractal.getResolution(), 1, 255, 0, 0, 255);

	// Set render target(where it will draw) to the window
	DisplayWindow::setRenderTarget(nullptr);

	// Draw the texture on the window
	DisplayWindow::copyTextureToRenderTarget(texture, nullptr, nullptr);

	// Update what is one the window(to show what has been drawn)
	DisplayWindow::flip();

	// Make sure the window stays alive by updating it(also updates button presses etc.)
	DisplayWindow::update();

	// Save the texture to file
	DisplayWindow::saveTextureToFile("fractal_img_cross.png", texture);*/

	// Run until the window is closed by user
	/*while (DisplayWindow::isRunning()) {
		DisplayWindow::update();
	}

	DisplayWindow::setRenderTarget(nullptr);
	DisplayWindow::destroySurface(surface);

	DisplayWindow::deinit();

	// Exit function
	return;
}*/

/*// Set render target(where it will draw) to the texture
	DisplayWindow::setRenderTarget(texture);

	// Draw a big red cross
	DisplayWindow::drawRectangle(fractal.getResolution() / 2, 0, 1, fractal.getResolution(), 255, 0, 0, 255);
	DisplayWindow::drawRectangle(0, fractal.getResolution() / 2, fractal.getResolution(), 1, 255, 0, 0, 255);

	// Set render target(where it will draw) to the window
	DisplayWindow::setRenderTarget(nullptr);

	// Draw the texture on the window
	DisplayWindow::copyTextureToRenderTarget(texture, nullptr, nullptr);

	// Update what is one the window(to show what has been drawn)
	DisplayWindow::flip();

	// Make sure the window stays alive by updating it(also updates button presses etc.)
	DisplayWindow::update();

	// Save the texture to file
	DisplayWindow::saveTextureToFile("fractal_img_cross.png", texture);*/

	// Run until the window is closed by user
	/*while (DisplayWindow::isRunning()) {
		DisplayWindow::update();
	}*/