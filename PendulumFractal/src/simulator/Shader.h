
#pragma once

#include <string>

class Shader {
public:
	Shader(std::string fragmentSource);

	void use();
	unsigned int getProgram();
	int getUniLoc(std::string var);
private:
	const std::string VERTEX_SOURCE = "src/simulator/shaders/default-vertex.glsl";

	unsigned int shaderProgram;
};