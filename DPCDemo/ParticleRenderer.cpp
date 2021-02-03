#include <algorithm>
#include <iostream>
#include <glm/gtx/color_space.hpp> 

#include "SimConstants.h"
#include "ParticleRenderer.h"

ParticleRenderer::ParticleRenderer() :
	positionVbo(GL_STATIC_DRAW), colorVbo(GL_STATIC_DRAW) {
}

bool ParticleRenderer::Init(ShaderFactory& shaderFactory) {
	if (!shaderFactory.CreateShaderProgram("definedColorParticles", &programId)) {
		std::cout << "Failed to load the 'definedColorParticles' shader!" << std::endl;
		return false;
	}
	projectionMatrixLocation = glGetUniformLocation(programId, "projectionMatrix");
	viewMatrixLocation = glGetUniformLocation(programId, "viewMatrix");
	pointScaleLocation = glGetUniformLocation(programId, "pointScale");

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

void ParticleRenderer::Transfer(std::vector<glm::vec3>& particlePositions, std::vector<float>& particleSpeeds) {
	positionVbo.vertices.clear();
	colorVbo.vertices.clear();
	float h = 0.7f;

	auto minAndMax  = std::minmax_element(particleSpeeds.begin(), particleSpeeds.end());

	long count = 0;
	for (const glm::vec3& position : particlePositions) {
		positionVbo.vertices.push_back(position);

		// Color by speed
		if (!COLOR_PARTICLES_RANDOMLY) {
			h = (particleSpeeds[count] - minAndMax.first[0]) / (minAndMax.second[0] - minAndMax.first[0]);
		}

		colorVbo.vertices.push_back(glm::rgbColor(glm::vec3(360 * h, 1.0f, 1.0f)));

		if (COLOR_PARTICLES_RANDOMLY) {
			h += 0.05f;
			if (h >= 1.0f) { h = 0.0f; }
		}
		
		++count;
	}

	glBindVertexArray(vao);
	positionVbo.TransferToOpenGl();
	colorVbo.TransferToOpenGl();
}

void ParticleRenderer::SetViewMatrix(glm::mat4& viewMatrix, float pointScale)
{
	this->viewMatrix = viewMatrix;
	this->pointScale = pointScale;
}

void ParticleRenderer::Update(float currentTime, float lastFrameTime, const Camera& camera) {
	// No updates needed for default Axis
}

void ParticleRenderer::Render(float currentTime, const glm::mat4& projectionMatrix) {
	glUseProgram(programId);
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniform1f(pointScaleLocation, pointScale);

	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, positionVbo.vertices.size());
}
