#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include "model_data/vertex.h"
#include "model_data/material.h"
#include "model_data/buffer.h"
#include "model_data/mesh.h"
#include "model_data/texture.h"
#include "model_data/util.h"
#include "config.h"
#include "coordinateConversion.h"
#include "gldebug.h"
#include "io/fileio.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <sstream>
#include <numbers>
#include <chrono>
#include <algorithm>
#include <functional>
#include <limits>

std::mutex gpsMutex;
float latestLat = 0.0f;
float latestLon = 0.0f;
std::atomic<bool> udpRunning{ true };


void startUDPReceiver(int port = 5005) {
	std::thread([port]() {
		SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (sock == INVALID_SOCKET) {
			std::cerr << "socket failed\n";
			return;
		}

		sockaddr_in serverAddr{};
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(port);
		serverAddr.sin_addr.s_addr = INADDR_ANY;

		if (bind(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
			std::cerr << "bind failed\n";
			closesocket(sock);
			return;
		}

		char buffer[128];
		std::cout << "UDP Receiver running on port " << port << std::endl;

		while (udpRunning) {
			int len = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, nullptr, nullptr);
			printf("recv\n");
			if (len > 0) {
				buffer[len] = '\0';
				std::istringstream iss(buffer);
				float lat, lon;
				if (iss >> lat) {
					if (iss.peek() == ',') iss.ignore();
					if (iss >> lon) {
						std::lock_guard<std::mutex> lock(gpsMutex);
						latestLat = lat;
						latestLon = lon;
						std::cout << "Received GPS: " << latestLat << "," << latestLon << std::endl;
					}
				}
			}
		}

		closesocket(sock);
		}).detach();
}



glm::vec2 mouse;
glm::vec2 mouseDelta;
float yaw = -90.0f;
float pitch = 0.0f;
glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::mat4 view = glm::mat4(1.0f);

float speedModifier = 1.0f;

void updateCamera() {
	pitch = glm::clamp(pitch, -89.0f, 89.0f);
	glm::vec3 forward;
	forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	forward.y = sin(glm::radians(pitch));
	forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	forward = glm::normalize(forward);

	glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
	glm::vec3 up = glm::normalize(glm::cross(right, forward));

	view = glm::lookAt(camPos, camPos + forward, up);
}

void cursorCallback(GLFWwindow* window, double x, double y) {
	glm::vec3 position = glm::vec3(view[3][0], view[3][1], view[3][2]);
	glm::vec2 newPos = glm::vec2((float)x, (float)y);

	glm::vec2 delta = newPos - mouse;
	mouse = newPos;

	yaw += delta.x * 0.1f;
	pitch -= delta.y * 0.1f;
}

int main(int argc, const char** argv) {

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup failed\n";
		return 1;
	}
	startUDPReceiver(5005);
#pragma region INIT_OPEN_GL
	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW" << std::endl;
	}

	GLFWwindow* window = glfwCreateWindow(1440, 1080, "GeoData", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	if (glewInit()) {
		std::cout << "Failed to initialize GLEW" << std::endl;
	}


	{
		double x;
		double y;
		glfwGetCursorPos(window, &x, &y);
		mouse.x = (float)x;
		mouse.y = (float)y;
	}
	glfwSetCursorPosCallback(window, cursorCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
#pragma endregion

	const Config config;
#if 0
	printf("mapfile: %s\n", config.mapfileName.c_str());
	printf("textureFile: %s\n", config.textureFileName.c_str());
	printf("xmin: %f\n", config.xmin);
	printf("xmax: %f\n", config.xmax);
	printf("ymin: %f\n", config.ymin);
	printf("ymax: %f\n", config.ymax);
#endif

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 projection = glm::perspective((float)std::numbers::pi * 0.3f, 1440.0f / 1080.0f, 0.01f, 100.0f);
	
#pragma region INIT_MESHES
	Mesh mesh;
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		Material mat;
		assetimporter::loadModel("models", config.mapfileName, vertices, indices, mat);
		{
			glm::mat4 rot = glm::rotate(glm::mat4(1.0f), (float)std::numbers::pi * -0.5f, { 1.0f, 0.0f, 0.0f });
			MinMaxXY m = getMinMaxXY(vertices);
			m.maxX += abs(m.minX);
			m.maxY += abs(m.minY);
			for (auto& v : vertices) {
				v.pos = glm::vec3(rot * glm::vec4(v.pos, 1.0f));
				glm::vec3 p = v.pos;
				p.x += m.minX;
				p.z += m.minY;
				float r = p.x / m.maxX;
				float t = p.z / m.maxY;
				v.uv = glm::vec2(r,  1.0f - t);
				v.pos.y *= 1.5f;
				v.norm = r * v.norm;
			}
		}
		VertexBuffer vbo(vertices.data(), vertices.size());
		IndexBuffer ibo(indices.data(), indices.size());
		Texture tex("textures/" + config.textureFileName);
		Shader shader("shaders/shader");
		mat.assignTex(tex);
		mat.assignShader(shader);
		mesh.assignMaterial(mat);
		mesh.assignBuffers(vbo, ibo);
	}
	Mesh pin;
	glm::mat4 pinModel = glm::mat4(1.0f);
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		Material mat;
		assetimporter::loadModel("models", "pin.obj", vertices, indices, mat);
		VertexBuffer vbo(vertices.data(), vertices.size());
		IndexBuffer ibo(indices.data(), indices.size());
		pin.assignBuffers(vbo, ibo);
		Shader shader("shaders/shader");
		mat.assignShader(shader);
		mat.addUniformI("isPin", 1);
		mat.addUniformF3("color", { 1.0f, 0.0f, 0.0f });
		pin.assignMaterial(mat);
	}
