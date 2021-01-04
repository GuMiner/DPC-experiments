#include <iostream>
#include <glm/gtx/color_space.hpp> 
#include "ParticleRenderer.h"

ParticleRenderer::ParticleRenderer() :
	positionVbo(GL_STATIC_DRAW), colorVbo(GL_STATIC_DRAW) {
}

bool ParticleRenderer::Init(ShaderFactory& shaderFactory) {
	if (!shaderFactory.CreateShaderProgram("definedColorParticles", &programId))
	{
		std::cout << "Failed to load the 'definedColorParticles' shader!" << std::endl;
		return false;
	}
	projectionMatrixLocation = glGetUniformLocation(programId, "projectionMatrix");

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	positionVbo.SetupOpenGlBuffers();
	colorVbo.SetupOpenGlBuffers();

	positionVbo.vertices.push_back(glm::vec3(0.5, 0.5, 0.5));
	colorVbo.vertices.push_back(glm::vec3(0.9f, 0.9f, 0.9f));

	positionVbo.vertices.push_back(glm::vec3(0.11, 0.25, 0.70));
	colorVbo.vertices.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

	positionVbo.vertices.push_back(glm::vec3(0.54, 0.42, 0.32));
	colorVbo.vertices.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

	positionVbo.TransferToOpenGl();
	colorVbo.TransferToOpenGl();
	return true;
}

void ParticleRenderer::Transfer(std::vector<glm::vec3>* particlePositions) {
	positionVbo.vertices.clear();
	colorVbo.vertices.clear();
	float h = 0.7f;
	float s = 0.2f;
	float v = 0.3f;

	for (const glm::vec3& position : *particlePositions)
	{
		positionVbo.vertices.push_back(position);

		s = 1;
		v = 1;
		colorVbo.vertices.push_back(glm::rgbColor(glm::vec3(360 * h, s, v)));

		h += 0.05f;
		s += 0.05f;
		v += 0.05f;
		if (h >= 1.0f) { h = 0.0f; }
		if (s >= 1.0f) { s = 0.0f; }
		if (v >= 1.0f) { v = 0.0f; }
		// colorVbo.vertices.push_back(glm::vec3(0.5f, 1.0f, 0.3f));
	}

	glBindVertexArray(vao);
	positionVbo.TransferToOpenGl();
	colorVbo.TransferToOpenGl();
}

void ParticleRenderer::Update(float currentTime, float lastFrameTime, const Camera& camera) {
	// No updates needed for default Axis
}

void ParticleRenderer::Render(float currentTime, const glm::mat4& projectionMatrix) {
	glUseProgram(programId);
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, positionVbo.vertices.size());
}
