#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <filesystem>
#include <resources/pack.hpp>
#include <resources/resources.hpp>
#include <shader.hpp>

#include "pch.h"
#include "test-config.h"

namespace fs = std::filesystem;
using namespace resource;

bool initOpenGL(int major = 3, int minor = 3) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  GLFWwindow *window =
      glfwCreateWindow(1000, 1000, "shader test", nullptr, nullptr);
  if (!window) {
    return false;
  }
  glfwMakeContextCurrent(window);
  return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

class TestShader : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    shader_pack = fs::temp_directory_path() / "TestShader.pack";
    auto vec = std::vector<fs::path>(
        {fs::path(CMAKE_SOURCE_DIR) / "src/tests/TestShader"});
    ASSERT_NO_THROW(resource::packer::Pack(vec, shader_pack))
        << "Pack function should not throw any exceptions. Check TestResources "
           "for more information.";
    ASSERT_NO_THROW(shaders_ = std::make_shared<Entry>(
                        resource::LoadResources(shader_pack)));
  }
  static void TearDownTestSuite() {
    glfwTerminate();
    ASSERT_NO_THROW(resource::UnloadResources(shader_pack));
    fs::remove_all(shader_pack);
  }
  static fs::path shader_pack;
  static std::shared_ptr<Entry> shaders_;
};
fs::path TestShader::shader_pack;
std::shared_ptr<Entry> TestShader::shaders_;

TEST_F(TestShader, TestLoading) {
  if (!initOpenGL(3, 3)) {
    std::cout << "Failed to initialize GLFW. Skipping this test.";
    return;
  }
  std::shared_ptr<Shader> shader;
  ASSERT_NO_THROW(
      shader = std::make_shared<Shader>(shaders_->Get("TestShader/test.vert"),
                                        shaders_->Get("TestShader/test.frag"),
                                        shaders_->Get("TestShader/test.geom")));
}

