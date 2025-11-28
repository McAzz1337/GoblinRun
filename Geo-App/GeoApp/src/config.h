#pragma once

#include <string>

struct Config {
	std::string mapfileName;
	std::string textureFileName;
	double xmin;
	double xmax;
	double ymin;
	double ymax;

	Config();
};
