#pragma once

#include <sstream>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>


#define GL_ASSERT(x) if (!x) { __debugbreak(); }

#define GL_CALL(x)  clearError(); x; GL_ASSERT(checkError(#x, __FILE__, __LINE__))

#define GL_ASSERT_EXISTENCE(x, y) if (x == 0) { notifyNonExistent(#y, __FILE__, __LINE__); __debugbreak(); } GL_CALL(y)

static void glCheck(const char* func, const char* file, int line) {
	std::stringstream ss;
	ss << "-------------------------------------" << std::endl;
	ss << "Call to OpenGl after glfwTerminate" << std::endl;
	ss << "function:\t" << func << std::endl;
	ss << "file:\t" << file << std::endl;
	ss << "line:\t" << line << std::endl;
	ss << "-------------------------------------" << std::endl;

	std::cout << ss.str();
	__debugbreak();
}

static void clearError() {
	while (glGetError() != GL_NO_ERROR) {}
}

static bool checkError(const char* func, const char* file, int line) {
	bool noError = true;
	int err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::stringstream ss;
		ss << "-------------------------------------" << std::endl;
		ss << "GL_ERROR[" << std::hex << err << "]" << std::endl;
		ss << "function:\t" << func << std::endl;
		ss << std::dec;
		ss << "file:\t" << file << std::endl;
		ss << "line:\t" << line << std::endl;
		ss << "error string: " << glewGetErrorString(err) << std::endl;
		ss << "-------------------------------------" << std::endl;
		std::cout << ss.str();
		noError = false;
	}
	return noError;
}

static std::string getObjectType(const char* src) {
	std::string func = src;
	auto contains = [&func](const char* c) -> bool {
		return func.find(c) != std::string::npos;
		};

	if (contains("Buffer")) {

		if (contains("GL_ARRAY_BUFFER")) {

			return "Vertex Buffer";
		} else if (contains("GL_ELEMENT_ARRAY_BUFFER")) {

			return "Index Buffer";
		}
	} else if (contains("VertexArray")) {

		return "VertexArray";
	} else if (contains("Program")) {

		return "Shader";
	} else if (contains("Texture")) {

		return "Texture";
	}

	return "Uninplemented!";
}

static void notifyNonExistent(const char* func, const char* file, int line) {

	std::stringstream ss;
	ss << "-------------------------------------" << std::endl;
	ss << "Usage of non existent " << getObjectType(func) << std::endl;
	ss << "function:\t" << func << std::endl;
	ss << "file:\t" << file << std::endl;
	ss << "line:\t" << line << std::endl;
	ss << "-------------------------------------" << std::endl;

	std::cout << ss.str();
}
