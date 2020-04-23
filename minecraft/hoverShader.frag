#version 330 core
out vec4 FragColor;
uniform sampler2D destruction;
uniform sampler2D hover;
in vec2 TexCoords;
flat in int TextureSide;
void main() {
    vec4 hoverColor = texture(hover,TexCoords);
    vec4 destructionColor = texture(destruction,TexCoords);
    if (hoverColor.a < 0.1 && destructionColor.a < 0.1)  {
       discard;
    }
    if (hoverColor.a > 0.1) {
        FragColor = hoverColor;
    }
    if (texture(destruction,TexCoords).a > 0.1) {
        FragColor = destructionColor;
    }
}