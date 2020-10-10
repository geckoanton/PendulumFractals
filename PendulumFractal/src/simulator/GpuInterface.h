
#pragma once

// glad must be included before glfw
#include "../../glad.h"
#include "../GLFW/glfw3.h"

class Shader;

namespace GpuInterface {
	extern unsigned int fractalVAO, fractalVBO, fractalEBO;

	extern Shader** shaders;
	extern GLFWwindow* window;

	int init();
};
