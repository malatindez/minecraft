#include "Shader.hpp"
static inline void CheckCompileErrors(GLuint shader, std::string const &type) {
  GLint success;
  GLchar infoLog[1024];
  if (type != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 1024, NULL, infoLog);
      std::stringstream err;
      err << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
          << infoLog
          << "\n -- --------------------------------------------------- -- "
          << std::endl;
      throw std::invalid_argument(err.str());
    }
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader, 1024, NULL, infoLog);
      std::stringstream err;
      err << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
          << infoLog
          << "\n -- --------------------------------------------------- -- "
          << std::endl;
      throw std::invalid_argument(err.str());
    }
  }
}

Shader::Shader(std::string const &vertexShaderCode,
               std::string const &fragmentShaderCode,
               std::string const &geometryShaderCode) {
  auto vertexPtr = vertexShaderCode.c_str();
  auto fragmentPtr = fragmentShaderCode.c_str();
  auto geometryPtr = geometryShaderCode.c_str();

  uint32_t vertex = UINT32_MAX;
  uint32_t fragment = UINT32_MAX;
  uint32_t geometry = UINT32_MAX;
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vertexPtr, NULL);
  glCompileShader(vertex);
  CheckCompileErrors(vertex, "VERTEX");
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fragmentPtr, NULL);
  glCompileShader(fragment);
  CheckCompileErrors(fragment, "FRAGMENT");

  if (geometryShaderCode.size() != 0) {
    geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry, 1, &geometryPtr, NULL);
    glCompileShader(geometry);
    CheckCompileErrors(geometry, "GEOMETRY");
  }
  id_ = std::make_shared<uint32_t>(glCreateProgram());
  glAttachShader(*id_, vertex);
  glAttachShader(*id_, fragment);
  if (geometry != UINT32_MAX)
    glAttachShader(*id_, geometry);
  glLinkProgram(*id_);
  CheckCompileErrors(*id_, "PROGRAM");
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  if (geometry != UINT32_MAX)
    glDeleteShader(geometry);
}
Shader::Shader(resource::Entry const &vertex_shader_file,
               resource::Entry const &fragment_shader_file)
    : Shader(vertex_shader_file.string(), fragment_shader_file.string()) {}
Shader::Shader(resource::Entry const &vertex_shader_file,
               resource::Entry const &fragment_shader_file,
               resource::Entry const &geometry_shader_file)
    : Shader(vertex_shader_file.string(), fragment_shader_file.string(),
             geometry_shader_file.string()) {}
Shader::~Shader() {
  if (id_.use_count() == 1) {
    glDeleteProgram(*id_);
  }
}

void Shader::set_bool(std::string const &name, bool value) const {
  glUniform1i(GetLocation(name), (int)value);
}
void Shader::set_int(std::string const &name, int value) const {
  glUniform1i(GetLocation(name), value);
}
void Shader::set_uint(std::string const &name, uint32_t value) const {
  glUniform1ui(GetLocation(name), value);
}
void Shader::set_float(std::string const &name, float value) const {
  glUniform1f(GetLocation(name), value);
}

