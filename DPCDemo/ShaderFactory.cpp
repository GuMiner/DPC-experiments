#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include "ShaderFactory.h"

ShaderFactory::ShaderFactory()
    : shaderPrograms()
{
}

bool ShaderFactory::LoadStringFromFile(const std::string& filename, std::string& result)
{
    std::ifstream file(filename.c_str());
    if (!file)
    {
        return false;
    }

    std::stringstream stream;
    stream << file.rdbuf();
    result = stream.str();

    file.close();
    return true;
}

bool ShaderFactory::ReadShader(const char* rootName, const char* extension, std::string* readShader)
{
    std::stringstream filenameStream;
    filenameStream << "shaders/" << rootName << extension;

    if (!LoadStringFromFile(filenameStream.str().c_str(), *readShader))
    {
        std::cout << "Could not load " << extension << " shader: " << *readShader << "!" << std::endl;
        return false;
    }

    return true;
}

// Creates and compiles a new shader of the specified type; returns true on success.
bool ShaderFactory::CreateShader(GLenum shaderType, const char* shaderSource, GLuint* shaderId)
{
    GLint compileStatus;
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    if (!compileStatus)
    {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

        std::stringstream logStream;
        char* shaderLog = new char[logLength];
        glGetShaderInfoLog(shader, logLength, &logLength, &shaderLog[0]);
        logStream << "GLSL compilation error: " << shaderLog << ".";
        std::cout << logStream.str().c_str() << std::endl;

        delete[] shaderLog;
        return false;
    }

    *shaderId = shader;
    return true;
}

// Creates a new shader program, regardless of the number of shader stages. Stages are added in order of specification in the vector.
bool ShaderFactory::CreateProgram(std::vector<GLuint> shaders, GLuint* program)
{
    // Create the program
    GLint compileStatus;
    *program = glCreateProgram();
    for (unsigned int i = 0; i < shaders.size(); i++)
    {
        glAttachShader(*program, shaders[i]);
    }

    glLinkProgram(*program);
    glGetProgramiv(*program, GL_LINK_STATUS, &compileStatus);
    if (!compileStatus)
    {
        GLint logLength;
        glGetProgramiv(*program, GL_INFO_LOG_LENGTH, &logLength);

        char* buffer = new char[logLength];
        glGetProgramInfoLog(*program, logLength, &logLength, buffer);
        std::cout << "GLSL program compilation error: " << buffer << std::endl;
        delete[] buffer;

        return false;
    }

    // These are auto-deleted when the program is deleted
    for (unsigned int i = 0; i < shaders.size(); i++)
    {
        glDeleteShader(shaders[i]);
    }

    return true;
}

// Creates a basic vertex-fragment shader program and adds it to the list of programs that will be deleted at the end of program operation
bool ShaderFactory::CreateShaderProgram(const char* rootName, GLuint* programId)
{
    std::string vsShader, fsShader;
    if (!ReadShader(rootName, ".vs", &vsShader) || !ReadShader(rootName, ".fs", &fsShader))
    {
        return false;
    }

    GLuint vertexShader, fragmentShader;
    if (!CreateShader(GL_VERTEX_SHADER, vsShader.c_str(), &vertexShader) || !CreateShader(GL_FRAGMENT_SHADER, fsShader.c_str(), &fragmentShader))
    {
        return false;
    }

    std::vector<GLuint> shaders;
    shaders.push_back(vertexShader);
    shaders.push_back(fragmentShader);

    if (!CreateProgram(shaders, programId))
    {
        return false;
    }

    shaderPrograms.push_back(*programId);
    return true;
}

// Deletes the specified shader program, removing it from the list of known programs.
void ShaderFactory::DeleteShaderProgram(GLuint program)
{
    glDeleteProgram(program);
    for (unsigned int i = 0; i < shaderPrograms.size(); i++)
    {
        if (shaderPrograms[i] == program)
        {
            shaderPrograms.erase(shaderPrograms.begin() + i);
            break;
        }
    }
}

ShaderFactory::~ShaderFactory()
{
    for (unsigned int i = 0; i < shaderPrograms.size(); i++)
    {
        glDeleteProgram(shaderPrograms[i]);
    }

    shaderPrograms.clear();
}
