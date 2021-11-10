#pragma once
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <type_traits>

#include "Resources/file.hpp"
class Shader {
 public:
  // constructor generates the shader on the fly
  // ------------------------------------------------------------------------
  Shader(std::string const& vertex_shader_code,
         std::string const& fragment_shader_code,
         std::string const& geometry_shader_code = "");
  Shader(resource::File const& vertex_shader_file,
         resource::File const& fragment_shader_file);
  Shader(resource::File const& vertex_shader_file,
         resource::File const& fragment_shader_file,
         resource::File const& geometry_shader_file);
  ~Shader();
  // activate the shader
  void Use() { glUseProgram(*id_); }

  uint32_t id() { return *id_; }
  // utility uniform functions
  void set_bool(std::string const& name, bool value) const;
  void set_int(std::string const& name, int value) const;
  void set_uint(std::string const& name, uint32_t value) const;
  void set_float(std::string const& name, float value) const;
  void set_vec1(std::string const& name, glm::vec1 const& value) const;
  void set_vec2(std::string const& name, glm::vec2 const& value) const;
  void set_vec3(std::string const& name, glm::vec3 const& value) const;
  void set_vec4(std::string const& name, glm::vec4 const& value) const;

  void set_mat2(std::string const& name, glm::mat2 const& value) const;
  void set_mat3(std::string const& name, glm::mat3 const& value) const;
  void set_mat4(std::string const& name, glm::mat4 const& value) const;
  void set_mat2x2(std::string const& name, glm::mat2x2 const& value) const;
  void set_mat2x3(std::string const& name, glm::mat2x3 const& value) const;
  void set_mat2x4(std::string const& name, glm::mat2x4 const& value) const;
  void set_mat3x2(std::string const& name, glm::mat3x2 const& value) const;
  void set_mat3x3(std::string const& name, glm::mat3x3 const& value) const;
  void set_mat3x4(std::string const& name, glm::mat3x4 const& value) const;
  void set_mat4x2(std::string const& name, glm::mat4x2 const& value) const;
  void set_mat4x3(std::string const& name, glm::mat4x3 const& value) const;
  void set_mat4x4(std::string const& name, glm::mat4x4 const& value) const;

  void set_vec(std::string const& name, glm::vec1 const& value) const;
  void set_vec(std::string const& name, glm::vec2 const& value) const;
  void set_vec(std::string const& name, glm::vec3 const& value) const;
  void set_vec(std::string const& name, glm::vec4 const& value) const;

  void set_mat(std::string const& name, glm::mat2 const& value) const;
  void set_mat(std::string const& name, glm::mat3 const& value) const;
  void set_mat(std::string const& name, glm::mat4 const& value) const;
  void set_mat(std::string const& name, glm::mat2x3 const& value) const;
  void set_mat(std::string const& name, glm::mat2x4 const& value) const;
  void set_mat(std::string const& name, glm::mat3x2 const& value) const;
  void set_mat(std::string const& name, glm::mat3x4 const& value) const;
  void set_mat(std::string const& name, glm::mat4x2 const& value) const;
  void set_mat(std::string const& name, glm::mat4x3 const& value) const;

 private:
  std::shared_ptr<uint32_t> id_;
};