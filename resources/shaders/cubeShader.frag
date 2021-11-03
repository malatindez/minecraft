#version 330 core
out vec4 FragColor;
uniform sampler2D diffuseCubeTexture[6];
uniform sampler2D specularCubeTexture[6];

in vec2 TexCoords;
flat in int TextureSide;
void main() {

    FragColor = texture(diffuseCubeTexture[TextureSide], TexCoords);
    FragColor.a = 1.0;
}