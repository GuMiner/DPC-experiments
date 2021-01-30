#include <iostream>

#include "SimConstants.h"
#include "MeshRenderer.h"

MeshRenderer::MeshRenderer() :
	positionVbo(GL_STATIC_DRAW), normalVbo(GL_STATIC_DRAW) {
	meshMatrix = glm::mat4(1.0f); // Identity matrix
}

bool MeshRenderer::Init(ShaderFactory& shaderFactory, std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals) {
	if (!shaderFactory.CreateShaderProgram("mesh", &programId)) {
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
	for (const glm::vec3 vertex : vertices) {
		positionVbo.vertices.push_back(vertex);
		normalVbo.vertices.push_back(normals[counter / 3]);
		++counter;
	}

	positionVbo.TransferToOpenGl();
	normalVbo.TransferToOpenGl();
	return true;
}

void MeshRenderer::UpdateMeshMatrix(glm::mat4& updatedMeshMatrix) {
	meshMatrix = updatedMeshMatrix;
}

void MeshRenderer::Update(float currentTime, float lastFrameTime, const Camera& camera) {
	glUseProgram(programId);
	glUniformMatrix4fv(meshMatrixLocation, 1, GL_FALSE, &meshMatrix[0][0]);
}

void MeshRenderer::Render(float currentTime, const glm::mat4& projectionMatrix) {
	glUseProgram(programId);
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, positionVbo.vertices.size());
}
