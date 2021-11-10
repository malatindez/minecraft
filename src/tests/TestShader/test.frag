#version 400 core
out vec4 FragColor;

in vec2 vec2o;
in vec3 vec3o;
in vec4 vec4o;
void main() {
    vec4 vec = vec4o + vec4(vec3o,0) + vec4(vec2o,vec2o);
    FragColor = vec;
}