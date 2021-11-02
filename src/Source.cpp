#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Shader.h"

#include <iostream>
#include <iostream>
#include <map>
#include "Block.h"
#include "World.h"
#include "Input.h"
#include "Interface.h"
#include "Player.h"
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

double deltaTime, lastFrame = 0, breakingStart = 0, placementStart = 0;
bool breaking = false;
bool placement = false;
enum current_state {
    CS_INTERFACE,
    CS_CAMERA
};
int main() {
    srand((uint32_t)time(0));
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
    Shader cubeShader("shaders\\cubeShader.vert", "shaders\\cubeShader.frag");
    Shader hoverShader("shaders\\cubeShader.vert", "shaders\\hoverShader.frag");
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
        loader.LoadTexture("resources\\minecraft\\gui\\blocks\\destroy_stage_0.png"),
        loader.LoadTexture("resources\\minecraft\\gui\\blocks\\destroy_stage_1.png"),
        loader.LoadTexture("resources\\minecraft\\gui\\blocks\\destroy_stage_2.png"),
        loader.LoadTexture("resources\\minecraft\\gui\\blocks\\destroy_stage_3.png"),
        loader.LoadTexture("resources\\minecraft\\gui\\blocks\\destroy_stage_4.png"),
        loader.LoadTexture("resources\\minecraft\\gui\\blocks\\destroy_stage_5.png"),
        loader.LoadTexture("resources\\minecraft\\gui\\blocks\\destroy_stage_6.png"),
        loader.LoadTexture("resources\\minecraft\\gui\\blocks\\destroy_stage_7.png"),
        loader.LoadTexture("resources\\minecraft\\gui\\blocks\\destroy_stage_8.png"),
        loader.LoadTexture("resources\\minecraft\\gui\\blocks\\destroy_stage_9.png")
    }, destroy_none = loader.LoadTexture("resources\\minecraft\\gui\\blocks\\destroy_none.png"),
        hover = loader.LoadTexture("resources\\minecraft\\gui\\blocks\\hover.png");


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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCullFace(GL_BACK);
    int32_t renderDistance = 16;
    World w = World::NewWorld(&loader, &blocks, "rzhaka", renderDistance);

    glfwSwapInterval(1);
    double a = glfwGetTime();
    bool state = false;
    Interface gui(&loader);
    Input input(window);
    Player player(window, &w, &deltaTime, &input);
    player.cam.Position = glm::vec3(renderDistance * 8, 65, renderDistance * 8);
    w.updatePlayerCoords(Block::Coords(player.cam.Position.x, player.cam.Position.y, player.cam.Position.z));
#include <thread>
    w.startThreads(std::thread::hardware_concurrency());
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        double  currentFrame = glfwGetTime();
        if (currentFrame > 2) {
            w.updateVertices();
        }
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        input.update();
        player.update();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        cubeShader.use();
        glm::mat4 projection = glm::perspective(glm::radians(player.cam.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
        glm::mat4 view = player.cam.GetViewMatrix();
        cubeShader.setMat4("projection", projection);
        cubeShader.setMat4("view", view);
        w.Draw(&cubeShader);
        w.updatePlayerCoords(Block::Coords(player.cam.Position.x, player.cam.Position.y, player.cam.Position.z));
        if (state && !breaking) {
            glActiveTexture(GL_TEXTURE15);
            glBindTexture(GL_TEXTURE_2D, destroy_none);
        }
        int32_t hoveredx = 2147483647, hoveredy = 2147483647, hoveredz = 2147483647;
        int32_t prevHoveredBufx = 2147483647, prevHoveredBufy = 2147483647, prevHoveredBufz = 2147483647;
        
        for (double i = 0; i < 5; i+= 1) {
            glm::vec3 g = player.cam.Position + glm::vec3(player.cam.Front.x * i, player.cam.Front.y * i, player.cam.Front.z * i);
            int32_t x = (int32_t)std::round(g.x);
            int32_t y = (int32_t)std::round(g.y);
            int32_t z = (int32_t)std::round(g.z);
            if (x >= 0 && x < (int32_t)(16 * renderDistance) && z >= 0 && z < (int32_t)(16 * renderDistance)) {
                if (w.getBlock(x, y, z) != nullptr) {

                    if (i > 2 && (prevHoveredBufx >= 0 && prevHoveredBufx < (int32_t)(16 * renderDistance) && prevHoveredBufz >= 0 && prevHoveredBufz < (int32_t)(16 * renderDistance)) && (placement)) {
                        if (lastFrame - placementStart > 0) {
                            placementStart = lastFrame + 0.25;
                            w.placeBlock(prevHoveredBufx, prevHoveredBufy, prevHoveredBufz, &(blocks[1]));
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
                        if (prevhoveredx != hoveredx || prevhoveredy != hoveredy || prevhoveredz != hoveredz) {
                            prevhoveredx = hoveredx;
                            prevhoveredy = hoveredy;
                            prevhoveredz = hoveredz;
                            breaking = false;
                        }
                        else if (int(5.0f * (lastFrame - breakingStart) * 100 / w.getBlock(x, y, z)->ref->hardness) >= 10) {
                            breaking = false;
                            glActiveTexture(GL_TEXTURE15);
                            glBindTexture(GL_TEXTURE_2D, destroy_none);
                            w.breakBlock(hoveredx, hoveredy, hoveredz);
                            hoveredx = 2147483647, hoveredy = 2147483647, hoveredz = 2147483647;
                        }
                        else {
                            glActiveTexture(GL_TEXTURE15);
                            glBindTexture(GL_TEXTURE_2D, destroy_stage[int(5.0f * (lastFrame - breakingStart) * 100 / w.getBlock(x, y, z)->ref->hardness)]);
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
        if (hoveredx >= 0 && hoveredx < (int32_t)(16 * renderDistance) && hoveredz >= 0 && hoveredz < (int32_t)(16 * renderDistance)) {

            glActiveTexture(GL_TEXTURE14);
            glBindTexture(GL_TEXTURE_2D, hover);
            Block* x = w.getBlock(hoveredx, hoveredy, hoveredz);
            if (x != nullptr && x->ref != nullptr) {
                x->ref->BindTextures();
                model = glm::translate(model, glm::vec3(hoveredx, hoveredy, hoveredz));
                model = glm::scale(model, glm::vec3(1.0001f));
                hoverShader.setMat4("model", model);
                glBindVertexArray(cubeVAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
        gui.DrawInterface(Interface::TYPE::INVENTORY_WIDGET, SCR_WIDTH, SCR_HEIGHT);

        gui.DrawInterface(Interface::TYPE::CURSOR, SCR_WIDTH, SCR_HEIGHT);
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