#include "vertexArray.h"
#include "buffer.h"
#include <GL/glew.h>

VertexArray::VertexArray() {
	glGenVertexArrays(1, &handle);
}
VertexArray::VertexArray(const VertexArray& other) : handle(other.handle), vbo(other.vbo) {

}

VertexArray::VertexArray(VertexBuffer vbo) {

	glGenVertexArrays(1, &handle);
	assignBuffer(vbo);

	enableAttribPointers();
}

VertexArray::~VertexArray() {

}

void VertexArray::deallocate() {
	if (handle != 0) {
		glDeleteVertexArrays(1, &handle);
		handle = 0;
	}
}

void VertexArray::assignBuffer(VertexBuffer vbo) {
	bind();
	vbo.bind();
	this->vbo = vbo;
	enableAttribPointers();
}

void VertexArray::enableAttribPointers() {
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*) 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*) offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*) offsetof(Vertex, norm));
}

void VertexArray::bind() const {
	glBindVertexArray(handle);
}

