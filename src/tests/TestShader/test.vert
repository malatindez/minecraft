#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoords;
layout(location=3) in float aTextureSide;

out vec2 vec2o;
out vec3 vec3o;
out vec4 vec4o;

uniform bool bool_v;
uniform int int_v;
uniform uint uint_v;
uniform float float_v;
uniform float vec1_v;
uniform vec2 vec2_v;
uniform vec3 vec3_v;
uniform vec4 vec4_v;
uniform mat2x2 mat2x2_v;
uniform mat2x3 mat2x3_v;
uniform mat2x4 mat2x4_v;
uniform mat3x2 mat3x2_v;
uniform mat3x3 mat3x3_v;
uniform mat3x4 mat3x4_v;
uniform mat4x2 mat4x2_v;
uniform mat4x3 mat4x3_v;
uniform mat4x4 mat4x4_v;
void main()
{
    float t = 0;
    if(bool_v){
        t = vec1_v * float_v * uint_v * int_v;
    }
    t = t * mat2x2_v[0][0] * mat2x3_v[1][2] * mat2x4_v[1][3] * 
    mat3x2_v[2][0] * mat3x3_v[1][2] * mat3x4_v[2][0] * 
    mat4x2_v[2][1] * mat4x3_v[3][0] * mat4x4_v[3][0];
    
    vec2o = vec2_v * t;
    vec3o = vec3_v * t;
    vec4o = vec4_v * t;
    gl_Position = vec4o + vec4(vec3o, t) - vec4(vec2o, t * t, t * 6);
}