#include <glad/glad.h>
#include <glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Shader.h"

#include <iostream>
#include <iostream>
#include "Model.h"
#include <map>
#include "Block.h"
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
GLfloat deltaTime, lastFrame = 0, breakingStart = 0;
bool breaking = false;
int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "kinda minecraft version i.d.k.", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    float vertices[] = {
        // positions          // normals           // texture coords
        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, 0, // Bottom-left
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0, // top-right
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0, // bottom-right         
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0, // top-right
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f, 0, // bottom-left
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f, 0, // top-left
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, 1, // bottom-left
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1, // bottom-right
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1, // top-right
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1, // top-right
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f, 1, // top-left
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, 1, // bottom-left
        // Left face
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 2, // top-right
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 2, // top-left
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 2, // bottom-left
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 2, // bottom-left
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f, 2, // bottom-right
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 2, // top-right
        // Right face
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 3, // top-left
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 3, // bottom-right
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 3, // top-right         
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 3, // bottom-right
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 3, // top-left
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f, 3, // bottom-left     
        // Bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f, 4, // top-right
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f, 4, // top-left
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f, 4, // bottom-left
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f, 4, // bottom-left
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 4, // bottom-right
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f, 4, // top-right
        // Top face
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, 5, // top-left
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, 5, // bottom-right
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f, 5, // top-right     
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, 5, // bottom-right
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, 5, // top-left
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 5  // bottom-left    
    };

    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    auto blocks = loadBlocks();
    Shader cubeShader("cubeShader.vert", "cubeShader.frag");
    cubeShader.use();
    cubeShader.setInt("diffuseCubeTexture[0]", 0);
    cubeShader.setInt("diffuseCubeTexture[1]", 1);
    cubeShader.setInt("diffuseCubeTexture[2]", 2);
    cubeShader.setInt("diffuseCubeTexture[3]", 3);
    cubeShader.setInt("diffuseCubeTexture[4]", 4);
    cubeShader.setInt("diffuseCubeTexture[5]", 5);

    cubeShader.setInt("specularCubeTexture[0]", 6);
    cubeShader.setInt("specularCubeTexture[1]", 7);
    cubeShader.setInt("specularCubeTexture[2]", 8);
    cubeShader.setInt("specularCubeTexture[3]", 9);
    cubeShader.setInt("specularCubeTexture[4]", 10);
    cubeShader.setInt("specularCubeTexture[5]", 11);
    unsigned int destroy_stage[10] = {
        loadTexture("blocks\\minecraft\\textures\\destroy_stage_0.png"),
        loadTexture("blocks\\minecraft\\textures\\destroy_stage_1.png"),
        loadTexture("blocks\\minecraft\\textures\\destroy_stage_2.png"),
        loadTexture("blocks\\minecraft\\textures\\destroy_stage_3.png"),
        loadTexture("blocks\\minecraft\\textures\\destroy_stage_4.png"),
        loadTexture("blocks\\minecraft\\textures\\destroy_stage_5.png"),
        loadTexture("blocks\\minecraft\\textures\\destroy_stage_6.png"),
        loadTexture("blocks\\minecraft\\textures\\destroy_stage_7.png"),
        loadTexture("blocks\\minecraft\\textures\\destroy_stage_8.png"),
        loadTexture("blocks\\minecraft\\textures\\destroy_stage_9.png")
    }, destroy_none = loadTexture("blocks\\minecraft\\textures\\destroy_none.png"), 
        hover = loadTexture("blocks\\minecraft\\textures\\hover.png");

    cubeShader.setInt("destroy_stage", 15);
    cubeShader.setInt("hover", 14);

    glActiveTexture(GL_TEXTURE15);
    glBindTexture(GL_TEXTURE_2D, destroy_none);
    glActiveTexture(GL_TEXTURE14);
    glBindTexture(GL_TEXTURE_2D, hover);
    int prevhoveredx = -1, prevhoveredz = -1;
    glfwSwapInterval(0);
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        cubeShader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        cubeShader.setMat4("projection", projection);
        cubeShader.setMat4("view", view);



        int hoveredx = -1, hoveredz = -1;
        for (int i = 0; i < 16; i++) {

            glm::vec3 g = camera.Position + glm::vec3(camera.Front.x * i, camera.Front.y * i, camera.Front.z * i);
            int x = std::round(g.x);
            int y = std::round(g.y);
            int z = std::round(g.z);
            if (y == -2) {
                hoveredx = x;
                hoveredz = z;
                break;
            }
        }
        if (hoveredx != prevhoveredx or hoveredz != prevhoveredz) {
            breaking = false;
            prevhoveredx = hoveredx;
            prevhoveredz = hoveredz;
        }




        glm::mat4 model = glm::mat4(1.0f);
        blocks[0].BindTextures();
        glBindVertexArray(cubeVAO);
        for (size_t i = 0; i < 8; i++) {
            for (size_t j = 0; j < 8; j++) {
                if ((i * j + 1) % 2 == 0) {
                    model = glm::mat4(1.0f);
                    model = ::glm::translate(model, glm::vec3(i, -2, j));
                    cubeShader.setMat4("model", model);
                    if (i == hoveredx and j == hoveredz) {
                        cubeShader.setBool("hovered", 1);
                        if (int(5.0f * (lastFrame - breakingStart) / blocks[0].hardness) > 10) {
                            breaking = false;
                        }
                        if (breaking) {
                            glActiveTexture(GL_TEXTURE15);
                            glBindTexture(GL_TEXTURE_2D, destroy_stage[int(5.0f * (lastFrame - breakingStart) / blocks[0].hardness)]);
                        }

                        glDrawArrays(GL_TRIANGLES, 0, 36);
                        cubeShader.setBool("hovered", 0);
                        if (breaking) {
                            glActiveTexture(GL_TEXTURE15);
                            glBindTexture(GL_TEXTURE_2D, destroy_none);
                        }
                    }
                    else {
                        glDrawArrays(GL_TRIANGLES, 0, 36);
                    }
                }
            }
        }
        
        blocks[1].BindTextures();
        for (size_t i = 0; i < 8; i++) {
            for (size_t j = 0; j <8; j++) {
                if ((i * j + 1) % 2 == 1) {
                    model = glm::mat4(1.0f);
                    model = ::glm::translate(model, glm::vec3(i, -2, j));
                    cubeShader.setMat4("model", model);

                    if (i == hoveredx and j == hoveredz) {
                        cubeShader.setBool("hovered", 1);
                        if (int(5.0f * (lastFrame - breakingStart) / blocks[1].hardness) > 10) {
                            breaking = false;
                        }
                        if (breaking) {
                            glActiveTexture(GL_TEXTURE15);
                            glBindTexture(GL_TEXTURE_2D, destroy_stage[int(5.0f * (lastFrame - breakingStart) / blocks[1].hardness)]);
                        }
                        glDrawArrays(GL_TRIANGLES, 0, 36);
                        cubeShader.setBool("hovered", 0);
                        if (breaking) {
                            glActiveTexture(GL_TEXTURE15);
                            glBindTexture(GL_TEXTURE_2D, destroy_none);
                        }
                    }
                    else {
                        glDrawArrays(GL_TRIANGLES, 0, 36);
                    }
                }
            }
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        if (breaking == false) {
            breaking = true;
            breakingStart = lastFrame;
        }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        breaking = false;
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
