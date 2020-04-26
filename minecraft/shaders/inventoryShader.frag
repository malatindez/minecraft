#version 330 core
out vec4 FragColor;
uniform sampler2D inventory;
in vec2 TexCoords;
void main() {
    if(texture(inventory, TexCoords).a > 0.1) {
        FragColor = texture(inventory, TexCoords);
    } else {
        discard;
    }
}