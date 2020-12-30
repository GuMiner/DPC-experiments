#include <iostream>
#include "Axis.h"

Axis::Axis() : 
	positionVbo(GL_STATIC_DRAW), colorVbo(GL_STATIC_DRAW) {
}

bool Axis::Init(ShaderFactory& shaderFactory) {
	if (!shaderFactory.CreateShaderProgram("definedColor", &programId))
	{
		std::cout << "Failed to load the 'definedColor' shader!" << std::endl;
		return false;
	}
	projectionMatrixLocation = glGetUniformLocation(programId, "projectionMatrix");

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	positionVbo.SetupOpenGlBuffers();
	colorVbo.SetupOpenGlBuffers();

	positionVbo.vertices.push_back(glm::vec3(0, 0, 0));
	colorVbo.vertices.push_back(glm::vec3(0.9f, 0.9f, 0.9f));
	positionVbo.vertices.push_back(glm::vec3(1, 0, 0));
	colorVbo.vertices.push_back(glm::vec3(1.0f, 0.0f, 0.0f));

	positionVbo.vertices.push_back(glm::vec3(0, 0, 0));
	colorVbo.vertices.push_back(glm::vec3(0.9f, 0.9f, 0.9f));
	positionVbo.vertices.push_back(glm::vec3(0, 1, 0));
	colorVbo.vertices.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

	positionVbo.vertices.push_back(glm::vec3(0, 0, 0));
	colorVbo.vertices.push_back(glm::vec3(0.9f, 0.9f, 0.9f));
	positionVbo.vertices.push_back(glm::vec3(0, 0, 1));
	colorVbo.vertices.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

	positionVbo.TransferToOpenGl();
	colorVbo.TransferToOpenGl();
	return true;
}

void Axis::Update(float currentTime, float lastFrameTime, const Camera& camera) {
	// No updates needed for default Axis
}

void Axis::Render(float currentTime, const glm::mat4& projectionMatrix) {
	glUseProgram(programId);
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	
	glBindVertexArray(vao);
	glDrawArrays(GL_LINES, 0, positionVbo.vertices.size());
}
