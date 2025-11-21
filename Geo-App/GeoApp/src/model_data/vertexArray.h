#pragma once

#include"buffer.h"

class VertexArray {
	uint32_t handle;
	VertexBuffer vbo;

public:
	VertexArray();
	VertexArray(const VertexArray& other);
	VertexArray(VertexBuffer vbo);
	~VertexArray();

	void deallocate();

	void enableAttribPointers();
	void assignBuffer(VertexBuffer vbo);
	void bind() const;

	inline VertexBuffer& getVbo() {
		return vbo;
	}


};

