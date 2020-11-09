
#include "GpuInterface.h"
#include "Shader.h"
#include "Fractal.h"

#include <iostream>

unsigned int GpuInterface::fractalVAO, GpuInterface::fractalVBO, GpuInterface::fractalEBO;

Shader** GpuInterface::shaders = new Shader * [FractalData::Type::size];
GLFWwindow* GpuInterface::window = nullptr;

int GpuInterface::init() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);	// set opengl version to 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// setting up glfw to use core-profile (access to a smaller set of openGL functions)
	// creating a glfw window with the corresponding width, height and title (NOT USED)
	window = glfwCreateWindow(1, 1, "", nullptr, nullptr);
	glfwSetWindowPos(window, -1000, -1000);
	glfwHideWindow(window);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);	// make the context of the window the main context on this thread
	// passing glad the function to load the opengl function pointer (OS-specific)
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	// set appropriate blend function (when transparency is used)

	// VAO
	glGenVertexArrays(1, &fractalVAO);
	glBindVertexArray(fractalVAO);
	// VBO
	glGenBuffers(1, &fractalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, fractalVBO);
	// EBO
	glGenBuffers(1, &fractalEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fractalEBO);
	// sets up "drawVertices" and "drawIndices" data structure
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);	// first 2 floats - vertex x and y coordinates
	glEnableVertexAttribArray(0);
	// disable fractalVAO
	glBindVertexArray(0);

	// set up shaders
	shaders[FractalData::Type::flipFractal] = new Shader("src/simulator/shaders/flipFractal-fragment.glsl");

	return 0;
}