void Shader::set_bvec1(std::string const &name, glm::bvec1 const &value) const {
  glUniform1i(GetLocation(name), value[0]);
}
void Shader::set_bvec2(std::string const &name, glm::bvec2 const &value) const {
  glUniform2i(GetLocation(name), value[0], value[1]);
}
void Shader::set_bvec3(std::string const &name, glm::bvec3 const &value) const {
  glUniform3i(GetLocation(name), value[0], value[1], value[2]);
}
void Shader::set_bvec4(std::string const &name, glm::bvec4 const &value) const {
  glUniform4i(GetLocation(name), value[0], value[1], value[2], value[3]);
}
void Shader::set_ivec1(std::string const &name, glm::ivec1 const &value) const {
  glUniform1iv(GetLocation(name), 1, &value[0]);
}
void Shader::set_ivec2(std::string const &name, glm::ivec2 const &value) const {
  glUniform2iv(GetLocation(name), 1, &value[0]);
}
void Shader::set_ivec3(std::string const &name, glm::ivec3 const &value) const {
  glUniform3iv(GetLocation(name), 1, &value[0]);
}
void Shader::set_ivec4(std::string const &name, glm::ivec4 const &value) const {
  glUniform4iv(GetLocation(name), 1, &value[0]);
}
void Shader::set_uvec1(std::string const &name, glm::uvec1 const &value) const {
  glUniform1uiv(GetLocation(name), 1, &value[0]);
}
void Shader::set_uvec2(std::string const &name, glm::uvec2 const &value) const {
  glUniform2uiv(GetLocation(name), 1, &value[0]);
}
void Shader::set_uvec3(std::string const &name, glm::uvec3 const &value) const {
  glUniform3uiv(GetLocation(name), 1, &value[0]);
}
void Shader::set_uvec4(std::string const &name, glm::uvec4 const &value) const {
  glUniform4uiv(GetLocation(name), 1, &value[0]);
}
void Shader::set_vec1(std::string const &name, glm::vec1 const &value) const {
  glUniform1fv(GetLocation(name), 1, &value[0]);
}
void Shader::set_vec2(std::string const &name, glm::vec2 const &value) const {
  glUniform2fv(GetLocation(name), 1, &value[0]);
}
void Shader::set_vec3(std::string const &name, glm::vec3 const &value) const {
  glUniform3fv(GetLocation(name), 1, &value[0]);
}
void Shader::set_vec4(std::string const &name, glm::vec4 const &value) const {
  glUniform4fv(GetLocation(name), 1, &value[0]);
}
void Shader::set_dvec1(std::string const &name, glm::dvec1 const &value) const {
  if (glUniform1dv == 0) {
    throw std::runtime_error(
        "OpenGL error! You should use OpenGL 4.0 or above to pass double to "
        "shader.");
  }
  glUniform1dv(GetLocation(name), 1, &value[0]);
}
void Shader::set_dvec2(std::string const &name, glm::dvec2 const &value) const {
  if (glUniform2dv == 0) {
    throw std::runtime_error(
        "OpenGL error! You should use OpenGL 4.0 or above to pass double to "
        "shader.");
  }
  glUniform2dv(GetLocation(name), 1, &value[0]);
}
void Shader::set_dvec3(std::string const &name, glm::dvec3 const &value) const {
  if (glUniform3dv == 0) {
    throw std::runtime_error(
        "OpenGL error! You should use OpenGL 4.0 or above to pass double to "
        "shader.");
  }
  glUniform3dv(GetLocation(name), 1, &value[0]);
}
void Shader::set_dvec4(std::string const &name, glm::dvec4 const &value) const {
  if (glUniform4dv == 0) {
    throw std::runtime_error(
        "OpenGL error! You should use OpenGL 4.0 or above to pass double to "
        "shader.");
  }
  glUniform4dv(GetLocation(name), 1, &value[0]);
}

void Shader::set_mat2(std::string const &name, glm::mat2 const &value) const {
  glUniformMatrix2fv(GetLocation(name), 1, GL_FALSE, &value[0][0]);
}
void Shader::set_mat3(std::string const &name, glm::mat3 const &value) const {
  glUniformMatrix3fv(GetLocation(name), 1, GL_FALSE, &value[0][0]);
}
void Shader::set_mat4(std::string const &name, glm::mat4 const &value) const {
  glUniformMatrix4fv(GetLocation(name), 1, GL_FALSE, &value[0][0]);
}
void Shader::set_mat2x2(std::string const &name,
                        glm::mat2x2 const &value) const {
  set_mat2(name, value);
}
void Shader::set_mat2x3(std::string const &name,
                        glm::mat2x3 const &value) const {
  glUniformMatrix2x3fv(GetLocation(name), 1, GL_FALSE, &value[0][0]);
}
void Shader::set_mat2x4(std::string const &name,
                        glm::mat2x4 const &value) const {
  glUniformMatrix2x4fv(GetLocation(name), 1, GL_FALSE, &value[0][0]);
}
void Shader::set_mat3x2(std::string const &name,
                        glm::mat3x2 const &value) const {
  glUniformMatrix3x2fv(GetLocation(name), 1, GL_FALSE, &value[0][0]);
}
void Shader::set_mat3x3(std::string const &name,
                        glm::mat3x3 const &value) const {
  set_mat3(name, value);
}
void Shader::set_mat3x4(std::string const &name,
                        glm::mat3x4 const &value) const {
  glUniformMatrix2x4fv(GetLocation(name), 1, GL_FALSE, &value[0][0]);
}
void Shader::set_mat4x2(std::string const &name,
                        glm::mat4x2 const &value) const {
  glUniformMatrix4x2fv(GetLocation(name), 1, GL_FALSE, &value[0][0]);
}
void Shader::set_mat4x3(std::string const &name,
                        glm::mat4x3 const &value) const {
  glUniformMatrix4x2fv(GetLocation(name), 1, GL_FALSE, &value[0][0]);
}
void Shader::set_mat4x4(std::string const &name,
                        glm::mat4x4 const &value) const {
  glUniformMatrix4fv(GetLocation(name), 1, GL_FALSE, &value[0][0]);
}

