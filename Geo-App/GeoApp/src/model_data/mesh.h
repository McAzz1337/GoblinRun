#pragma once
#include "vertexArray.h"
#include "texture.h"
#include  "../shader.h"
#include "material.h"


class Mesh {

	VertexArray vao;
	IndexBuffer ibo;
	Material mat;


public:
	Mesh();
	Mesh(VertexArray vao, IndexBuffer ibo);
	Mesh(VertexBuffer vbo, IndexBuffer ibo);
	Mesh(const Mesh& other);
	~Mesh();

	void assignVertexArray(VertexArray vao, IndexBuffer ibo);
	void assignBuffers(VertexBuffer vbo, IndexBuffer ibo);
	void assignMaterial(Material mat);;

	void bind(const glm::mat4& projectionView) const;

	inline Material& getMaterial() { return mat;  }
	VertexBuffer& getVbo();
	IndexBuffer& getIbo();
};