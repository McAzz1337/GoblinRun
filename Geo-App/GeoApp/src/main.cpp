#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include "model_data/vertex.h"
#include "model_data/material.h"
#include "model_data/buffer.h"
#include "model_data/mesh.h"
#include "gldebug.h"
#include <numbers>
#include <chrono>

int main(int argc, const char** argv) {

	for (int i = 0; i < argc; i++) {
		printf("Argument[%i] = %s\n", i, argv[i]);
	}

	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW" << std::endl;
	}

	GLFWwindow* window = glfwCreateWindow(1440, 1080, "GeoData", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	if (glewInit()) {
		std::cout << "Failed to initialize GLEW" << std::endl;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::perspective((float)std::numbers::pi * 0.3f, 1440.0f / 1080.0f, 0.01f, 100.0f);
	
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	Material mat;
	assetimporter::init();
	assetimporter::loadModel("models", "res.obj", vertices, indices, mat);
	VertexBuffer vbo(vertices.data(), vertices.size());
	IndexBuffer ibo(indices.data(), indices.size());
	Shader shader("shaders/shader");
	mat.assignShader(shader);
	Mesh mesh;
	mesh.assignMaterial(mat);
	mesh.assignBuffers(vbo, ibo);

	std::chrono::time_point last = std::chrono::steady_clock::now();
	float elapsed = 0.0f;


	float rotationAngle = (float)std::numbers::pi * 0.01;

	while (!glfwWindowShouldClose(window)) {
		std::chrono::time_point now = std::chrono::steady_clock::now();
		float delta = std::chrono::duration<float, std::milli>(now - last).count() / 1000.0f;
		elapsed += delta;
		delta /= 100.0f;


		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			view = glm::translate(view, glm::vec3(0.0, 0.0, 1.0f) * delta);
		} else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			view = glm::translate(view, glm::vec3(0.0, 0.0, -1.0f * delta));
		} else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			view = glm::translate(view, glm::vec3(1.0, 0.0, .0f * delta));
		} else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			view = glm::translate(view, glm::vec3(-1.0, 0.0, .0f * delta));
		}  else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			model = glm::rotate(model, rotationAngle, glm::vec3(0.0, 1.0, 0.0f * delta));
		} else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			model = glm::rotate(model, -rotationAngle, glm::vec3(0.0, 1.0, 0.0f * delta));
		}
		else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			model = glm::rotate(model, rotationAngle, glm::vec3(1.0, 0.0, 0.0f * delta));
		} 
		else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			model = glm::rotate(model, -rotationAngle, glm::vec3(1.0, 0.0, 0.0f * delta));
		}

		mesh.bind(projection * view * model);
		GL_CALL(glDrawElements(GL_TRIANGLES, mesh.getIbo().getSize(), GL_UNSIGNED_INT, nullptr));

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}