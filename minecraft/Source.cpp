#include "glad.c"
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
#include "World.h"
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
GLfloat deltaTime, lastFrame = 0, breakingStart = 0, placementStart = 0;
bool breaking = false;
bool placement = false;
int main() {
    srand(time(0));
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
    TextureLoader loader;
    auto blocks = loadBlocks(&loader);
    Shader cubeShader("cubeShader.vert", "cubeShader.frag");
    Shader hoverShader("cubeShader.vert", "hoverShader.frag");
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


    hoverShader.use();
    hoverShader.setInt("diffuseCubeTexture[0]", 0);
    hoverShader.setInt("diffuseCubeTexture[1]", 1);
    hoverShader.setInt("diffuseCubeTexture[2]", 2);
    hoverShader.setInt("diffuseCubeTexture[3]", 3);
    hoverShader.setInt("diffuseCubeTexture[4]", 4);
    hoverShader.setInt("diffuseCubeTexture[5]", 5);

    hoverShader.setInt("specularCubeTexture[0]", 6);
    hoverShader.setInt("specularCubeTexture[1]", 7);
    hoverShader.setInt("specularCubeTexture[2]", 8);
    hoverShader.setInt("specularCubeTexture[3]", 9);
    hoverShader.setInt("specularCubeTexture[4]", 10);
    hoverShader.setInt("specularCubeTexture[5]", 11);
    hoverShader.setInt("hover", 14);
    hoverShader.setInt("destruction", 15);
    glActiveTexture(GL_TEXTURE15);
    glBindTexture(GL_TEXTURE_2D, destroy_none);
    glActiveTexture(GL_TEXTURE14);
    glBindTexture(GL_TEXTURE_2D, hover);
    int prevhoveredx = -1, prevhoveredy = -1, prevhoveredz = -1;
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    World w = World::NewWorld(&loader, &blocks, "rzhaka");
    uint32_t renderDistance = 8;
    camera.Position = glm::vec3(renderDistance * 8, 65, renderDistance * 8);
    Chunk*** chunks = new Chunk**[renderDistance];
    for (int i = 0; i < renderDistance; i++) {
        chunks[i] = new Chunk*[renderDistance];
        for (int j = 0; j < renderDistance; j++) {
            chunks[i][j] = w.generateChunk(i, j);
        }
    }
    glfwSwapInterval(0);
    float a = glfwGetTime();
    bool state = false;
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        cubeShader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        cubeShader.setMat4("projection", projection);
        cubeShader.setMat4("view", view);
        for (int i = 0; i < renderDistance; i++) {
            for (int j = 0; j < renderDistance; j++) {
                cubeShader.setMat4("model", chunks[i][j]->getModel());
                chunks[i][j]->Draw();
            }
        }
        if (state and not breaking) {
            glActiveTexture(GL_TEXTURE15);
            glBindTexture(GL_TEXTURE_2D, destroy_none);
        }
        int hoveredx = 2147483647, hoveredy = 2147483647, hoveredz = 2147483647;
        int prevHoveredBufx = 2147483647, prevHoveredBufy = 2147483647, prevHoveredBufz = 2147483647;
        for (int i = 0; i < 5; i++) {

            glm::vec3 g = camera.Position + glm::vec3(camera.Front.x * i, camera.Front.y * i, camera.Front.z * i);
            int x = std::round(g.x);
            int y = std::round(g.y);
            int z = std::round(g.z);
            if (x >= 0 and x < 16 * renderDistance and z >= 0 and z < 16 * renderDistance) {
                if (chunks[x / 16][z / 16]->getBlock(x, y, z) != nullptr) {

                    if (i > 2 and (prevHoveredBufx >= 0 and prevHoveredBufx < 16 * renderDistance and prevHoveredBufz >= 0 and prevHoveredBufz < 16 * renderDistance) and (placement)) {
                        if (lastFrame - placementStart > 0.1) {
                            placement = false;
                            chunks[prevHoveredBufx / 16][prevHoveredBufz / 16]->PlaceBlock(prevHoveredBufx, prevHoveredBufy, prevHoveredBufz, &(blocks[1]));
                            hoveredx = prevHoveredBufx;
                            hoveredy = prevHoveredBufy;
                            hoveredz = prevHoveredBufz;
                        }
                    }
                    hoveredx = x;
                    hoveredy = y;
                    hoveredz = z;
                    if (breaking) {
                        state = true;
                        if (prevhoveredx != hoveredx or prevhoveredy != hoveredy or prevhoveredz != hoveredz) {
                            prevhoveredx = hoveredx;
                            prevhoveredy = hoveredy;
                            prevhoveredz = hoveredz;
                            breaking = false;
                        }
                        else if (int(5.0f * (lastFrame - breakingStart) * 100 / chunks[x / 16][z / 16]->getBlock(x, y, z)->ref->hardness) >= 10) {
                            breaking = false;
                            glActiveTexture(GL_TEXTURE15);
                            glBindTexture(GL_TEXTURE_2D, destroy_none);
                            chunks[hoveredx / 16][hoveredz / 16]->BreakBlock(hoveredx, hoveredy, hoveredz);
                            hoveredx = 2147483647, hoveredy = 2147483647, hoveredz = 2147483647;
                        }
                        else {
                            glActiveTexture(GL_TEXTURE15);
                            glBindTexture(GL_TEXTURE_2D, destroy_stage[int(5.0f * (lastFrame - breakingStart) * 100 / chunks[x / 16][z / 16]->getBlock(x, y, z)->ref->hardness)]);
                        }
                    }
                    break;

                }
            }
            prevHoveredBufx = x;
            prevHoveredBufy = y;
            prevHoveredBufz = z;
        }
        hoverShader.use();
        hoverShader.setMat4("projection", projection);
        hoverShader.setMat4("view", view);
        glm::mat4 model(1.0f);
        if (hoveredx >= 0 and hoveredx < 16 * renderDistance and hoveredz >= 0 and hoveredz < 16 * renderDistance) {
            chunks[hoveredx / 16][hoveredz / 16]->getBlock(hoveredx, hoveredy, hoveredz)->ref->BindTextures();

            model = glm::translate(model, glm::vec3(hoveredx, hoveredy, hoveredz));
            model = glm::scale(model, glm::vec3(1.0001));
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            hoverShader.setMat4("model", model);
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
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        if (placement == false) {
            placement = true;
            placementStart = lastFrame;
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
