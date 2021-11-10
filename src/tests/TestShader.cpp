#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <filesystem>

#include "Resources/Resources.hpp"
#include "Resources/pack.hpp"
#include "Shader.hpp"
#include "TestConfig.h"
#include "gtest/gtest.h"

namespace fs = std::filesystem;

class TestShader : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(
        1000, 1000, "shader test", NULL, NULL);
    ASSERT_FALSE(window == NULL) << "Failed to create GLFW window" << std::endl;
    glfwMakeContextCurrent(window);
    ASSERT_TRUE(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) << "Failed to load glad" << std::endl;

    shader_pack = fs::temp_directory_path() / "TestShader.pack";
    auto vec = std::vector<fs::path>(
        {fs::path(CMAKE_SOURCE_DIR) / "src/tests/TestShader"});
    ASSERT_NO_THROW(resource::packer::Pack(vec, shader_pack))
        << "Pack function should not throw any exceptions. Check TestResources "
           "for more information.";
    ASSERT_NO_THROW(shaders_ = std::make_shared<Directory>(
                        Resources::LoadResources(shader_pack)));
  }
  static void TearDownTestSuite() {
    glfwTerminate();
    ASSERT_NO_THROW(Resources::UnloadResources(shader_pack));
    fs::remove_all(shader_pack);
  }
  static fs::path shader_pack;
  static std::shared_ptr<Directory> shaders_;
};
fs::path TestShader::shader_pack;
std::shared_ptr<Directory> TestShader::shaders_;

TEST_F(TestShader, TestLoading) {
  std::shared_ptr<Shader> shader;
  ASSERT_NO_THROW(shader = std::make_shared<Shader>(
                      shaders_->GetFile("TestShader/test.vert"),
                      shaders_->GetFile("TestShader/test.frag"),
                      shaders_->GetFile("TestShader/test.geom")));
}
TEST_F(TestShader, TestSetters) {
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
  ASSERT_NO_THROW(shader->set_vec1("vec1_v", glm::vec1(0)));
  ASSERT_NO_THROW(shader->set_vec2("vec2_v", glm::vec2(0)));
  ASSERT_NO_THROW(shader->set_vec3("vec3_v", glm::vec3(0)));
  ASSERT_NO_THROW(shader->set_vec4("vec4_v", glm::vec4(0)));
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

TEST_F(TestShader, TestSettersExceptions) {
  std::shared_ptr<Shader> shader;
  ASSERT_NO_THROW(shader = std::make_shared<Shader>(
                      shaders_->GetFile("TestShader/test.vert"),
                      shaders_->GetFile("TestShader/test.frag"),
                      shaders_->GetFile("TestShader/test.geom")));
  shader->Use();
  ASSERT_THROW(shader->set_bool("a", false), std::invalid_argument);
  ASSERT_THROW(shader->set_int("b", 0), std::invalid_argument);
  ASSERT_THROW(shader->set_uint("c", 1), std::invalid_argument);
  ASSERT_THROW(shader->set_float("d", false), std::invalid_argument);
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
}