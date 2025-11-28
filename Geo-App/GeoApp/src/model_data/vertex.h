#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>


struct Vertex {
	glm::vec3 pos;
	glm::vec2 uv;
	glm::vec3 norm;

public:
	Vertex(glm::vec3 pos, glm::vec2 uv, glm::vec3 norm);
	~Vertex();


};