void Shader::set_bvec(std::string const &name, glm::bvec1 const &value) const {
  set_bvec1(name, value);
}
void Shader::set_bvec(std::string const &name, glm::bvec2 const &value) const {
  set_bvec2(name, value);
}
void Shader::set_bvec(std::string const &name, glm::bvec3 const &value) const {
  set_bvec3(name, value);
}
void Shader::set_bvec(std::string const &name, glm::bvec4 const &value) const {
  set_bvec4(name, value);
}

void Shader::set_ivec(std::string const &name, glm::ivec1 const &value) const {
  set_ivec1(name, value);
}
void Shader::set_ivec(std::string const &name, glm::ivec2 const &value) const {
  set_ivec2(name, value);
}
void Shader::set_ivec(std::string const &name, glm::ivec3 const &value) const {
  set_ivec3(name, value);
}
void Shader::set_ivec(std::string const &name, glm::ivec4 const &value) const {
  set_ivec4(name, value);
}

void Shader::set_uvec(std::string const &name, glm::uvec1 const &value) const {
  set_uvec1(name, value);
}
void Shader::set_uvec(std::string const &name, glm::uvec2 const &value) const {
  set_uvec2(name, value);
}
void Shader::set_uvec(std::string const &name, glm::uvec3 const &value) const {
  set_uvec3(name, value);
}
void Shader::set_uvec(std::string const &name, glm::uvec4 const &value) const {
  set_uvec4(name, value);
}

void Shader::set_vec(std::string const &name, glm::vec1 const &value) const {
  set_vec1(name, value);
}
void Shader::set_vec(std::string const &name, glm::vec2 const &value) const {
  set_vec2(name, value);
}
void Shader::set_vec(std::string const &name, glm::vec3 const &value) const {
  set_vec3(name, value);
}
void Shader::set_vec(std::string const &name, glm::vec4 const &value) const {
  set_vec4(name, value);
}

void Shader::set_dvec(std::string const &name, glm::dvec1 const &value) const {
  set_dvec1(name, value);
}
void Shader::set_dvec(std::string const &name, glm::dvec2 const &value) const {
  set_dvec2(name, value);
}
void Shader::set_dvec(std::string const &name, glm::dvec3 const &value) const {
  set_dvec3(name, value);
}
void Shader::set_dvec(std::string const &name, glm::dvec4 const &value) const {
  set_dvec4(name, value);
}

void Shader::set_mat(std::string const &name, glm::mat2 const &value) const {
  set_mat2(name, value);
}
void Shader::set_mat(std::string const &name, glm::mat3 const &value) const {
  set_mat3(name, value);
}
void Shader::set_mat(std::string const &name, glm::mat4 const &value) const {
  set_mat4(name, value);
}
void Shader::set_mat(std::string const &name, glm::mat2x3 const &value) const {
  set_mat2x3(name, value);
}
void Shader::set_mat(std::string const &name, glm::mat2x4 const &value) const {
  set_mat2x4(name, value);
}
void Shader::set_mat(std::string const &name, glm::mat3x2 const &value) const {
  set_mat3x2(name, value);
}
void Shader::set_mat(std::string const &name, glm::mat3x4 const &value) const {
  set_mat3x4(name, value);
}
void Shader::set_mat(std::string const &name, glm::mat4x2 const &value) const {
  set_mat4x2(name, value);
}
void Shader::set_mat(std::string const &name, glm::mat4x3 const &value) const {
  set_mat4x3(name, value);
}
