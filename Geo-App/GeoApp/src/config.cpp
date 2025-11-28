#include "config.h"

#include <functional>

#include "io/fileio.h"

Config::Config() {
	std::string configText;
	readFile("config/config.txt", configText);
	size_t firstNewline = configText.find_first_of("\n");
	size_t  firstSpace = configText.find_first_of(" ");
	size_t end = firstNewline <= firstSpace ? firstNewline : firstSpace;
	if (end != std::string::npos) {
		configText = configText.substr(0, end);
	}

	std::function<std::string(const std::string&)> getValue = [](const std::string& line) {
		size_t i = line.find(" ");
		if (i != std::string::npos) {
			return line.substr(i + 1);
		}
		printf("Malformed config at line: %s\n", line.c_str());
		exit(1);
		};
	std::string configFileName;
	if (configText.find(".txt") == std::string::npos) {
		configFileName = configText + ".txt";
	}
	else {
		configFileName = configText;
	}
	std::string configFilePath = "config/" + configFileName;
	std::vector<std::string> configValues;
	readFileSplit(configFilePath, configValues);
	for (auto& s : configValues) {
		if (s.find("map") != std::string::npos) {
			this->mapfileName = getValue(s);
		}
		else if (s.find("image") != std::string::npos) {
			this->textureFileName = getValue(s);
		}
		else if (s.find("xmin") != std::string::npos) {
			this->xmin = std::stod(getValue(s));
		}
		else if (s.find("xmax") != std::string::npos) {
			this->xmax = std::stod(getValue(s));
		}
		else if (s.find("ymin") != std::string::npos) {
			this->ymin = std::stod(getValue(s));
		}
		else if (s.find("ymax") != std::string::npos) {
			this->ymax = std::stod(getValue(s));
		}
	}
}