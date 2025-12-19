#pragma once

#include <glm/vec3.hpp>

void wgs84ToCH1903(double lat, double lon, double& E, double& N);
glm::vec3 wgs84ToGL(double lat, double lon, double xmin, double xmax, double ymin, double ymax, double alt = 0.0f);

