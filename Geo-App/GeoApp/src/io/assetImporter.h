#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "../model_data/vertex.h"

class Material;

namespace assetimporter {

	enum AssetType {
		DIFFUSE0,
		DIFFUSE1,
		NORMAL_MAP,
		BUMP_MAP,
		SHADER,
	};

	void init();

	void loadModel(const std::string& dir,
				   const std::string& file,
				   std::vector<Vertex>& vertices,
				   std::vector<uint32_t>& indices,
				   Material& material);



};
