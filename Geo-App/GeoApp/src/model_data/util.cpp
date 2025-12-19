#include "util.h"

MinMaxXY getMinMaxXY(const std::vector<Vertex>& vertices) {
	MinMaxXY m;
	for (auto& v : vertices) {
		if (v.pos.x < m.minX) m.minX = v.pos.x;
		if (v.pos.y < m.minY) m.minY = v.pos.y;
		if (v.pos.x > m.maxX) m.maxX = v.pos.x;
		if (v.pos.y > m.maxY) m.maxY = v.pos.y;
	}

	return m;
}
