#include <iostream>
#include <glm/gtx/transform.hpp>

#include "SimConstants.h"
#include "MeshRenderer.h"

MeshRenderer::MeshRenderer() :
	positionVbo(GL_STATIC_DRAW), normalVbo(GL_STATIC_DRAW), modelZRotation(0) {
}

bool MeshRenderer::Init(ShaderFactory& shaderFactory, std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals) {
	if (!shaderFactory.CreateShaderProgram("mesh", &programId))
	{
		std::cout << "Failed to load the 'mesh' shader!" << std::endl;
		return false;
	}
	projectionMatrixLocation = glGetUniformLocation(programId, "projectionMatrix");
	meshMatrixLocation = glGetUniformLocation(programId, "meshMatrix");

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	positionVbo.SetupOpenGlBuffers();
	normalVbo.SetupOpenGlBuffers();

	int counter = 0;
	for (const glm::vec3 vertex : vertices)
	{
		positionVbo.vertices.push_back(vertex);
		normalVbo.vertices.push_back(normals[counter / 3]);
		++counter;
	}

	positionVbo.TransferToOpenGl();
	normalVbo.TransferToOpenGl();
	return true;
}

void MeshRenderer::UpdateRotation(float currentZRotation) {
	modelZRotation = currentZRotation;
}

void MeshRenderer::Update(float currentTime, float lastFrameTime, const Camera& camera) {
	// No updates needed for default Axis

	// Test rotation
	modelZRotation += 0.001f;
}

void MeshRenderer::Render(float currentTime, const glm::mat4& projectionMatrix) {
	glUseProgram(programId);
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	
	// TODO -- this assumes the model is at the center of the simulation grid
	// This is right, but only for the fan model.
	glm::mat4 meshMatrix =
		glm::translate(glm::vec3(SIM_MAX / 2.0f, SIM_MAX / 2.0f, 0.0f)) * 
		glm::rotate(modelZRotation, glm::vec3(0.0f, 0.0f, 1.0f)) * 
		glm::translate(glm::vec3(-SIM_MAX / 2.0f, -SIM_MAX / 2.0f, 0.0f));
	glUniformMatrix4fv(meshMatrixLocation, 1, GL_FALSE, &meshMatrix[0][0]);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, positionVbo.vertices.size());
}
