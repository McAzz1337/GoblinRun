#include "mesh.h"

Mesh::Mesh() {}

Mesh::Mesh(VertexArray vao, IndexBuffer ibo) : vao(vao), ibo(ibo) {}


Mesh::Mesh(VertexBuffer vbo, IndexBuffer ibo) : vao(VertexArray(vbo)) {}

Mesh::Mesh(const Mesh& other): vao(other.vao), ibo(other.ibo), mat(other.mat) {

	}


Mesh::~Mesh() {}

void Mesh::assignVertexArray(VertexArray vao, IndexBuffer ibo) {
	this->vao = vao;
	this->ibo = ibo;
}

void Mesh::assignBuffers(VertexBuffer vbo, IndexBuffer ibo) {
	vao.assignBuffer(vbo);
	this->ibo = ibo;
}


void Mesh::assignMaterial(Material mat) {
	this->mat = mat;
}

void Mesh::bind(const glm::mat4& projectionView) const {
	vao.bind();
	ibo.bind();
	mat.bind(projectionView);
}

VertexBuffer& Mesh::getVbo() {
	return vao.getVbo();
}

IndexBuffer& Mesh::getIbo() {
	return ibo;
}
