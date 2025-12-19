#include "coordinateConversion.h"

#include <algorithm>

#include <glm/glm.hpp>

void wgs84ToCH1903(double lat, double lon, double& E, double& N) {
	double lat_sec = lat * 3600.0;
	double lon_sec = lon * 3600.0;

	double lat_aux = (lat_sec - 169028.66) / 10000.0;
	double lon_aux = (lon_sec - 26782.5) / 10000.0;

	E = 600072.37
		+ 211455.93 * lon_aux
		- 10938.51 * lon_aux * lat_aux
		- 0.36 * lon_aux * lat_aux * lat_aux
		- 44.54 * lon_aux * lon_aux * lon_aux;

	N = 200147.07
		+ 308807.95 * lat_aux
		+ 3745.25 * lon_aux * lon_aux
		+ 76.63 * lat_aux * lat_aux
		- 194.56 * lon_aux * lon_aux * lat_aux
		+ 119.79 * lat_aux * lat_aux * lat_aux;
}

// 3️⃣ Convenience: WGS84 → OpenGL vec3
glm::vec3 wgs84ToGL(double lat, double lon,
	double xmin, double xmax,
	double ymin, double ymax,
	double alt) {
	double E, N;
	wgs84ToCH1903(lat, lon, E, N);
	double xhalf = (xmin + xmax) / 2.0;
	double zhalf = (ymin + ymax) / 2.0;
	double xdelta = (xmax - xmin) / 2.0;
	double zdelta = (ymax - ymin) / 2.0;
	double delta = std::max<double>(xdelta, zdelta);

	float x_gl = (E - xhalf) / delta; // east
	float z_gl = (N - zhalf) / delta; // north
	float y_gl = alt;                 // height

	return glm::vec3(x_gl, y_gl, -z_gl) ;
}
