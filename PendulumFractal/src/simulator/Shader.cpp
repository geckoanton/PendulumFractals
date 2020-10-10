
#include "shader.h"
#include "GpuInterface.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(std::string fragmentSource) {
	// load vertexShader and fragmentShader sources
	std::ifstream file;
	file.open(VERTEX_SOURCE);	// open vertex source
	std::stringstream vertexStream;
	vertexStream << file.rdbuf();
	file.close();

	std::string vertexStr = vertexStream.str();

	char* vtxStr = new char[vertexStr.length() + 1];	// store the files source in vtxStr
	vertexStr.copy(vtxStr, vertexStr.length() + 1);
	vtxStr[vertexStr.length()] = '\0';

	file.open(fragmentSource);	// open fragment source
	std::stringstream fragmentStream;
	fragmentStream << file.rdbuf();
	file.close();

	std::string fragmentStr = fragmentStream.str();

	char* frgStr = new char[fragmentStr.length() + 1];	// store the files source in frgStr
	fragmentStr.copy(frgStr, fragmentStr.length() + 1);
	frgStr[fragmentStr.length()] = '\0';

	int success;
	char infoLog[512];

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vtxStr, NULL);
	glCompileShader(vertexShader);	// compiling vertex shader
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "Vertex shader: \"" + VERTEX_SOURCE + "\" failed compilation:\n" + infoLog << std::endl;
	}

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &frgStr, NULL);
	glCompileShader(fragmentShader);	// compiling fragment shader
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "Fragment shader: \"" + fragmentSource + "\" failed compilation:\n" + infoLog << std::endl;
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	delete[] vtxStr;
	delete[] frgStr;
}
void Shader::use() {
	glUseProgram(shaderProgram);
}
unsigned int Shader::getProgram() {
	return shaderProgram;
}
int Shader::getUniLoc(std::string var) {
	return glGetUniformLocation(getProgram(), var.c_str());
}
