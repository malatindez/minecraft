#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
class Shader {
 public:
  unsigned int ID;
  // constructor generates the shader on the fly
  // ------------------------------------------------------------------------
  Shader(const char* vertexPath, const char* fragmentPath,
         const char* geometryPath = nullptr);
  // activate the shader
  void use() { glUseProgram(ID); }
  // utility uniform functions

  void setBool(const std::string& name, bool value) const;
  void setInt(const std::string& name, int value) const;
  void setUInt(const std::string& name, unsigned int value) const;
  void setFloat(const std::string& name, float value) const;
  template <class T>
  void setVec(const std::string& name, const T& value) const;
  void setMat2(const std::string& name, const glm::mat2& value) const;
  void setMat3(const std::string& name, const glm::mat3& value) const;
  void setMat4(const std::string& name, const glm::mat4& value) const;
  void setMat2x2(const std::string& name, const glm::mat2x2& value) const;
  void setMat2x3(const std::string& name, const glm::mat2x3& value) const;
  void setMat2x4(const std::string& name, const glm::mat2x4& value) const;
  void setMat3x2(const std::string& name, const glm::mat3x2& value) const;
  void setMat3x3(const std::string& name, const glm::mat3x3& value) const;
  void setMat3x4(const std::string& name, const glm::mat3x4& value) const;
  void setMat4x2(const std::string& name, const glm::mat4x2& value) const;
  void setMat4x3(const std::string& name, const glm::mat4x3& value) const;
  void setMat4x4(const std::string& name, const glm::mat4x4& value) const;

 private:
  // utility function for checking shader compilation/linking errors.
  // ------------------------------------------------------------------------
  void checkCompileErrors(GLuint shader, std::string type);
};
#endif