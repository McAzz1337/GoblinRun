#version 410 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 norm;

uniform mat4 mvp;
uniform mat3 view;
uniform mat4 model;

out vec2 f_uv;
out vec3 f_norm;
out vec4 fragPos;

void main() {
	gl_Position = mvp * vec4(pos, 1.0);
	f_uv = uv;
	f_norm = view * norm;
	fragPos = model * vec4(pos, 1.0);
}