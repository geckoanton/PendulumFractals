
#include "../simulator/GpuInterface.h"
#include "../simulator/FractalSection.h"
#include "../simulator/Fractal.h"

#include <iostream>
#include <Windows.h>

int main() {
	std::cout << "teeeeeest" << std::endl;

	GpuInterface::init();

	FractalData::Section s = { 0.0, 0.0, 1.0, 1.0 };
	FractalData::InitialCondition ic = { 1.0, 1.0, 1.0, 1.0, 9.82 };

	/*SYSTEMTIME time;
	GetSystemTime(&time);
	LONG time_ms = (time.wSecond * 1000) + time.wMilliseconds;*/

	//FractalSection fs(GpuInterface::shaders[FractalData::Type::flipFractal], 128, &s, &ic);

	Fractal f(1024, FractalData::flipFractal, ic);

	std::cout << f.countBoxes(0) << std::endl;

	/*SYSTEMTIME time2;
	GetSystemTime(&time2);
	LONG time_ms2 = (time2.wSecond * 1000) + time2.wMilliseconds;*/

	//std::cout << time_ms2 - time_ms << std::endl;

	//glfwSwapBuffers(GpuInterface::window);

	return 0;
}
