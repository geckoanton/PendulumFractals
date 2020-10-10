
#include "FractalSection.h"
#include "GpuInterface.h"
#include "Shader.h"
#include "Fractal.h"

#include <iostream>

FractalSection::FractalSection(Shader* shader, int width, int height) {
	// FBO
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	// texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_R8, GL_UNSIGNED_BYTE, 0);
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
	this->width = width;
	this->height = height;
}

void FractalSection::generateFractal(FractalData::Section* s, FractalData::InitialCondition* ic) {
	// set vertices
	float* v = new float[4 * 2];
	v[0 * 2 + 0] = -1;
	v[0 * 2 + 1] = 1;

	v[1 * 2 + 0] = 1;
	v[1 * 2 + 1] = 1;

	v[2 * 2 + 0] = 1;
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
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindVertexArray(GpuInterface::fractalVAO);
	glBindBuffer(GL_ARRAY_BUFFER, GpuInterface::fractalVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GpuInterface::fractalEBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), v, GL_STREAM_DRAW);	// load fractal vertices
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (4 - 2) * 3 * sizeof(int), i, GL_STATIC_DRAW);	// load fractal EBO:s
	glViewport(0, 0, width, height);
	glDrawElements(GL_TRIANGLES, (4 - 2) * 3, GL_UNSIGNED_INT, 0);	// draw content to screen
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	delete[] v;
	delete[] i;


	/*float* v = new float[4 * 2];
	v[0 * 2 + 0] = -1;
	v[0 * 2 + 1] = 1;

	v[1 * 2 + 0] = 1;
	v[1 * 2 + 1] = 1;

	v[2 * 2 + 0] = 1;
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
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(GpuInterface::fractalVAO);
	glBindBuffer(GL_ARRAY_BUFFER, GpuInterface::fractalVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GpuInterface::fractalEBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), v, GL_STREAM_DRAW);	// load fractal vertices
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (4 - 2) * 3 * sizeof(int), i, GL_STATIC_DRAW);	// load fractal EBO:s
	glViewport(0, 0, width, height);
	glDrawElements(GL_TRIANGLES, (4 - 2) * 3, GL_UNSIGNED_INT, 0);	// draw content to screen
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	delete[] v;
	delete[] i;*/

}
