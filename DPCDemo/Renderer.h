#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wignored-attributes"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#pragma clang diagnostic pop
#include <glm\mat4x4.hpp>
#include "FpsCounter.h"
#include "ImguiRenderer.h"
#include "IRenderable.h"
#include "OpenGl.h"
#include "ShaderFactory.h"
#include "Viewer.h"

class Renderer
{
	FpsCounter fpsCounter;
	ImguiRenderer guiRenderer;
	OpenGl opengl;
	ShaderFactory shaderFactory;
	Viewer viewer;

	std::vector<IRenderable*> updateOrder;
	std::vector<IRenderable*> renderOrder;

	void update(float currentTime, float frameTime);
	void render(float currentTime, glm::mat4& viewMatrix);

public:
	Renderer();
	bool Init();
	void Teardown();
	
	void Run();
};

