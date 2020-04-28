#ifndef INTERFACE_H
#define INTERFACE_H
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "TextureLoader.h"
#include "Shader.h"
class Interface {
	const float vertices[6*4] = {
		// Front face
		-1.0f, -1.0f, 0.0f, 1.0f, // bottom-left
		 1.0f, -1.0f, 1.0f, 1.0f, // bottom-right
		 1.0f,  1.0f, 1.0f, 0.0f, // top-right
		 1.0f,  1.0f, 1.0f, 0.0f, // top-right
		-1.0f,  1.0f, 0.0f, 0.0f, // top-left
		-1.0f, -1.0f, 0.0f, 1.0f, // bottom-left    
	};
public:

#define INTERFACE_TYPE_LEN 3
	enum class TYPE {
		INVENTORY = 0,
		INVENTORY_WIDGET = 1,
		CURSOR = 2
	};
	uint32_t* interfaces;
	uint32_t* VAOs;
	uint32_t* VBOs;
	float** interfaceVertices;

	Shader *inventoryShader;
	static uint32_t Load(TextureLoader* tl, std::string texture, float* vertices, uint32_t& VAO, uint32_t& VBO) {
		uint32_t returnValue = tl->LoadTexture(texture);
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 6 * 4 * sizeof(float), vertices, GL_STATIC_DRAW);

		glBindVertexArray(VAO);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);
		return returnValue;
	}
	
	float* moveVertices(float textureFromx, float textureFromy, float textureTox, float textureToy, float textureSizex, float textureSizey,
		float scalex, float scaley, float movex, float movey) {
		float* second_vertices = new float[6 * 4];
		memcpy(second_vertices, vertices, 6 * 4 * sizeof(float));
		second_vertices[4 * 4 + 2] = textureFromx / textureSizex;
		second_vertices[4 * 4 + 3] = textureFromy / textureSizey;
		second_vertices[1 * 4 + 2] = textureTox / textureSizex;
		second_vertices[1 * 4 + 3] = textureToy / textureSizey;
		second_vertices[0 * 4 + 2] = second_vertices[5 * 4 + 2] = textureFromx / textureSizex;
		second_vertices[0 * 4 + 3] = second_vertices[5 * 4 + 3] = textureToy / textureSizey;
		second_vertices[2 * 4 + 2] = second_vertices[3 * 4 + 2] = textureTox / textureSizex;
		second_vertices[2 * 4 + 3] = second_vertices[3 * 4 + 3] = textureFromy / textureSizey;
		textureSizex *= 2;
		textureSizey *= 2;
		second_vertices[4 * 4 + 0] = -(textureTox - textureFromx) / textureSizex;
		second_vertices[4 * 4 + 1] = (textureToy - textureFromy) / textureSizey;
		second_vertices[1 * 4 + 0] = (textureTox - textureFromx) / textureSizex;
		second_vertices[1 * 4 + 1] = -(textureToy - textureFromy) / textureSizey;
		second_vertices[0 * 4 + 0] = second_vertices[5 * 4 + 0] = -(textureTox - textureFromx) / textureSizex;
		second_vertices[0 * 4 + 1] = second_vertices[5 * 4 + 1] = -(textureToy - textureFromy) / textureSizey;
		second_vertices[2 * 4 + 0] = second_vertices[3 * 4 + 0] = (textureTox - textureFromx) / textureSizex;
		second_vertices[2 * 4 + 1] = second_vertices[3 * 4 + 1] = (textureToy - textureFromy) / textureSizey;
		
		for (size_t i = 0; i < 6; i++) {
			second_vertices[i * 4 + 0] *= scalex;
			second_vertices[i * 4 + 1] *= scaley;
			second_vertices[i * 4 + 0] += movex;
    		second_vertices[i * 4 + 1] += movey;
		}
		return second_vertices;
	}
	void LoadInterface(TextureLoader* tl, TYPE TYPE, std::string texturePath, float textureFromx, float textureFromy, float textureTox, float textureToy, float textureSizex, float textureSizey,
		float scalex, float scaley, float movex, float movey) {
		interfaceVertices[int(TYPE)] = moveVertices(textureFromx, textureFromy, textureTox, textureToy, textureSizex, textureSizey,
			scalex, scaley, movex, movey);
		interfaces[int(TYPE)] = Load(tl, texturePath, interfaceVertices[int(TYPE)], VAOs[int(TYPE)], VBOs[int(TYPE)]);
	}
	Interface(TextureLoader* tl) {
		inventoryShader = new Shader("shaders\\inventoryShader.vert", "shaders\\inventoryShader.frag");
		inventoryShader->use();
		inventoryShader->setInt("inventory", 13);
		interfaceVertices = new float*[INTERFACE_TYPE_LEN];
		VAOs = new uint32_t[INTERFACE_TYPE_LEN];
		interfaces = new uint32_t[INTERFACE_TYPE_LEN];
		VBOs = new uint32_t[INTERFACE_TYPE_LEN];
		LoadInterface(tl, TYPE::INVENTORY, "resources\\minecraft\\gui\\container\\inventory.png", 1.0f, 1.0f, 352.0f, 332.0f, 512.0f, 512.0f, 1, 1, 0, 0);
		LoadInterface(tl, TYPE::INVENTORY_WIDGET, "resources\\minecraft\\gui\\widgets.png", 1.0f, 1.0f, 364.0f, 44.0f, 512.0f, 512.0f, 1, 1, 0, -0.5);
		LoadInterface(tl, TYPE::CURSOR, "resources\\minecraft\\gui\\icons.png", 6.0f, 6.0f, 24.0f, 24.0f, 512.0f, 512.0f, 0.75, 0.75, 0, 0);
	}

	void DrawInterface(TYPE type, uint32_t SCR_WIDTH, uint32_t SCR_HEIGHT) {
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE13);
		float aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
		this->inventoryShader->use();
		this->inventoryShader->setMat4("projection", glm::ortho(-1.0f, 1.0f, -1.0f / aspect, 1.0f / aspect, 0.1f, 10000.0f));
		glBindTexture(GL_TEXTURE_2D, interfaces[int(type)]);
		glBindVertexArray(VAOs[int(type)]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glEnable(GL_DEPTH_TEST);
	}
};

#endif