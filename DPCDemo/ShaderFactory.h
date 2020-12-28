#pragma once
#include <string>
#include <vector>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wignored-attributes"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#pragma clang diagnostic pop

// Handles loading and deleting shaders from a common 'shaders' folder
class ShaderFactory
{
    bool LoadStringFromFile(const std::string& filename, std::string& result);

    bool ReadShader(const char* rootName, const char* extension, std::string* readShader);
    bool CreateShader(GLenum shaderType, const char* shaderSource, GLuint* shaderId);
    bool CreateProgram(std::vector<GLuint> shaders, GLuint* program);

    std::vector<GLuint> shaderPrograms;

public:
    ShaderFactory();
    bool CreateShaderProgram(const char* rootName, GLuint* programId);
    void DeleteShaderProgram(GLuint programId);
    ~ShaderFactory();
};

