#pragma once

#include <vector>

#include "vertex.h"

struct MinMaxXY {
	float minX = FLT_MAX;
	float minY = FLT_MAX;
	float maxX = -FLT_MAX;
	float maxY = -FLT_MAX;
};

MinMaxXY getMinMaxXY(const std::vector<Vertex>& vertices);
