#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wignored-attributes"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#pragma clang diagnostic pop
#include <glm\mat4x4.hpp>
#include "Axis.h"
#include "FpsCounter.h"
#include "ImguiRenderer.h"
#include "IRenderable.h"
#include "OpenGl.h"
#include "ParticleRenderer.h"
#include "ShaderFactory.h"
#include "Viewer.h"

class Renderer
{
	ImguiRenderer guiRenderer;
	OpenGl opengl;
	ShaderFactory shaderFactory;
	Axis axis;
	ParticleRenderer particleRenderer;
	FpsCounter fpsCounter;
	Viewer viewer;

	std::vector<IRenderable*> updateOrder;
	std::vector<IRenderable*> renderOrder;

	void update(float currentTime, float frameTime);
	void render(float currentTime, glm::mat4& viewMatrix);

	std::atomic<bool>* shouldReadUpdate;
	std::atomic<bool>* hasReadUpdate;
	std::vector<glm::vec3>* particlePositions;

public:
	Renderer(std::atomic<bool>* shouldReadUpdate, std::atomic<bool>* hasReadUpdate,
		std::vector<glm::vec3>* particlePositions);
	bool Init();
	void Teardown();
	
	void Run();
};

