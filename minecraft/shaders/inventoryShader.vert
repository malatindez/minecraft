#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec3 FragPos;
out vec2 TexCoords;

uniform mat4 projection;
uniform float offset;
void main() {
	gl_Position = projection * vec4(aPos, offset, 1.0);
    TexCoords = aTexCoords;
}