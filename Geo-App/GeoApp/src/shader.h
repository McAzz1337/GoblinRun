#pragma once


#include <functional>
#include <unordered_map>
#include <string>

#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>



class Shader {


private:
	uint32_t id = 0;
	std::string file;
	mutable std::unordered_map<const char*, int> uniforms;

public:
	Shader();
	Shader(uint32_t id);
	Shader(const std::string& file);
	Shader(const Shader& other);
	~Shader();

	void load(const std::string& file);
	void reloadFromDisk();
	void free();

	void bind() const;

	int getUniform(const char* name) const;

	void setUniformf1(const char* name, float x) const;
	void setUniformf2(const char* name, float x, float y) const;
	void setUniformf2(const char* name, const glm::vec2& v) const;
	void setUniformf3(const char* name, float x, float y, float z) const;
	void setUniformf3(const char* name, const glm::vec3& v) const;
	void setUniformf4(const char* name, float x, float y, float z, float w) const;
	void setUniformf4(const char* name, const glm::vec4& v) const;

	void setUniformi1(const char* name, int x) const;
	void setUiformi1v(const char* name, int* x, int count) const;


	void setMatrix3(const char* name, const glm::mat3& m) const;
	void setMatrix4(const char* name, const glm::mat4& m) const;


	static Shader* createShader(const std::string& file);
	static void unbind();

	inline const std::string& getFilePath() const { return file; }

	inline operator std::function<void()>() {

		return [this]() {

			this->free();
			};
	}


private:
	static void readShaderFile(const char* file, std::string& dst);
	static bool compileShader(uint32_t& id, int type, const std::string& src, bool crash = false);
	static bool linkShader(uint32_t& program, int vs, int fs, bool crash = false);
};