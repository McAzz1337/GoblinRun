#version 410 core

in vec2 f_uv;
in vec3 f_norm;
in vec4 fragPos;
out vec4 color;

uniform sampler2D tex;
uniform vec3 cameraPos;

vec3 red = vec3(1.0, 0.0, 0.0);
vec3 blue = vec3(0.0, 0.0, 1.0);
vec3 green = vec3(0.0, 1.0, 0.0);
vec3 white = vec3(1.0, 1.0, 1.0);

vec3 lightPos = vec3(0.0, 2.0, 0.0);

uniform int isPin;

void main() {
		vec3 lightDir = normalize(lightPos - fragPos.xyz);
		float diff = max(dot(f_norm, lightDir), 0.0);
		float ambient = 0.3;
	if (isPin == 1) {
		color = vec4(red * (ambient + diff), 1.0);
	} else {

		vec3 c = texture(tex, f_uv).xyz;
		color = vec4(c * (ambient + diff), 1.0);
	}
}
