#include "Shader.hpp"
static inline void CheckCompileErrors(GLuint shader, std::string type) {
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

Shader::Shader(std::string const& vertexShaderCode,
               std::string const& fragmentShaderCode,
               std::string const& geometryShaderCode) {
  auto vertexPtr = vertexShaderCode.c_str();
  auto fragmentPtr = fragmentShaderCode.c_str();
  auto geometryPtr = geometryShaderCode.c_str();

  uint32_t vertex, fragment, geometry;
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
  if (geometryShaderCode.size() != 0) glAttachShader(*id_, geometry);
  glLinkProgram(*id_);
  CheckCompileErrors(*id_, "PROGRAM");
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  if (geometryShaderCode.size() != 0) glDeleteShader(geometry);
}
Shader::Shader(resource::File const& vertex_shader_file,
               resource::File const& fragment_shader_file)
    : Shader(vertex_shader_file.string(), fragment_shader_file.string()) {}
Shader::Shader(resource::File const& vertex_shader_file,
               resource::File const& fragment_shader_file,
               resource::File const& geometry_shader_file)
    : Shader(vertex_shader_file.string(), fragment_shader_file.string(),
             geometry_shader_file.string()) {}
Shader::~Shader() {
  if (id_.use_count() == 1) {
    glDeleteProgram(*id_);
  }
}
static inline GLint GetLocation(GLint programId, std::string const& name) {
  GLint location = glGetUniformLocation(programId, name.c_str());
  if (location == -1 || location == GL_INVALID_VALUE || location == GL_INVALID_OPERATION) {
    throw std::invalid_argument("Provided name is invalid!");
  }
  return location;
}
void Shader::set_vec1(std::string const& name, glm::vec1 const& value) const {
  glUniform1fv(GetLocation(*id_, name), 1, &value[0]);
}
void Shader::set_vec2(std::string const& name, glm::vec2 const& value) const {
  glUniform2fv(GetLocation(*id_, name), 1, &value[0]);
}
void Shader::set_vec3(std::string const& name, glm::vec3 const& value) const {
  glUniform3fv(GetLocation(*id_, name), 1, &value[0]);
}
void Shader::set_vec4(std::string const& name, glm::vec4 const& value) const {
  glUniform4fv(GetLocation(*id_, name), 1, &value[0]);
}

void Shader::set_mat2(std::string const& name, glm::mat2 const& value) const {
  glUniformMatrix2fv(GetLocation(*id_, name), 1, GL_FALSE, &value[0][0]);
}
void Shader::set_mat3(std::string const& name, glm::mat3 const& value) const {
  glUniformMatrix3fv(GetLocation(*id_, name), 1, GL_FALSE, &value[0][0]);
}
void Shader::set_mat4(std::string const& name, glm::mat4 const& value) const {
  glUniformMatrix4fv(GetLocation(*id_, name), 1, GL_FALSE, &value[0][0]);
}
void Shader::set_mat2x2(std::string const& name,
                        glm::mat2x2 const& value) const {
  set_mat2(name, value);
}
void Shader::set_mat2x3(std::string const& name,
                        glm::mat2x3 const& value) const {
  glUniformMatrix2x3fv(GetLocation(*id_, name), 1, GL_FALSE, &value[0][0]);
}
void Shader::set_mat2x4(std::string const& name,
                        glm::mat2x4 const& value) const {
  glUniformMatrix2x4fv(GetLocation(*id_, name), 1, GL_FALSE, &value[0][0]);
}
void Shader::set_mat3x2(std::string const& name,
                        glm::mat3x2 const& value) const {
  glUniformMatrix3x2fv(GetLocation(*id_, name), 1, GL_FALSE, &value[0][0]);
}
void Shader::set_mat3x3(std::string const& name,
                        glm::mat3x3 const& value) const {
  set_mat3(name, value);
}
void Shader::set_mat3x4(std::string const& name,
                        glm::mat3x4 const& value) const {
  glUniformMatrix2x4fv(GetLocation(*id_, name), 1, GL_FALSE, &value[0][0]);
}
void Shader::set_mat4x2(std::string const& name,
                        glm::mat4x2 const& value) const {
  glUniformMatrix4x2fv(GetLocation(*id_, name), 1, GL_FALSE, &value[0][0]);
}
void Shader::set_mat4x3(std::string const& name,
                        glm::mat4x3 const& value) const {
  glUniformMatrix4x2fv(GetLocation(*id_, name), 1, GL_FALSE, &value[0][0]);
}
void Shader::set_mat4x4(std::string const& name,
                        glm::mat4x4 const& value) const {
  glUniformMatrix4fv(GetLocation(*id_, name), 1, GL_FALSE, &value[0][0]);
}
void Shader::set_bool(std::string const& name, bool value) const {
  glUniform1i(GetLocation(*id_, name), (int)value);
}
void Shader::set_int(std::string const& name, int value) const {
  glUniform1i(GetLocation(*id_, name), value);
}
void Shader::set_uint(std::string const& name, uint32_t value) const {
  glUniform1ui(GetLocation(*id_, name), value);
}
void Shader::set_float(std::string const& name, float value) const {
  glUniform1f(GetLocation(*id_, name), value);
}

#define MINECRAFT_SHADER_CPP_FUNC_VEC(x)                                \
  void Shader::set_vec(std::string const& name, glm::vec##x const& vec) \
      const {                                                           \
    set_vec##x##(name, vec);                                            \
  }
MINECRAFT_SHADER_CPP_FUNC_VEC(1);
MINECRAFT_SHADER_CPP_FUNC_VEC(2);
MINECRAFT_SHADER_CPP_FUNC_VEC(3);
MINECRAFT_SHADER_CPP_FUNC_VEC(4);
#undef MINECRAFT_SHADER_CPP_FUNC_VEC
#define MINECRAFT_SHADER_CPP_FUNC_MAT(a, b)                                   \
  void Shader::set_mat(std::string const& name, glm::mat##a##x##b const& mat) \
      const {                                                                 \
    set_mat##a##x##b(name, mat);                                              \
  }
#define MINECRAFT_SHADER_CPP_FUNC_MAT_ITR(x) \
  MINECRAFT_SHADER_CPP_FUNC_MAT(##x, 2);     \
  MINECRAFT_SHADER_CPP_FUNC_MAT(##x, 3);     \
  MINECRAFT_SHADER_CPP_FUNC_MAT(##x, 4);
MINECRAFT_SHADER_CPP_FUNC_MAT_ITR(2);
MINECRAFT_SHADER_CPP_FUNC_MAT_ITR(3);
MINECRAFT_SHADER_CPP_FUNC_MAT_ITR(4);
#undef MINECRAFT_SHADER_CPP_FUNC_MAT
#undef MINECRAFT_SHADER_CPP_FUNC_MAT_ITR
