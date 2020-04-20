#version 330 core
out vec4 FragColor;
uniform sampler2D diffuseCubeTexture[6];
uniform sampler2D specularCubeTexture[6];
uniform sampler2D destroy_stage;
uniform sampler2D hover;

uniform bool hovered;
in vec2 TexCoords;
flat in int TextureSide;
void main() {

    FragColor = texture(diffuseCubeTexture[TextureSide], TexCoords);
    if (texture(destroy_stage, TexCoords).a > 0.1) {
        FragColor = FragColor*texture(destroy_stage, TexCoords);
    }
    if (hovered) {
        FragColor = FragColor*(texture(hover, TexCoords));
    }
}