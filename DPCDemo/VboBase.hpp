#pragma once
#include <vector>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wignored-attributes"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#pragma clang diagnostic pop

// Defines a base vertex type for VBOs. 
template<typename T>
class VboBase {
public:
    std::vector<T> vertices;

private:
    GLuint buffer;
    GLuint layoutPosition;

protected:

    // Child classes typically call TransferToOpenGl by specifying the item count and usage best for that type.
    void SendToOpenGl(GLuint itemCount, GLenum usage) {
        glEnableVertexAttribArray(layoutPosition);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glVertexAttribPointer(layoutPosition, itemCount, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), &vertices[0], usage);
    }

    // Child classes determine the layout position.
    void InitializeToLocation(GLuint layoutPosition) {
        glGenBuffers(1, &buffer);
        this->layoutPosition = layoutPosition;
    }

public:
    virtual void SetupOpenGlBuffers() = 0;
    virtual void TransferToOpenGl() = 0;

    void Deinitialize() {
        glDeleteBuffers(1, &buffer);
    }
};