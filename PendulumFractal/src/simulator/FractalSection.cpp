
#include "FractalSection.h"
#include "GpuInterface.h"
#include "Shader.h"
#include "Fractal.h"

#include <iostream>
#include <math.h>

bool FractalSection::gpuInterfaceInited = false;

FractalSection::FractalSection(Shader* shader, int width, int height, FractalData::Section* s, FractalData::InitialCondition* ic) {
	if (!gpuInterfaceInited) {
		GpuInterface::init();
		gpuInterfaceInited = true;
	}

	// FBO
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	// texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, FractalData::MAX_FRACTAL_SECTION_SIDE, FractalData::MAX_FRACTAL_SECTION_SIDE, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);	// set texture at attachment0 (location 0?)
	// needed?!
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Failed to initialize FractalSection FBO." << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);	// return to screen buffer

	this->shader = shader;
	this->width = pow(2, ceil(log2(width)));
	this->height = pow(2, ceil(log2(height)));

	generate(s, ic);
	createDataArray();
}
FractalSection::~FractalSection() {
	if (buffer != nullptr) {
		delete[] buffer;
	}
	glDeleteTextures(1, &texture);
	glDeleteFramebuffers(1, &FBO);
}

unsigned char* FractalSection::getBuffer() {
	return buffer;
}

int FractalSection::getSize() {
	return width * height;
}
int FractalSection::getWidth() {
	return width;
}
int FractalSection::getHeight() {
	return height;
}

void FractalSection::generate(FractalData::Section* s, FractalData::InitialCondition* ic) {
	// set vertices
	float* v = new float[4 * 2];
	v[0 * 2 + 0] = -1;
	v[0 * 2 + 1] = (float)height * 2 / (float)FractalData::MAX_FRACTAL_SECTION_SIDE - 1;

	v[1 * 2 + 0] = (float)width * 2 / (float)FractalData::MAX_FRACTAL_SECTION_SIDE - 1;
	v[1 * 2 + 1] = (float)height * 2 / (float)FractalData::MAX_FRACTAL_SECTION_SIDE - 1;;

	v[2 * 2 + 0] = (float)width * 2 / (float)FractalData::MAX_FRACTAL_SECTION_SIDE - 1;
	v[2 * 2 + 1] = -1;

	v[3 * 2 + 0] = -1;
	v[3 * 2 + 1] = -1;

	// triangulate
	int* i = new int[(4 - 2) * 3];
	for (int k = 0; k < 4 - 2; k++) {
		i[k * 3] = 0;
		i[k * 3 + 1] = 4 - k - 1;
		i[k * 3 + 2] = 4 - k - 2;
	}
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	shader->use();

	glUniform2f(shader->getUniLoc("end"), (2.0f * (float) width) / (float) FractalData::MAX_FRACTAL_SECTION_SIDE, (2.0f * (float) height) / (float) FractalData::MAX_FRACTAL_SECTION_SIDE);
	glUniform4f(shader->getUniLoc("section"), s->cornerX, s->cornerY, s->spanX, s->spanY);

	glUniform1f(shader->getUniLoc("m1"), ic->m1);
	glUniform1f(shader->getUniLoc("m2"), ic->m2);
	glUniform1f(shader->getUniLoc("l1"), ic->l1);
	glUniform1f(shader->getUniLoc("l2"), ic->l2);
	glUniform1f(shader->getUniLoc("g"), ic->g);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindVertexArray(GpuInterface::fractalVAO);
	glBindBuffer(GL_ARRAY_BUFFER, GpuInterface::fractalVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GpuInterface::fractalEBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), v, GL_STREAM_DRAW);	// load fractal vertices
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (4 - 2) * 3 * sizeof(int), i, GL_STREAM_DRAW);	// load fractal EBO:s
	glViewport(0, 0, FractalData::MAX_FRACTAL_SECTION_SIDE, FractalData::MAX_FRACTAL_SECTION_SIDE);
	glDrawElements(GL_TRIANGLES, (4 - 2) * 3, GL_UNSIGNED_INT, 0);	// draw content to screen
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	delete[] v;
	delete[] i;

}
void FractalSection::createDataArray() {
	buffer = new unsigned char[width * height];

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	double currentTime = glfwGetTime();
	while (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		if (glfwGetTime() - currentTime > MAX_FBO_WAIT) {
			std::cout << "Fractal generation exceeded maximum wait time." << std::endl;
			break;
		}
	}


	glReadPixels(0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
