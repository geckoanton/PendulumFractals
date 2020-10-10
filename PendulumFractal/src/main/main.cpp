
#include <iostream>
#include "../simulator/GpuInterface.h"
#include "../simulator/FractalSection.h"
#include "../simulator/Fractal.h"

int main() {
	std::cout << "teeeeeest" << std::endl;

	GpuInterface::init();
	FractalSection fs(GpuInterface::shaders[FractalData::Type::flipFractal], 256, 256);

	FractalData::Section s = { 0.0, 0.0, 1.0, 1.0 };
	FractalData::InitialCondition ic = { 1.0, 1.0, 1.0, 1.0, 9.82 };

	fs.generateFractal(&s, &ic);
	glfwSwapBuffers(GpuInterface::window);

	while (true);

	return 0;
}
