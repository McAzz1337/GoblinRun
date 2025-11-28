#pragma once
#include <GL/glew.h>
#include <memory.h>
#include <iostream>

#include "vertex.h"


template <typename T, int BUFFER_TYPE>
class Buffer {
	uint32_t handle;
	size_t size;

public:
	Buffer() : size(0) {
		glGenBuffers(1, &handle);
		bind();
	}

	Buffer(const Buffer& other) : handle(other.handle), size(other.size) {}

	Buffer(const T* data, size_t size) {
		glGenBuffers(1, &handle);
		bind();
		bufferData(data, size);
	}

	void deallocte() {
		if (handle != 0) {
			glDeleteBuffers(1, &handle);
			handle = 0;
		}
	}

	void bind() const { glBindBuffer(BUFFER_TYPE, handle); }

	void bufferData(const T* data, size_t size) {
		this->size = size;
		glBufferData(BUFFER_TYPE, size  * sizeof(T), data, GL_STATIC_DRAW);
	}

	size_t getSize() const { return size; }
};

typedef Buffer<Vertex, GL_ARRAY_BUFFER> VertexBuffer;
typedef Buffer<uint32_t, GL_ELEMENT_ARRAY_BUFFER> IndexBuffer;