#pragma endregion

	std::chrono::time_point last = std::chrono::steady_clock::now();
	float elapsed = 0.0f;


	float pinAngle = 0.0f;
	float pinRotationSpeed = (float)std::numbers::pi * 0.3f;

	while (!glfwWindowShouldClose(window)) {
		std::chrono::time_point now = std::chrono::steady_clock::now();
		float delta = std::chrono::duration<float, std::milli>(now - last).count() / 1000.0f;
		elapsed += delta;
		last = now;


		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		updateCamera(); 

#pragma region CONTROLS
		glm::vec3 forward;
		forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		forward.y = sin(glm::radians(pitch));
		forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		forward = glm::normalize(forward);
		glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, 1);
		} else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camPos += forward * delta * speedModifier;
		} else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camPos -= forward * delta * speedModifier;
		} else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camPos -= right * delta * speedModifier;
		} else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camPos += right * delta * speedModifier;
		} else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			camPos += glm::vec3(0.0f, 1.0f, 0.0f) * delta * speedModifier;
		} else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
			camPos += glm::vec3(0.0f, -1.0f, 0.0f) * delta * speedModifier;
		} else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			speedModifier = 0.1f;
		} else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
			speedModifier = 1.0f;
		}
#pragma endregion

		updateCamera();

#pragma region COORDINATES_AND_PIN
		float lat, lon;
		{
			std::lock_guard<std::mutex> lock(gpsMutex);
			lat = latestLat;
			lon = latestLon;
		}

		glm::vec3 pinPos = wgs84ToGL(lat, lon, config.xmin, config.xmax, config.ymin, config.ymax);

#if 0 
		printf("pin: %f, %f, %f\n", pinPos.x, pinPos.y, pinPos.z); 
#endif
		pinAngle += pinRotationSpeed * delta;
		pinAngle = fmod(pinAngle, 2.0f * std::numbers::pi_v<float>);
		pinModel = glm::translate(glm::mat4(1.0f), pinPos);
		pinModel = glm::rotate(pinModel, pinAngle, {0.0f, 1.0f, 0.0f});
#pragma endregion

#pragma region RENDERING
		glm::vec3 cameraPos = glm::vec3(view[3][0], view[3][1], view[3][2]);
		{
			mesh.bind(projection * view * model);
			const Shader& shader = mesh.getMaterial().getShader();
			shader.setUniformf3("cameraPos", cameraPos);
			shader.setMatrix3("view", glm::mat3(view));
			shader.setMatrix4("model", model);
			GL_CALL(glDrawElements(GL_TRIANGLES, mesh.getIbo().getSize(), GL_UNSIGNED_INT, nullptr));
		}
		{
			pin.bind(projection* view * pinModel);
			const Shader& shader = pin.getMaterial().getShader();
			shader.setUniformf3("cameraPos", cameraPos);
			shader.setMatrix3("view", glm::mat3(view));
			shader.setMatrix4("model", model);
			GL_CALL(glDrawElements(GL_TRIANGLES, pin.getIbo().getSize(), GL_UNSIGNED_INT, nullptr));

		} 
#pragma endregion

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	udpRunning = false;
	WSACleanup();
	return 0;
}