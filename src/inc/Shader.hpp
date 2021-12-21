#pragma once
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <type_traits>

#include "Resources/Entry.hpp"
class Shader {
public:
  // constructor generates the shader on the fly
  // ------------------------------------------------------------------------
  Shader(std::string const &vertex_shader_code,
         std::string const &fragment_shader_code,
         std::string const &geometry_shader_code = "");
  Shader(resource::Entry const &vertex_shader_file,
         resource::Entry const &fragment_shader_file);
  Shader(resource::Entry const &vertex_shader_file,
         resource::Entry const &fragment_shader_file,
         resource::Entry const &geometry_shader_file);
  ~Shader();
  // activate the shader
  void Use() { glUseProgram(*id_); }

  inline GLint GetLocation(std::string const &name) const {
    GLint location = glGetUniformLocation(*id_, name.c_str());
    if (location == -1 || location == GL_INVALID_VALUE ||
        location == GL_INVALID_OPERATION) {
      throw std::invalid_argument("Provided name is invalid!");
    }
    return location;
  }

  uint32_t id() const { return *id_; }
  // utility uniform functions
  void set_bool(std::string const &name, bool value) const;
  void set_int(std::string const &name, int value) const;
  void set_uint(std::string const &name, uint32_t value) const;
  void set_float(std::string const &name, float value) const;

  void set_bvec1(std::string const &name, glm::bvec1 const &value) const;
  void set_bvec2(std::string const &name, glm::bvec2 const &value) const;
  void set_bvec3(std::string const &name, glm::bvec3 const &value) const;
  void set_bvec4(std::string const &name, glm::bvec4 const &value) const;
  void set_ivec1(std::string const &name, glm::ivec1 const &value) const;
  void set_ivec2(std::string const &name, glm::ivec2 const &value) const;
  void set_ivec3(std::string const &name, glm::ivec3 const &value) const;
  void set_ivec4(std::string const &name, glm::ivec4 const &value) const;
  void set_uvec1(std::string const &name, glm::uvec1 const &value) const;
  void set_uvec2(std::string const &name, glm::uvec2 const &value) const;
  void set_uvec3(std::string const &name, glm::uvec3 const &value) const;
  void set_uvec4(std::string const &name, glm::uvec4 const &value) const;
  void set_vec1(std::string const &name, glm::vec1 const &value) const;
  void set_vec2(std::string const &name, glm::vec2 const &value) const;
  void set_vec3(std::string const &name, glm::vec3 const &value) const;
  void set_vec4(std::string const &name, glm::vec4 const &value) const;
  void set_dvec1(std::string const &name, glm::dvec1 const &value) const;
  void set_dvec2(std::string const &name, glm::dvec2 const &value) const;
  void set_dvec3(std::string const &name, glm::dvec3 const &value) const;
  void set_dvec4(std::string const &name, glm::dvec4 const &value) const;

  void set_mat2(std::string const &name, glm::mat2 const &value) const;
  void set_mat3(std::string const &name, glm::mat3 const &value) const;
  void set_mat4(std::string const &name, glm::mat4 const &value) const;
  void set_mat2x2(std::string const &name, glm::mat2x2 const &value) const;
  void set_mat2x3(std::string const &name, glm::mat2x3 const &value) const;
  void set_mat2x4(std::string const &name, glm::mat2x4 const &value) const;
  void set_mat3x2(std::string const &name, glm::mat3x2 const &value) const;
  void set_mat3x3(std::string const &name, glm::mat3x3 const &value) const;
  void set_mat3x4(std::string const &name, glm::mat3x4 const &value) const;
  void set_mat4x2(std::string const &name, glm::mat4x2 const &value) const;
  void set_mat4x3(std::string const &name, glm::mat4x3 const &value) const;
  void set_mat4x4(std::string const &name, glm::mat4x4 const &value) const;

  void set_bvec(std::string const &name, glm::bvec1 const &value) const;
  void set_bvec(std::string const &name, glm::bvec2 const &value) const;
  void set_bvec(std::string const &name, glm::bvec3 const &value) const;
  void set_bvec(std::string const &name, glm::bvec4 const &value) const;

  void set_ivec(std::string const &name, glm::ivec1 const &value) const;
  void set_ivec(std::string const &name, glm::ivec2 const &value) const;
  void set_ivec(std::string const &name, glm::ivec3 const &value) const;
  void set_ivec(std::string const &name, glm::ivec4 const &value) const;

  void set_uvec(std::string const &name, glm::uvec1 const &value) const;
  void set_uvec(std::string const &name, glm::uvec2 const &value) const;
  void set_uvec(std::string const &name, glm::uvec3 const &value) const;
  void set_uvec(std::string const &name, glm::uvec4 const &value) const;

  void set_vec(std::string const &name, glm::vec1 const &value) const;
  void set_vec(std::string const &name, glm::vec2 const &value) const;
  void set_vec(std::string const &name, glm::vec3 const &value) const;
  void set_vec(std::string const &name, glm::vec4 const &value) const;

  void set_dvec(std::string const &name, glm::dvec1 const &value) const;
  void set_dvec(std::string const &name, glm::dvec2 const &value) const;
  void set_dvec(std::string const &name, glm::dvec3 const &value) const;
  void set_dvec(std::string const &name, glm::dvec4 const &value) const;

  void set_mat(std::string const &name, glm::mat2 const &value) const;
  void set_mat(std::string const &name, glm::mat3 const &value) const;
  void set_mat(std::string const &name, glm::mat4 const &value) const;
  void set_mat(std::string const &name, glm::mat2x3 const &value) const;
  void set_mat(std::string const &name, glm::mat2x4 const &value) const;
  void set_mat(std::string const &name, glm::mat3x2 const &value) const;
  void set_mat(std::string const &name, glm::mat3x4 const &value) const;
  void set_mat(std::string const &name, glm::mat4x2 const &value) const;
  void set_mat(std::string const &name, glm::mat4x3 const &value) const;

private:
  std::shared_ptr<uint32_t> id_;
};