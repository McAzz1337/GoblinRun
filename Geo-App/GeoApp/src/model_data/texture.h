#pragma once

#include "glm/vec2.hpp"
#include <string>


class Texture {

protected:
	std::string file;
	uint32_t id = 0;
	int w = 0;
	int h = 0;

public:
	Texture();
	Texture(const Texture& other);
	Texture(const std::string& path);
	virtual ~Texture();

	void unload();

	void load(const std::string& path);
	virtual void bind(int index = 0) const;

	inline const std::string& getFilePath() const { return file; }
	inline uint32_t getId() const { return id; }
	inline glm::vec2 getSize() const { return glm::vec2((float) w, (float) h); }

	static Texture* loadFromFile(const std::string& path);

	static Texture createEmptyTexture(int width, int height);
	static Texture createTextureFromData(const unsigned char* data, int width, int height);
	static void flipImage(const unsigned char* src, int width, int height, int comp, unsigned char* dst);
};



