#include "shader.h"

#include <ios>
#include <iostream>
#include <fstream>
#include <filesystem>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "gldebug.h"

#include "io/fileio.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

const std::string VS_EXT = ".vert";
const std::string GS_EXT = ".geo";
const std::string FS_EXT = ".frag";


Shader::Shader() {

}


Shader::Shader(uint32_t id) : id(id), file(file), uniforms(uniforms) {

}

Shader::Shader(const std::string& file) {
	load(file);
}

Shader::Shader(const Shader& other) : id(other.id), file(other.file), uniforms(other.uniforms) {

}

Shader::~Shader() {}

void Shader::load(const std::string& file) {

	this->file = file;
	std::string fileName = "";
	extractFileName(file, fileName);
	std::string vsrc;
	std::string fsrc;

	readFile((file + VS_EXT).c_str(), vsrc, true);
	readFile((file + FS_EXT).c_str(), fsrc, true);

	printf("%s Vertex Shader source:\n %s\n\n", fileName.c_str(), vsrc.c_str());
	printf("%s Fragment Shader source:\n %s\n\n", fileName.c_str(), fsrc.c_str());

	uint32_t vs;
	uint32_t fs;

	compileShader(vs, GL_VERTEX_SHADER, vsrc, true);
	compileShader(fs, GL_FRAGMENT_SHADER, fsrc, true);

	linkShader(id, vs, fs);

	glDetachShader(id, vs);
	glDetachShader(id, fs);

	glDeleteShader(vs);
	glDeleteShader(fs);

	setUniformi1("tex", 0);
}

void Shader::reloadFromDisk() {
	printf("Reloading file: %s\n", file.c_str());
	load(file);
}

void Shader::free() {

	glDeleteProgram(id);
}

void Shader::bind() const {

	GL_ASSERT_EXISTENCE(id, glUseProgram(id));
}

int Shader::getUniform(const char* name) const {
	bind();

	if (uniforms.contains(name))
		return uniforms[name];

	int uniform = glGetUniformLocation(id, name);

	if (uniform == -1) printf("can not find uniform %s\n", name);

	uniforms[name] = uniform;

	return uniform;
}


void Shader::setUniformf1(const char* name, float x) const {

	int uniform = getUniform(name);
	glUniform1f(uniform, x);
}

void Shader::setUniformf2(const char* name, float x, float y) const {

	int uniform = getUniform(name);
	glUniform2f(uniform, x, y);
}

void Shader::setUniformf2(const char* name, const glm::vec2& v) const {

	int uniform = getUniform(name);
	glUniform2f(uniform, v.x, v.y);
}

void Shader::setUniformf3(const char* name, float x, float y, float z) const {

	int uniform = getUniform(name);
	glUniform3f(uniform, x, y, z);

}

void Shader::setUniformf3(const char* name, const glm::vec3& v) const {

	int uniform = getUniform(name);
	glUniform3f(uniform, v.x, v.y, v.z);

}

void Shader::setUniformf4(const char* name, float x, float y, float z, float w) const {

	int uniform = getUniform(name);
	glUniform4f(uniform, x, y, z, w);
}

void Shader::setUniformf4(const char* name, const glm::vec4& v) const {

	int uniform = getUniform(name);
	glUniform4f(uniform, v.x, v.y, v.z, v.w);
}

void Shader::setUniformi1(const char* name, int x) const {

	int uniform = getUniform(name);
	glUniform1i(uniform, x);
}

void Shader::setUiformi1v(const char* name, int* x, int count) const {

	int uniform = getUniform(name);
	glUniform1iv(uniform, count, x);
}




void Shader::setMatrix3(const char* name, const glm::mat3& m) const {
	int uniform = getUniform(name);
	glUniformMatrix3fv(uniform, 1, GL_FALSE, &m[0][0]);
}


void Shader::setMatrix4(const char* name, const glm::mat4& m) const {

	int uniform = getUniform(name);
	glUniformMatrix4fv(uniform, 1, GL_FALSE, &m[0][0]);
}

Shader* Shader::createShader(const std::string& file) {

	uint32_t id = 0;

	std::string vsrc;
	std::string fsrc;

	readShaderFile((file + VS_EXT).c_str(), vsrc);
	readShaderFile((file + FS_EXT).c_str(), fsrc);

	uint32_t vs;
	uint32_t fs;

	if (!compileShader(vs, GL_VERTEX_SHADER, vsrc)) return nullptr;
	if (!compileShader(fs, GL_FRAGMENT_SHADER, fsrc)) return nullptr;

	if (!linkShader(id, vs, fs)) return nullptr;

	glDetachShader(id, vs);
	glDetachShader(id, fs);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return new Shader(id);
}

void Shader::unbind() {

	glUseProgram(0);
}



void Shader::readShaderFile(const char* file, std::string& dst) {

	std::ifstream in(file);

	if (!in) {

		dst = "N/A";
		return;
	}

	std::string line;

	while (getline(in, line))
		dst += line + "\n";

	in.close();
}

bool Shader::compileShader(uint32_t& id, int type, const std::string& src, bool crash) {

	id = glCreateShader(type);
	const char* temp = src.c_str();
	glShaderSource(id, 1, &temp, NULL);
	glCompileShader(id);

	int isCompiled = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE) {
		int maxLength = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

		char* infoLog = new char[maxLength];
		int length = 0;
		glGetShaderInfoLog(id, maxLength, &length, infoLog);

		glDeleteShader(id);
		switch (type) {
			case GL_VERTEX_SHADER:		std::cout << "Vertex ";		break;
			case GL_GEOMETRY_SHADER:	std::cout << "Geometry ";	break;
			case GL_FRAGMENT_SHADER:	std::cout << "Fragment ";	break;

		}
		std::cout << "shader compilation failure!" << std::endl << infoLog << std::endl;
		if (crash)
			__debugbreak();

		return false;
	}

	return true;
}

bool Shader::linkShader(uint32_t& program, int vs, int fs, bool crash) {

	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);
	int isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE) {
		int maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
                                              
		char* infoLog = new char[maxLength];
		int length = 0;
		glGetProgramInfoLog(program, maxLength, &length, infoLog);
                                       
		glDeleteProgram(program);
                               
		glDeleteShader(vs);
		glDeleteShader(fs);
		std::cout << "Shader link failure!" << std::endl;
		std::cout << infoLog << std::endl;
		if (crash)
			__debugbreak();

		return false;
	}

	glDeleteShader(vs);
	glDeleteShader(fs);

	return true;
}