TEST_F(TestShader, TestSettersExceptions) {
  if (!initOpenGL(3, 3)) {
    std::cout << "Failed to initialize GLFW. Skipping this test.";
    return;
  }
  std::shared_ptr<Shader> shader;
  ASSERT_NO_THROW(
      shader = std::make_shared<Shader>(shaders_->Get("TestShader/test.vert"),
                                        shaders_->Get("TestShader/test.frag"),
                                        shaders_->Get("TestShader/test.geom")));
  shader->Use();
  ASSERT_THROW(shader->set_bool("a", false), std::invalid_argument);
  ASSERT_THROW(shader->set_int("b", 0), std::invalid_argument);
  ASSERT_THROW(shader->set_uint("c", 1), std::invalid_argument);
  ASSERT_THROW(shader->set_float("d", 123), std::invalid_argument);
  ASSERT_THROW(shader->set_vec("e", glm::vec1(0)), std::invalid_argument);
  ASSERT_THROW(shader->set_vec("f", glm::vec2(0)), std::invalid_argument);
  ASSERT_THROW(shader->set_vec("g", glm::vec3(0)), std::invalid_argument);
  ASSERT_THROW(shader->set_vec("h", glm::vec4(0)), std::invalid_argument);
  ASSERT_THROW(shader->set_mat("j", glm::mat2x2(0)), std::invalid_argument);
  ASSERT_THROW(shader->set_mat("k", glm::mat2x3(0)), std::invalid_argument);
  ASSERT_THROW(shader->set_mat("l", glm::mat2x4(0)), std::invalid_argument);
  ASSERT_THROW(shader->set_mat("m", glm::mat3x2(0)), std::invalid_argument);
  ASSERT_THROW(shader->set_mat("n", glm::mat3x3(0)), std::invalid_argument);
  ASSERT_THROW(shader->set_mat("o", glm::mat3x4(0)), std::invalid_argument);
  ASSERT_THROW(shader->set_mat("p", glm::mat4x2(0)), std::invalid_argument);
  ASSERT_THROW(shader->set_mat("q", glm::mat4x3(0)), std::invalid_argument);
  ASSERT_THROW(shader->set_mat("r", glm::mat4x4(0)), std::invalid_argument);

  ASSERT_THROW(shader->set_dvec1("dvec1_v", glm::dvec1(0)), std::runtime_error);
  ASSERT_THROW(shader->set_dvec2("dvec2_v", glm::dvec2(0)), std::runtime_error);
  ASSERT_THROW(shader->set_dvec3("dvec3_v", glm::dvec3(0)), std::runtime_error);
  ASSERT_THROW(shader->set_dvec4("dvec4_v", glm::dvec4(0)), std::runtime_error);

  ASSERT_THROW(shader->set_bvec("a", glm::bvec1(0)), std::invalid_argument);
  ASSERT_THROW(shader->set_bvec("b", glm::bvec2(0)), std::invalid_argument);
  ASSERT_THROW(shader->set_bvec("c", glm::bvec3(0)), std::invalid_argument);
  ASSERT_THROW(shader->set_bvec("d", glm::bvec4(0)), std::invalid_argument);

  ASSERT_THROW(shader->set_ivec("a", glm::ivec1(0)), std::invalid_argument);
  ASSERT_THROW(shader->set_ivec("b", glm::ivec2(0)), std::invalid_argument);
  ASSERT_THROW(shader->set_ivec("c", glm::ivec3(0)), std::invalid_argument);
  ASSERT_THROW(shader->set_ivec("d", glm::ivec4(0)), std::invalid_argument);

  ASSERT_THROW(shader->set_uvec("a", glm::uvec1(0)), std::invalid_argument);
  ASSERT_THROW(shader->set_uvec("b", glm::uvec2(0)), std::invalid_argument);
  ASSERT_THROW(shader->set_uvec("c", glm::uvec3(0)), std::invalid_argument);
  ASSERT_THROW(shader->set_uvec("d", glm::uvec4(0)), std::invalid_argument);
}
TEST_F(TestShader, TestSetters) {
  if (!initOpenGL(4, 3)) {
    std::cout << "Failed to initialize GLFW. Skipping this test.";
    return;
  }
  std::shared_ptr<Shader> shader;
  ASSERT_NO_THROW(shader = std::make_shared<Shader>(
                      shaders_->GetFile("TestShader/test.vert"),
                      shaders_->GetFile("TestShader/test.frag"),
                      shaders_->GetFile("TestShader/test.geom")));
  shader->Use();
  ASSERT_NO_THROW(shader->set_bool("bool_v", false));
  ASSERT_NO_THROW(shader->set_int("int_v", 0));
  ASSERT_NO_THROW(shader->set_uint("uint_v", 1));
  ASSERT_NO_THROW(shader->set_float("float_v", false));

  ASSERT_NO_THROW(shader->set_bvec1("bvec1_v", glm::bvec1(0)));
  ASSERT_NO_THROW(shader->set_bvec2("bvec2_v", glm::bvec2(0)));
  ASSERT_NO_THROW(shader->set_bvec3("bvec3_v", glm::bvec3(0)));
  ASSERT_NO_THROW(shader->set_bvec4("bvec4_v", glm::bvec4(0)));

  ASSERT_NO_THROW(shader->set_ivec1("ivec1_v", glm::ivec1(0)));
  ASSERT_NO_THROW(shader->set_ivec2("ivec2_v", glm::ivec2(0)));
  ASSERT_NO_THROW(shader->set_ivec3("ivec3_v", glm::ivec3(0)));
  ASSERT_NO_THROW(shader->set_ivec4("ivec4_v", glm::ivec4(0)));

  ASSERT_NO_THROW(shader->set_uvec1("uvec1_v", glm::uvec1(0)));
  ASSERT_NO_THROW(shader->set_uvec2("uvec2_v", glm::uvec2(0)));
  ASSERT_NO_THROW(shader->set_uvec3("uvec3_v", glm::uvec3(0)));
  ASSERT_NO_THROW(shader->set_uvec4("uvec4_v", glm::uvec4(0)));

  ASSERT_NO_THROW(shader->set_vec1("vec1_v", glm::vec1(0)));
  ASSERT_NO_THROW(shader->set_vec2("vec2_v", glm::vec2(0)));
  ASSERT_NO_THROW(shader->set_vec3("vec3_v", glm::vec3(0)));
  ASSERT_NO_THROW(shader->set_vec4("vec4_v", glm::vec4(0)));

  ASSERT_NO_THROW(shader->set_dvec1("dvec1_v", glm::dvec1(0)));
  ASSERT_NO_THROW(shader->set_dvec2("dvec2_v", glm::dvec2(0)));
  ASSERT_NO_THROW(shader->set_dvec3("dvec3_v", glm::dvec3(0)));
  ASSERT_NO_THROW(shader->set_dvec4("dvec4_v", glm::dvec4(0)));

  ASSERT_NO_THROW(shader->set_mat2x2("mat2x2_v", glm::mat2x2(0)));
  ASSERT_NO_THROW(shader->set_mat2x3("mat2x3_v", glm::mat2x3(0)));
  ASSERT_NO_THROW(shader->set_mat2x4("mat2x4_v", glm::mat2x4(0)));
  ASSERT_NO_THROW(shader->set_mat3x2("mat3x2_v", glm::mat3x2(0)));
  ASSERT_NO_THROW(shader->set_mat3x3("mat3x3_v", glm::mat3x3(0)));
  ASSERT_NO_THROW(shader->set_mat3x4("mat3x4_v", glm::mat3x4(0)));
  ASSERT_NO_THROW(shader->set_mat4x2("mat4x2_v", glm::mat4x2(0)));
  ASSERT_NO_THROW(shader->set_mat4x3("mat4x3_v", glm::mat4x3(0)));
  ASSERT_NO_THROW(shader->set_mat4x4("mat4x4_v", glm::mat4x4(0)));
}