#version 430 core
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
uniform double double_v;


uniform bool bvec1_v;
uniform bvec2 bvec2_v;
uniform bvec3 bvec3_v;
uniform bvec4 bvec4_v;

uniform int ivec1_v;
uniform ivec2 ivec2_v;
uniform ivec3 ivec3_v;
uniform ivec4 ivec4_v;

uniform uint uvec1_v;
uniform uvec2 uvec2_v;
uniform uvec3 uvec3_v;
uniform uvec4 uvec4_v;

uniform float vec1_v;
uniform vec2 vec2_v;
uniform vec3 vec3_v;
uniform vec4 vec4_v;

uniform double dvec1_v;
uniform dvec2 dvec2_v;
uniform dvec3 dvec3_v;
uniform dvec4 dvec4_v;

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
    if(bool_v && bvec1_v && bvec2_v.x  && bvec3_v.y && bvec4_v.z){
        t = vec1_v * float_v * uint_v * int_v;
        t = t * float(double_v) * ivec1_v * uvec1_v;
        t = t * ivec2_v.x * ivec3_v.y * ivec4_v.z;
        t = t * uvec2_v.x * uvec3_v.y * uvec4_v.z;
        t = t * float(dvec1_v.x *dvec2_v.x * dvec3_v.y * dvec4_v.z);
    }
    t = t * mat2x2_v[0][0] * mat2x3_v[1][2] * mat2x4_v[1][3] * 
    mat3x2_v[2][0] * mat3x3_v[1][2] * mat3x4_v[2][0] * 
    mat4x2_v[2][1] * mat4x3_v[3][0] * mat4x4_v[3][0];

    vec2o = vec2_v * t;
    vec3o = vec3_v * t;
    vec4o = vec4_v * t;
    gl_Position = vec4o + vec4(vec3o, t) - vec4(vec2o, t * t, t * 6);
}