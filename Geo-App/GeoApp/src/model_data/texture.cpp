#include "texture.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "gldebug.h"
#include <unordered_map>

#include "stb/stb_image.h"

#include <filesystem>


std::unordered_map<std::string, Texture> textureCache;



Texture::Texture() {}

Texture::Texture(const Texture& other) : id(other.id), w(other.w), h(other.h), file(other.file) {

}

Texture::Texture(const std::string& path) : file(path) {
	load(path);
}

Texture::~Texture() {}

void Texture::unload() {
	if (id != 0) {
		glDeleteTextures(1, &id);
	}
}

void Texture::load(const std::string& path) {
	int comp = 0;
	unsigned char* flipped = nullptr;
	std::filesystem::path absPath = std::filesystem::absolute(path);
	std::cout << "Attempting to load texture from: " << absPath << std::endl;

	// Check if file exists
	if (!std::filesystem::exists(absPath)) {
		std::cerr << "File does NOT exist!" << std::endl;
	} else {
		std::cout << "File exists." << std::endl;
	}

	{
		unsigned char* data = nullptr;
		data = stbi_load(absPath.generic_string().c_str(), &w, &h, &comp, STBI_rgb_alpha);

		assert(data);
		//printf("path: %s\n", path.c_str());
		//printf("data: %s\n", data);
		//printf("comp: %i\n", comp);
		flipped = new unsigned char[w * comp * h];
		flipImage(data, w, h, comp, flipped);
		stbi_image_free(data);
	}

	GL_CALL(glGenTextures(1, &id));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, id));

	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

	GL_CALL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL_CALL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, flipped));

	//stbi_image_free(data);
	delete[] flipped;

}

void Texture::bind(int index) const {
	if (id != 0) {
		GL_CALL(glActiveTexture(GL_TEXTURE0 + index));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, id));
		//printf("texture bound at index : %i\n", index);
	}
}

Texture Texture::createEmptyTexture(int width, int height) {
	Texture tex;
	tex.w = width;
	tex.h = height;

	int length = height * width * 4;
	unsigned char* data = new unsigned char[length];
	for (int i = 0; i < length; i++) {
		data[i] = 0;
	}

	glGenTextures(1, &tex.id);
	glBindTexture(GL_TEXTURE_2D, tex.id);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));

	delete[] data;

	return tex;
}

Texture* Texture::loadFromFile(const std::string& path) {
	auto it = textureCache.find(path);
	if (it != textureCache.end()) {
		return &it->second;
	} else {
		Texture t(path);
		textureCache.emplace(std::make_pair(path, t));
		return &textureCache[path];
	}
}


Texture Texture::createTextureFromData(const unsigned char* data, int width, int height) {

	Texture tex;

	tex.w = width;
	tex.h = height;

	GL_CALL(glGenTextures(1, &tex.id));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, tex.id));


	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

	GL_CALL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL_CALL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));


	GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));

	return tex;
}

void Texture::flipImage(const unsigned char* src, int width, int height, int comp, unsigned char* dst) {
	int rowSize = width * comp;
	int lastLine = rowSize * (height - 1);

	for (int y = 0; y < height; y++) {
		int srcI = y * rowSize;
		int dstI = lastLine - y * rowSize;
		memcpy(&dst[dstI], &src[srcI], rowSize);
	}
}
