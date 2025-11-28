#include "fileio.h"
#include <ios>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <functional>
#include <cassert>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "../model_data/vertex.h"



void readFile(const std::string& path, std::string& dst, bool log) {

	std::ifstream in(path);

	if (!in.is_open()) {

		if (log)
			printf("failed to open file: %s\n", path.c_str());

		dst = "";
		return;
	}

	std::string line;
	while (getline(in, line)) {
		dst += line + "\n";
	}

	in.close();
}

void readFileSplit(const std::string& path, std::vector<std::string>& dst, bool log) {

	std::ifstream in(path);

	if (!in.is_open()) {

		if (log)
			printf("failed to open file: %s\n", path.c_str());

		return;
	}


	std::string line;
	while (getline(in, line)) {
		dst.push_back(line);
	}

	in.close();
}


void extractFileName(const std::string& path, std::string& dst, const char del) {
	size_t start = path.find_last_of(del);

	if (start != std::string::npos)
		dst = path.substr(start + 1);
}

void splitPath(const std::string& path, std::string& pathDst, std::string& fileDst) {
	size_t delimeter = path.find_last_of('/');

	if (delimeter == std::string::npos) {
		pathDst = "";
		fileDst = "";
	}

	pathDst = path.substr(0, delimeter);
	fileDst = path.substr(delimeter + 1);
}

void split(const std::string& src, const char delimeter, std::string* tokens, int size) {

	size_t start = 0;
	for (int i = 0; i < size; i++) {

		size_t end = src.find(delimeter, start);
		if (end == std::string::npos) {
			tokens[i] = src.substr(start);
			break;
		}

		tokens[i] = src.substr(start, end);
		start = end + 1;
	}
}

void split(const std::string& src, std::vector<std::string>& dst, const char delimeter) {

	size_t end = src.find(delimeter);
	size_t  start = 0;

	while (end != std::string::npos) {

		dst.push_back(src.substr(start, end - start));

		start = end + 1;
		end = src.find(delimeter, start);
	}

	dst.push_back(src.substr(start));

}



uint64_t requestFileSize(const std::string& path) {

	uint64_t begin;
	uint64_t end;

	std::ifstream in(path);

	if (!in.is_open())
		return 0;

	begin = in.tellg();

	in.seekg(0, std::ios::end);
	end = in.tellg();

	in.close();

	return end - begin;
}

void listDirectoryEntries(const std::string& dir, std::vector<std::string>& dst) {

	for (const auto entry : std::filesystem::directory_iterator(dir)) {
		dst.push_back(entry.path().filename().string());
	}

}






void writeFile(const std::string& path, const std::string& src) {

	std::ofstream out(path);

	out << src;

	out.close();
}


bool startsWith(const std::string& src, const std::string& s) {
	auto beginning = src.substr(0, s.length());
	return beginning == s;
}

std::vector<std::string> split(const std::string& src, const char delimeter) {
	std::vector<std::string> result;
	size_t offset = 0;
	size_t i = 0;
	while (true) {
		i = src.find(delimeter, offset);
		if (i != std::string::npos) {
			result.push_back(src.substr(offset, i - offset));
			offset = i + 1;
		} else {
			result.push_back(src.substr(offset));
			break;
		}
	}
	return result;

}

void readOBJ(const std::string& path, std::vector<Vertex>& verticies, std::vector<uint32_t>& indicies, std::string& mtl) {
	std::vector<std::string> lines;
	readFileSplit(path, lines);


	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<uint32_t> indices;
	for (auto& line : lines) {
		if (startsWith(line, "v ")) {
			auto verts = line.substr(2);
			auto v = split(verts, ' ');
			std::vector<float> intermediate;
			for (auto& vertex : v) {
				intermediate.push_back(std::stof(vertex));
			}
			assert(intermediate.size() == 3);
			positions.push_back(glm::vec3(intermediate[0], intermediate[1], intermediate[2]));
		} else if (startsWith(line, "vn")) {
			auto norms = line.substr(3, line.length() - 3);
			auto n = split(norms, ' ');
			std::vector<float> intermediate;
			for (auto& normal : n) {
				intermediate.push_back(std::stof(normal));
			}
			assert(intermediate.size() == 3);
			normals.push_back(glm::vec3(intermediate[0], intermediate[1], intermediate[2]));
		} else if (startsWith(line, "vt")) {
			auto tcs = line.substr(3, line.length() - 3);
			auto t = split(tcs, ' ');
			std::vector<float> intermediate;
			for (auto& coord : t) {
				intermediate.push_back(std::stof(coord));
			}
			assert(intermediate.size() == 2);
			uvs.push_back(glm::vec2(intermediate[0], intermediate[1]));


		} else if (startsWith(line, "mtllib")) {
			mtl = line.substr(strlen("mtllib") + 1);
		}
	}

	for (auto& line : lines) {
		if (startsWith(line, "f")) {
			auto sections = split(line.substr(2), ' ');
			for (auto& section : sections) {
				auto values = split(section, '/');
				std::vector<int> i;
				for (auto& value : values) {
					i.push_back(std::stoi(value) - 1);
				}
				verticies.push_back(Vertex(positions[i[0]], uvs[i[1]], normals[i[2]]));
				indicies.push_back(verticies.size() - 1);
			}
		}
	}
}

void readMTL(const std::string& path, std::string& albedo, std::string& bump) {
	std::vector<std::string> lines;
	readFileSplit(path, lines);

	for (const auto& line : lines) {
		if (startsWith(line, "map_Kd")) {
			 albedo = line.substr(strlen("map_Kd") + 1);
		} else if (startsWith(line, "map_Bump")) {
			bump = line.substr(strlen("map_Bump") + 1);
		}
	}
}
