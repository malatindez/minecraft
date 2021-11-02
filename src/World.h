#ifndef WORLD_H
#define WORLD_H
#include "PerlinNoise.h"
#include "Block.h"
#include <direct.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <queue>
#include <algorithm>
#include "Camera.h"
class Block {
public:
	uint8_t* metadata = nullptr;
	size_t metadata_size;
	UBlock* ref;
	struct Coords {
		int32_t x, y, z;
		Coords(int32_t x, int32_t y, int32_t z);
	} coords;

	Block(uint8_t* data, size_t size, UBlock* block, Coords coords);
	Block(const Block& b);
	Block(const Block* b);
	~Block();
	
	uint32_t Break();

};
class Chunk {
	inline static float vertices[] = {
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

	std::vector<UBlock>* blocks = nullptr;
	float ***optimizedVertices = nullptr;
	uint32_t ***optimizedIndexes = nullptr;
	UBlock*** ovblocks = nullptr;
	uint32_t **VBO = nullptr;
	uint32_t **VAO = nullptr;
	uint32_t **EBO = nullptr;
	uint32_t **OVSizel = nullptr;
	uint32_t *ovsize = nullptr;
	bool optimized = false, optimized2 = false;
public:
	std::mutex canBeDeletedMutex;
	int32_t canBeDeleted = 0;
	std::mutex optimizeMutex;
	const int32_t x, z;
	std::pair<int32_t, int32_t> getCoords() {
		return std::pair<int32_t, int32_t>(x, z);
	}
	glm::mat4 getModel() {
		glm::mat4 model = glm::mat4(1.0f);
		return ::glm::translate(model, glm::vec3(x * 16, 0, z * 16));
	}
	enum class Exceptions {WRONG_COORDS};
/*
coordinates inside a chunk(only 1 byte)
integer_id - id of the block(4 bytes)
block_data_size(2 bytes)

*** All data is written in Little Endian                          ***
*** Except for data, data is written by block in any way it wants ***

file data:
// x  y  z    integer_id    block_data_size 
// 00 00 00   00 00 00 01   00 00 
// Data
00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00 }
00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00   }
00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00     } total size is block_data_size bytes
...                                                    }
...                                                  }
00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00 }

// x  y  z    integer_id    block_data_size
// 01 00 00   00 00 00 01   00 00
// Data
00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00 }
00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00   }
00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00     } total size is block_data_size bytes
...                                                    }
...                                                  }
00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00 }

Blocks, !included in this file(by x y z coordinates) are counted as minecraft:air(id is 0).
*/
	Block**** ChunkBlocks;
	std::queue<std::pair<uint32_t*, uint32_t>>* VAODeleteList = nullptr;
	std::queue<std::pair<uint32_t*, uint32_t>>* VBODeleteList = nullptr;
	std::mutex* VAO_VBOmutex;
	Chunk(uint8_t* data, uint32_t data_size, std::vector<UBlock>* blocks, int32_t x, int32_t z, std::queue<std::pair<uint32_t*, uint32_t>>*, std::queue<std::pair<uint32_t*, uint32_t>>*, std::mutex*);
	Chunk(const Chunk& b);
	Chunk(const Chunk* b);
	bool isOptimized() {
		return optimized;
	}
	bool isVerticesLoaded() {
		return optimized2;
	}
	void optimizeRenderer(Chunk** nearby);
	//nearby new Chunk*[4]
	//Chunk[0] - Chunk[x-1][z]
	//Chunk[1] - Chunk[x][z-1]
	//Chunk[2] - Chunk[x+1][z]
	//Chunk[3] - Chunk[x][z+1]
	void optimizeRenderer(uint16_t yoffset, Chunk** nearby);
	void loadVerticesToVideoMemory();
	void deleteOptimizedRenderer(uint16_t yoffset);
	void deleteOptimizedRenderer();
	void Save();
	bool PlaceBlock(int32_t x, int32_t y, int32_t z, UBlock* block, Chunk** nearby);
	// returns an item id
	void BreakBlock(int32_t x, int32_t y, int32_t z, Chunk** nearby);
	void Draw();
	Block* getBlock(int32_t x, int32_t y, int32_t z);
	~Chunk();
};
// std::pow(2, 32) - 1
#define pow2232m1 4294967295
class World {
private:
	std::vector<UBlock>* blocks = nullptr;
	TextureLoader* tloader = nullptr;
	std::string name;
	const uint64_t seed;
	enum class Exceptions {
		CHUNK_SIZE_OVER_16MB = 0
	};
	PerlinNoise noise;
	std::mutex chunksMutex;
public:
	std::vector <Chunk*> chunks;
private:
	int32_t renderDistance = 0;
	std::mutex playerCoordsMutex;
	Block::Coords playerCoords;
	World(TextureLoader* tloader, std::vector<UBlock>* blocks, std::string name, uint64_t s, uint32_t renderDistance) : seed(s), noise(s), playerCoords(0, 0, 0) {
		this->tloader = tloader;
		this->blocks = blocks;
		this->renderDistance = renderDistance;
	}
	bool stopThreads = false;
	std::mutex loadedChunksMutex;
	std::vector<std::pair<int32_t, int32_t>> loadedChunks;

	std::mutex generationQueueMutex;
	std::vector<std::pair<int32_t, int32_t>> generationQueue;
	void dynamicGenerationThread() {
		while (!stopThreads) {
			while (generationQueue.empty()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(250));
				if (stopThreads) {
					return;
				}
			}
			generationQueueMutex.lock();
			if (generationQueue.empty()) {
				generationQueueMutex.unlock();
				continue;
			}
			std::pair<int32_t, int32_t> coords = generationQueue.front(); generationQueue.erase(generationQueue.begin());
			generationQueueMutex.unlock();
			//loadedChunksMutex.lock();
			//if (std::find(loadedChunks.begin(), loadedChunks.end(), coords) == loadedChunks.end()) {
		//		loadedChunksMutex.unlock();
		//		continue;
		//	}
		//	loadedChunksMutex.unlock();
			this->loadChunk(coords.first, coords.second);
			optimizationQueueMutex.lock();
			optimizationQueue.push_back(std::pair<int32_t, int32_t>(coords.first, coords.second));
			optimizationQueueMutex.unlock();
		}
	}
	std::mutex deletionQueueMutex;
	std::vector<std::pair<int32_t, int32_t>> deletionQueue;
	void dynamicDeletionThread() {
		while (!stopThreads) {
			while (deletionQueue.empty()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(250));
				if (stopThreads) {
					return;
				}
			}
			deletionQueueMutex.lock();
			if (deletionQueue.empty()) {
				deletionQueueMutex.unlock();
				continue;
			}
			std::pair<int32_t, int32_t> coords = (*deletionQueue.begin());
			deletionQueue.erase(deletionQueue.begin());
			deletionQueueMutex.unlock();
			this->deleteChunk(coords.first, coords.second);
			/*if (! this->deleteChunk(coords.first, coords.second)) {
				deletionQueueMutex.lock(); 
				bool flag = false;
				for (auto itr2 = deletionQueue.begin(); itr2 != deletionQueue.end(); itr2++) {
					if (coords.first == (*itr2).first && coords.second == (*itr2).second) {
						flag = true; break;
					}
				}
				if (!flag) {
					deletionQueue.push_back(std::pair<int32_t, int32_t>(coords.first, coords.second));
				}
				deletionQueueMutex.unlock();
			}*/
			
		}
	}

	std::mutex optimizationQueueMutex;
	std::vector<std::pair<int32_t, int32_t>> optimizationQueue;
	void dynamicOptimizationThread() {
		while (!stopThreads) {
			while (optimizationQueue.empty()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(250));
				if (stopThreads) {
					return;
				}
			}
			optimizationQueueMutex.lock();
			if (optimizationQueue.empty()) {
				optimizationQueueMutex.unlock();
				continue;
			}
			std::pair<int32_t, int32_t> coords = optimizationQueue.front(); optimizationQueue.erase(optimizationQueue.begin());
			optimizationQueueMutex.unlock();
			loadedChunksMutex.lock();
			if (std::find(loadedChunks.begin(), loadedChunks.end(), coords) == loadedChunks.end()) {
				loadedChunksMutex.unlock();
				continue;
			}
			loadedChunksMutex.unlock();
			if (!this->optimizeRenderer(coords.first, coords.second)) {
				optimizationQueueMutex.lock();
				optimizationQueue.push_back(coords);
				optimizationQueueMutex.unlock();
			}
		}
	}
	void threadManipulator() {
		auto previous_coords = playerCoords;
		uint32_t i = 0;
		std::vector<std::pair<int32_t, int32_t>> doubleChecked;
		std::vector<std::pair<int32_t, int32_t>> checked;
		while (!stopThreads) {
			std::this_thread::sleep_for(std::chrono::milliseconds(25));
			playerCoordsMutex.lock();
			auto current_coords = playerCoords;
			playerCoordsMutex.unlock();
			std::vector<std::pair<int32_t, int32_t>> localloadedChunks;
			std::vector<std::pair<int32_t, int32_t>> localgenerationQueue;
			for (int32_t i = -renderDistance + current_coords.x / 16; i < renderDistance + current_coords.x / 16; i++) {
				for (int32_t j = -renderDistance + current_coords.z / 16; j < renderDistance + current_coords.z / 16; j++) {
					bool flag = false;
					for (auto itr = loadedChunks.begin(); itr != loadedChunks.end(); itr++) {
						if ((*itr).first == i && (*itr).second == j) {
							flag = true;
							break;
						}
					}
					if (!flag) {
						localloadedChunks.push_back(std::pair<int32_t, int32_t>(i, j));
						localgenerationQueue.push_back(std::pair<int32_t, int32_t>(i, j));
					}
				}
			}

			if (localgenerationQueue.size() > 0) {
				generationQueueMutex.lock();
				generationQueue.insert(generationQueue.end(), localgenerationQueue.begin(), localgenerationQueue.end());
				generationQueueMutex.unlock();
			}

			if (localloadedChunks.size() > 0) {
				loadedChunksMutex.lock();
				loadedChunks.insert(loadedChunks.end(), localloadedChunks.begin(), localloadedChunks.end());
				loadedChunksMutex.unlock();
			}
			
			if (i % 10 == 0) {
				std::vector<std::pair<int32_t, int32_t>> newLoadedChunks;
				bool flag = false;
				for (auto itr = loadedChunks.begin(); itr != loadedChunks.end(); itr++) {
					if ((*itr).first  < -renderDistance + current_coords.x / 16 ||
						(*itr).first  > renderDistance + current_coords.x / 16 ||
						(*itr).second < -renderDistance + current_coords.z / 16 ||
						(*itr).second > renderDistance + current_coords.z / 16) {
						deletionQueueMutex.lock();
						bool flag2 = false;
						for (auto itr2 = deletionQueue.begin(); itr2 != deletionQueue.end(); itr2++) {
							if ((*itr).first == (*itr2).first && (*itr).second == (*itr2).second) {
								flag2 = true; break;
							}
						}
						if (!flag2) {
							deletionQueue.push_back((*itr));
							std::remove(checked.begin(), checked.end(), (*itr));
							std::remove(doubleChecked.begin(), doubleChecked.end(), (*itr));
							flag = true;
						}
						deletionQueueMutex.unlock();
					}
					else {
						newLoadedChunks.push_back(std::pair<int32_t, int32_t>((*itr).first, (*itr).second));
					}
				}
				if (flag) {
					loadedChunksMutex.lock();
					loadedChunks = newLoadedChunks;
					loadedChunksMutex.unlock();
				}
			}
			if (i % 100 == 0) {
				this->chunksMutex.lock();
				for (auto itr = this->chunks.begin(); itr != this->chunks.end(); itr++) {
					if (std::find(loadedChunks.begin(), loadedChunks.end(), std::pair<int32_t, int32_t>((*itr)->x, (*itr)->z)) == loadedChunks.end()) {
						deletionQueueMutex.lock();
						deletionQueue.push_back(std::pair<int32_t, int32_t>((*itr)->x, (*itr)->z));
						loadedChunksMutex.lock();
						std::remove(loadedChunks.begin(), loadedChunks.end(), std::pair<int32_t, int32_t>((*itr)->x, (*itr)->z));
						std::remove(checked.begin(), checked.end(), std::pair<int32_t, int32_t>((*itr)->x, (*itr)->z));
						std::remove(doubleChecked.begin(), doubleChecked.end(), std::pair<int32_t, int32_t>((*itr)->x, (*itr)->z));
						loadedChunksMutex.unlock();
						deletionQueueMutex.unlock();
					}
				}
				this->chunksMutex.unlock();
			}
			if (i % 200 == 0) {
				std::vector<std::pair<int32_t, int32_t>> pairs;
				for (auto itr = loadedChunks.begin(); itr != loadedChunks.end(); itr++) {
					if (std::find(checked.begin(), checked.end(), (*itr)) == checked.end()) {
						pairs.push_back((*itr));
						checked.push_back((*itr));
					}
					else if (std::find(doubleChecked.begin(), doubleChecked.end(), (*itr)) == doubleChecked.end()) {
						pairs.push_back((*itr));
						doubleChecked.push_back((*itr));
					}
				}
				if (pairs.size() > 0) {
					optimizationQueueMutex.lock();
					optimizationQueue.insert(optimizationQueue.end(), pairs.begin(), pairs.end());
					optimizationQueueMutex.unlock();
				}
			}
			i++;
		}
	}
	std::thread *manipulator = nullptr, *generation = nullptr, *deletion = nullptr, *optimization = nullptr;
	uint32_t numberOfThreads = 0;
	std::queue<std::pair<uint32_t*, uint32_t>> deleteVAOs;
	std::queue<std::pair<uint32_t*, uint32_t>> deleteVBOs;
	std::mutex VAO_VBOmutex;
public:
	~World() {
		stopThreads = true;
		if (manipulator != nullptr) {
			manipulator->join();
		}
		
		for (size_t i = 0; i < numberOfThreads; i++) {
			if (generation != nullptr) {
				generation[i].join();
			}
			if (deletion != nullptr) {
				deletion[i].join();
			}
			if (optimization != nullptr) {
				optimization[i].join();
			}
		}
		for (std::vector<Chunk*>::iterator itr = chunks.begin(); itr != chunks.end(); itr++) {
			delete (*itr);
			chunks.erase(itr);
		}
	}
	bool optimizeRenderer(int32_t x, int32_t z) {
		Chunk* chunk = nullptr;
		Chunk** chunks = new Chunk * [4];
		chunks[0] = chunks[1] = chunks[2] = chunks[3] = nullptr;
		chunksMutex.lock();
		//nearby new Chunk*[4]
		//Chunk[0] - Chunk[x-1][z]
		//Chunk[1] - Chunk[x][z-1]
		//Chunk[2] - Chunk[x+1][z]
		//Chunk[3] - Chunk[x][z+1]
		for (auto itr = this->chunks.begin(); itr != this->chunks.end(); itr++) {
			(*itr)->canBeDeletedMutex.lock();
			if ((*itr)->canBeDeleted < 0) {
				(*itr)->canBeDeletedMutex.unlock();
				continue;
			}
			if ((*itr)->x == x && (*itr)->z == z) {
				(*itr)->canBeDeleted += 1;
				chunk = (*itr);
			}
			if ((*itr)->x == x - 1 && (*itr)->z == z) {
				(*itr)->canBeDeleted += 1;
				chunks[0] = (*itr);
			}
			if ((*itr)->x == x + 1 && (*itr)->z == z) {
				(*itr)->canBeDeleted += 1;
				chunks[2] = (*itr);
			}
			if ((*itr)->x == x && (*itr)->z == z - 1) {
				(*itr)->canBeDeleted += 1;
				chunks[1] = (*itr);
			}
			if ((*itr)->x == x && (*itr)->z == z + 1) {
				(*itr)->canBeDeleted += 1;
				chunks[3] = (*itr);
			}
			(*itr)->canBeDeletedMutex.unlock();
		}
		chunksMutex.unlock();
		bool returnValue = (chunk != nullptr);
		if (returnValue && (chunks[0] == nullptr || chunks[1] == nullptr && chunks[2] == nullptr || chunks[3] == nullptr)) {
			playerCoordsMutex.lock();
			auto current_coords = playerCoords;
			playerCoordsMutex.unlock();
			if (chunk->x > -renderDistance + current_coords.x / 16 ||
				chunk->x < renderDistance + current_coords.x / 16 ||
				chunk->z > -renderDistance + current_coords.z / 16 ||
				chunk->z < renderDistance + current_coords.z / 16) {
				returnValue = true;
			}
			else {
				returnValue = false;
			}
		}
		if (returnValue) {
			chunk->deleteOptimizedRenderer();
			chunk->optimizeRenderer(chunks);
		}
		if (chunk != nullptr) {
			chunk->canBeDeletedMutex.lock();
			chunk->canBeDeleted--;
			chunk->canBeDeletedMutex.unlock();
		}
		for (size_t i = 0; i < 4; i++) {
			if (chunks[i] != nullptr) {
				chunks[i]->canBeDeletedMutex.lock();
				chunks[i]->canBeDeleted--;
				chunks[i]->canBeDeletedMutex.unlock();
			}
		}
		delete[] chunks;
		return returnValue;
	}
	void updateVertices() {
		chunksMutex.lock();
		for (auto itr = chunks.begin(); itr != chunks.end(); itr++) {
			if ((*itr)->isOptimized() && !(*itr)->isVerticesLoaded()) {
				(*itr)->loadVerticesToVideoMemory();
			}
		}
		chunksMutex.unlock();
		if (!deleteVAOs.empty()) {
			VAO_VBOmutex.lock();
			auto vao = deleteVAOs.front();  deleteVAOs.pop();
			auto vbo = deleteVBOs.front();  deleteVBOs.pop();
			VAO_VBOmutex.unlock();
			for (size_t i = 0; i < vao.second; i++) {
				glDeleteVertexArrays(1, vao.first);
				glDeleteBuffers(1, vao.first);
			}
			delete[] vao.first;
			delete[] vbo.first;
		}
	}
	void startThreads(uint32_t numberOfThreads) {
		this->numberOfThreads = numberOfThreads;
		manipulator = new std::thread(&World::threadManipulator, this);
		generation = new std::thread[numberOfThreads];
		optimization = new std::thread[numberOfThreads];
		deletion = new std::thread[numberOfThreads];
		for (size_t i = 0; i < numberOfThreads; i++) {
			generation[i] = std::thread(&World::dynamicGenerationThread, this);
			optimization[i] = std::thread(&World::dynamicOptimizationThread, this);
			deletion[i] = std::thread(&World::dynamicDeletionThread, this);
		}
	}
	void Draw(Shader* shader) {
		chunksMutex.lock();
		for (auto itr = chunks.begin(); itr != chunks.end(); itr++) {
			shader->setMat4("model", (*itr)->getModel());
			(*itr)->Draw();
		}
		chunksMutex.unlock();
	}
	Block* getBlock(int32_t x, int32_t y, int32_t z) {
		if (y < 0 || y > 255) {
			return nullptr;
		}
		chunksMutex.lock();
		for (std::vector<Chunk*>::iterator itr = chunks.begin(); itr != chunks.end(); itr++) {
			if (((*itr)->x * 16 < x && x < (*itr)->x * 16 + 16) && ((*itr)->z * 16 < z && z < (*itr)->z * 16 + 16)) {
				auto returnValue = (*itr)->getBlock(x, y, z);
				chunksMutex.unlock();
				return returnValue;
			}
		}
		chunksMutex.unlock();
		return nullptr;
	}
	void updatePlayerCoords(Block::Coords coords) {
		playerCoordsMutex.lock();
		this->playerCoords = coords;
		playerCoordsMutex.unlock();
	}
	bool breakBlock(int32_t x, int32_t y, int32_t z) {
		chunksMutex.lock();
		//Chunk[0] - Chunk[x-1][z]
		//Chunk[1] - Chunk[x][z-1]
		//Chunk[2] - Chunk[x+1][z]
		//Chunk[3] - Chunk[x][z+1]
		/*
		Chunk** cs = new Chunk*[4];
		cs[0] = cs[1] = cs[2] = cs[3] = nullptr;
		Chunk* cref = nullptr;
		for (std::vector<Chunk*>::iterator itr = chunks.begin(); itr != chunks.end(); itr++) {

			if (((*itr)->x * 16 < x && x < (*itr)->x * 16 + 16) && ((*itr)->z * 16 < z && z < (*itr)->z * 16 + 16)) {
				cref = (*itr);
			}

			if (((*itr)->x * 16 < x - 16 && x - 16 < (*itr)->x * 16 + 16) && ((*itr)->z * 16 < z && z < (*itr)->z * 16 + 16)) {
				cs[0] = (*itr);
			}
			if (((*itr)->x * 16 < x + 16 && x + 16 < (*itr)->x * 16 + 16) && ((*itr)->z * 16 < z && z < (*itr)->z * 16 + 16)) {
				cs[2] = (*itr);
			}
			if (((*itr)->x * 16 < x && x < (*itr)->x * 16 + 16) && ((*itr)->z * 16 < z - 16 && z - 16 < (*itr)->z * 16 + 16)) {
				cs[1] = (*itr);
			}
			if (((*itr)->x * 16 < x && x < (*itr)->x * 16 + 16) && ((*itr)->z * 16 < z + 16 && z + 16 < (*itr)->z * 16 + 16)) {
				cs[3] = (*itr);
			}
		}
		if (cref != nullptr) {
			try {
				cref->BreakBlock(x, y, z, cs);
			}
			catch (Chunk::Exceptions exception) {
				if (exception == Chunk::Exceptions::WRONG_COORDS) {
					delete[] cs;
					chunksMutex.unlock();
					return false;
				}
			}
		}
		optimizationQueueMutex.lock();
		for (size_t i = 0; i < 4; i++) {
			if(cs[i] != nullptr)
			optimizationQueue.push(std::pair<int32_t, int32_t>(cs[i]->x, cs[i]->z));
		}
		optimizationQueue.push(std::pair<int32_t, int32_t>(cref->x, cref->z));
		optimizationQueueMutex.unlock();
		delete[] cs;*/
		chunksMutex.unlock();
		return true;
	}

	bool placeBlock(int32_t x, int32_t y, int32_t z, UBlock* block) {
		chunksMutex.lock();
		/*
		//Chunk[0] - Chunk[x-1][z]
		//Chunk[1] - Chunk[x][z-1]
		//Chunk[2] - Chunk[x+1][z]
		//Chunk[3] - Chunk[x][z+1]
		Chunk** cs = new Chunk * [4];
		cs[0] = cs[1] = cs[2] = cs[3] = nullptr;
		Chunk* cref = nullptr;
		for (std::vector<Chunk*>::iterator itr = chunks.begin(); itr != chunks.end(); itr++) {

			if (((*itr)->x * 16 < x && x < (*itr)->x * 16 + 16) && ((*itr)->z * 16 < z && z < (*itr)->z * 16 + 16)) {
				cref = (*itr);
			}

			if (((*itr)->x * 16 < x - 16 && x - 16 < (*itr)->x * 16 + 16) && ((*itr)->z * 16 < z && z < (*itr)->z * 16 + 16)) {
				cs[0] = (*itr);
			}
			if (((*itr)->x * 16 < x + 16 && x + 16 < (*itr)->x * 16 + 16) && ((*itr)->z * 16 < z && z < (*itr)->z * 16 + 16)) {
				cs[2] = (*itr);
			}
			if (((*itr)->x * 16 < x && x < (*itr)->x * 16 + 16) && ((*itr)->z * 16 < z - 16 && z - 16 < (*itr)->z * 16 + 16)) {
				cs[1] = (*itr);
			}
			if (((*itr)->x * 16 < x && x < (*itr)->x * 16 + 16) && ((*itr)->z * 16 < z + 16 && z + 16 < (*itr)->z * 16 + 16)) {
				cs[3] = (*itr);
			}
		}
		if (cref != nullptr) {
			bool x = false;
			x = cref->PlaceBlock(x, y, z, block, cs);
			if (!x) {
				chunksMutex.unlock();
				return false;
			}
		}
		optimizationQueueMutex.lock();
		for (size_t i = 0; i < 4; i++) {
			if (cs[i] != nullptr)
				optimizationQueue.push(std::pair<int32_t, int32_t>(cs[i]->x, cs[i]->z));
		}
		optimizationQueue.push(std::pair<int32_t, int32_t>(cref->x, cref->z));
		optimizationQueueMutex.unlock();
		delete[] cs;*/
		chunksMutex.unlock();
		return true;
	}
	Chunk* getChunk(int32_t x, int32_t z) {
		chunksMutex.lock();
		for (std::vector<Chunk*>::iterator itr = chunks.begin(); itr != chunks.end(); itr++) {
			if ((*itr)->x == x && (*itr)->z == z) {
				chunksMutex.unlock();
				return (*itr);
			}
		}
		chunksMutex.unlock();
		return nullptr;
	}
	bool deleteChunk(int32_t x, int32_t z) {
		chunksMutex.lock();
		for (std::vector<Chunk*>::iterator itr = chunks.begin(); itr != chunks.end(); itr++) {
			if ((*itr)->x == x && (*itr)->z == z) {
				(*itr)->canBeDeletedMutex.lock();
				if ((*itr)->canBeDeleted == 0) {
					(*itr)->canBeDeleted = -1000;
					(*itr)->optimizeMutex.lock();
					(*itr)->optimizeMutex.unlock();
					(*itr)->canBeDeletedMutex.unlock();
					delete (*itr);
					chunks.erase(itr);
					chunksMutex.unlock();
					return true;
				}
				else {
					(*itr)->canBeDeletedMutex.unlock();
				}
			}
		}
		chunksMutex.unlock();
		return false;
	}
	void generateChunk(int32_t x, int32_t z) {
		uint32_t size = 16 * 16 * 9;
		uint8_t* gdata = new uint8_t[size];
		memset(gdata, 0, size);
		for (uint8_t i = 0; i < 16; i++) {
			for (uint8_t j = 0; j < 16; j++) {
				uint32_t height = (uint32_t)(30 + 10.0 * noise.noise((double(x) * 16 + i) / 16, 0, (double(z) * 16 + j) / 16));
				if (height > 255) {
					height = 255;
				}
				uint8_t* data = new uint8_t[height * 9];
				memset(data, 0, height * 9);
				for (uint8_t k = 0; k < uint8_t(height); k++) {
					data[(k) * 9 + 0] = i;
					data[(k) * 9 + 1] = k;
					data[(k) * 9 + 2] = j;
					if (k < 1 + rand() % 2 + rand() % 2 + rand() % 3) {
						((uint32_t*)(&(data[(k) * 9 + 3])))[0] = 4;
					}
					else if (k < height - 4) {
						((uint32_t*)(&(data[(k) * 9 + 3])))[0] = 1;
					}
					else if (k < height - 1) {
						((uint32_t*)(&(data[(k) * 9 + 3])))[0] = 2;
					}
					else {
						((uint32_t*)(&(data[(k) * 9 + 3])))[0] = 3;
					}
				}
				uint8_t* buf = new uint8_t[size + height * 9];
				memcpy(buf, gdata, size);
				memcpy(buf + size, data, height * 9);
				size = size + height * 9;
				delete[] gdata;
				delete[] data;
				gdata = buf;
			}
		}
		Chunk* returnValue = new Chunk(gdata, size, blocks, x, z, &deleteVAOs, &deleteVBOs, &VAO_VBOmutex);
		delete[] gdata;
		chunksMutex.lock();
		chunks.push_back(returnValue);
		chunksMutex.unlock();
	}
	void loadChunk(int32_t x, int32_t z) {
		std::string chunk_name = std::to_string(x) +  "," + std::to_string(z) + ".chunk";
		::std::ifstream chunk("saves\\" + name + "\\chunks\\" + chunk_name, std::ifstream::binary | std::ifstream::ate);
		Chunk* returnValue = nullptr;
		if (chunk.is_open()) {
			uint32_t chunk_size = (uint32_t)chunk.tellg();
			chunk.close();
			if (chunk_size > 16777216) { // chunk size is over 16 MB, that means that chunk is probably corrupted
				throw Exceptions::CHUNK_SIZE_OVER_16MB;
			}
			uint8_t* data = new uint8_t[chunk_size];
			chunk.get((char*)data, chunk_size);
			chunk.close();
			returnValue = new Chunk(data, chunk_size, blocks, x, z, &deleteVAOs, &deleteVBOs, &VAO_VBOmutex);
			delete[] data;
		} else {
			generateChunk(x, z);
			return;
		}

		chunksMutex.lock();
		chunks.push_back(returnValue);
		chunksMutex.unlock();
	}
	static World NewWorld(TextureLoader *t, std::vector<UBlock>* blocks, std::string name, uint32_t renderDistance) {
		_mkdir("saves"); // if save folder is !created, we shall do this 
		_mkdir(("saves\\" + name).c_str());
		_mkdir(("saves\\" + name + "\\chunks").c_str());
		::std::ofstream manifest("saves\\" + name + "\\MANIFEST");
		uint8_t* tempSeed = new uint8_t[8]; // 8 byte seed(same as uint64_t)
		for (uint8_t i = 0; i < 8; i++) {
			tempSeed[i] = rand();
			manifest << tempSeed[i];
		}
		uint64_t seed = (*(uint64_t*)tempSeed);
		delete[] tempSeed;
		manifest.close();
		return World(t, blocks, name, seed, renderDistance);
	}
	static World OpenWorld(TextureLoader* t, std::vector<UBlock>* blocks, std::string name, uint32_t renderDistance) {
		uint64_t seed;
		::std::ifstream manifest("saves\\" + name + "\\MANIFEST", std::ifstream::in | std::ifstream::binary);
		uint8_t* b = new uint8_t[8];
		manifest.get((char*)b, 8);
		seed = (*(uint64_t*)b);
		delete[] b;
		manifest.close();
		return World(t, blocks, name, seed, renderDistance);
	}
};


Block::Coords::Coords(int32_t x, int32_t y, int32_t z) {
	this->x = x;
	this->y = y;
	this->z = z;
}
Block::Block(uint8_t* data, size_t size, UBlock* block, Coords coords) : coords(coords) {
	this->metadata_size = size;
	if (size != 0) {
		this->metadata = new uint8_t[size];
		memcpy(this->metadata, data, size);
	}
	this->ref = block;
}
Block::Block(const Block& b) : coords(b.coords) {
	this->metadata_size = b.metadata_size;
	if (b.metadata_size != 0) {
		this->metadata = new uint8_t[b.metadata_size];
		memcpy(this->metadata, b.metadata, b.metadata_size);
	}
	this->ref = b.ref;
	this->coords = b.coords;
}
Block::Block(const Block* b) : coords(b->coords) {
	this->metadata_size = b->metadata_size;
	if (b->metadata_size != 0) {
		this->metadata = new uint8_t[b->metadata_size];
		memcpy(this->metadata, b->metadata, b->metadata_size);
	}
	this->ref = b->ref;
	this->coords = b->coords;
}
Block::~Block() {
	if (metadata != nullptr) {
		delete[] metadata;
	}
}

uint32_t Block::Break() { // returns an item id which should be dropped
	return ref->integer_id;
}

Chunk::Chunk(uint8_t* data, uint32_t data_size, std::vector<UBlock>* blocks, int32_t x, int32_t z, std::queue<std::pair<uint32_t*, uint32_t>>* vaos, std::queue<std::pair<uint32_t*, uint32_t>>* vbos, std::mutex* mutex) : x(x), z(z) {
	this->blocks = blocks;
	this->VAODeleteList = vaos;
	this->VBODeleteList = vbos;
	this->VAO_VBOmutex = mutex;
	uint32_t offset = 0;
	ChunkBlocks = new Block * **[16];
	for (size_t i = 0; i < 16; i++) {
		ChunkBlocks[i] = new Block * *[256];
		for (size_t j = 0; j < 256; j++) {
			ChunkBlocks[i][j] = new Block * [16];
			for (size_t k = 0; k < 16; k++) {
				ChunkBlocks[i][j][k] = nullptr;
			}
		}
	}
	while (offset + 8 < data_size) {
		uint8_t x = data[offset++];
		uint8_t y = data[offset++];
		uint8_t z = data[offset++];
		uint32_t id = 0;
		memcpy(&id, &(data[offset]), 4);
		offset += 4;
		uint16_t block_data_size = 0;
		memcpy(&block_data_size, &(data[offset]), 2);
		offset += 2;
		ChunkBlocks[x][y][z] = new Block(&(data[offset]), block_data_size, &((*blocks)[id]), Block::Coords(this->x * 16 + x, y, this->z * 16 + z));
	}
}
Chunk::Chunk(const Chunk& b) : x(b.x), z(b.z) {
	this->blocks = b.blocks;
	this->VAODeleteList = b.VAODeleteList;
	this->VBODeleteList = b.VBODeleteList;
	this->VAO_VBOmutex = b.VAO_VBOmutex;
	ChunkBlocks = new Block * **[16];
	for (size_t i = 0; i < 16; i++) {
		ChunkBlocks[i] = new Block * *[256];
		for (size_t j = 0; j < 256; j++) {
			ChunkBlocks[i][j] = new Block * [16];
			for (size_t k = 0; k < 16; k++) {
				if (b.ChunkBlocks[i][j][k] != nullptr) {
					ChunkBlocks[i][j][k] = new Block(b.ChunkBlocks[i][j][k]);
				}
				else {
					ChunkBlocks[i][j][k] = nullptr;
				}
			}
		}
	}
}
Chunk::Chunk(const Chunk* b) : x(x), z(z) {
	this->blocks = b->blocks;
	this->VAODeleteList = b->VAODeleteList;
	this->VBODeleteList = b->VBODeleteList;
	this->VAO_VBOmutex = b->VAO_VBOmutex;
	ChunkBlocks = new Block * **[16];
	for (size_t i = 0; i < 16; i++) {
		ChunkBlocks[i] = new Block * *[256];
		for (size_t j = 0; j < 256; j++) {
			ChunkBlocks[i][j] = new Block * [16];
			for (size_t k = 0; k < 16; k++) {
				if (b->ChunkBlocks[i][j][k] != nullptr) {
					ChunkBlocks[i][j][k] = new Block(b->ChunkBlocks[i][j][k]);
				}
				else {
					ChunkBlocks[i][j][k] = nullptr;
				}
			}
		}
	}
}

Chunk::~Chunk() {
	deleteOptimizedRenderer();
	if (ChunkBlocks == nullptr) {
		return;
	}
	for (size_t i = 0; i < 16; i++) {
		for (size_t j = 0; j < 256; j++) {
			for (size_t k = 0; k < 16; k++) {
				if (ChunkBlocks[i][j][k] != nullptr) {
					delete ChunkBlocks[i][j][k];
				}
			}
		}
	}
	for (size_t i = 0; i < 16; i++) {
		for (size_t j = 0; j < 256; j++) {
			delete[] ChunkBlocks[i][j];
		}
	}
	for (size_t i = 0; i < 16; i++) {
		delete[] ChunkBlocks[i];
	}
	delete[] ChunkBlocks;
	ChunkBlocks = nullptr;
}
void Chunk::optimizeRenderer(Chunk **nearby) {
	optimizeMutex.lock();
	if (optimized) {
		optimizeMutex.unlock();
		return;
	}
	optimizedVertices = new float** [16];
	ovblocks = new UBlock * *[16];
	VBO = new uint32_t * [16];
	VAO = new uint32_t * [16];
	EBO = new uint32_t * [16];
	OVSizel = new uint32_t * [16];
	ovsize = new uint32_t[16];
	for (uint16_t i = 0; i < 16; i++) {
		optimizeRenderer(i, nearby);
	}
	optimized = true;
	optimizeMutex.unlock();
}

//nearby new Chunk*[4]
//Chunk[0] - Chunk[x-1][z]
//Chunk[1] - Chunk[x][z-1]
//Chunk[2] - Chunk[x+1][z]
//Chunk[3] - Chunk[x][z+1]
void Chunk::optimizeRenderer(uint16_t yoffset, Chunk** nearby) {
	std::vector<std::pair<UBlock*, std::vector<std::pair<Block*, bool*>>>> blocks;
	// second is number of this blocks in chunk
	for (int32_t i = 0; i < 16; i++) {
		for (int32_t j = 16 * yoffset; j < 16 * (yoffset + 1); j++) {
			for (int32_t k = 0; k < 16; k++) {
				if (ChunkBlocks[i][j][k] != nullptr) {
					// if block nearby is minecraft:air - that means that we should process this block
					// otherwise we just skip it
					bool* statements = new bool[6]; // tiles which we should render
					if (k > 0) {
						statements[0] = (ChunkBlocks[i][j][k - 1] == nullptr);
					}
					else {
						statements[0] = (nearby[1] == nullptr || (nearby[1]->ChunkBlocks[i][j][15] == nullptr));
					}
					if (k < 15) {
						statements[1] = (ChunkBlocks[i][j][k + 1] == nullptr);
					}
					else {
						statements[1] = (nearby[3] == nullptr || (nearby[3]->ChunkBlocks[i][j][0] == nullptr));
					}
					if (i > 0) {
						statements[2] = (ChunkBlocks[i - 1][j][k] == nullptr);
					}
					else {
						statements[2] = (nearby[0] == nullptr || (nearby[0]->ChunkBlocks[15][j][k] == nullptr));
					}
					if (i < 15) {
						statements[3] = (ChunkBlocks[i + 1][j][k] == nullptr);
					}
					else {
						statements[3] = (nearby[2] == nullptr || (nearby[2]->ChunkBlocks[0][j][k] == nullptr));
					}
					if (j > 0) {
						statements[4] = (ChunkBlocks[i][j - 1][k] == nullptr);
					}
					else {
						statements[4] = true;
					}
					if (j < 255) {
						statements[5] = (ChunkBlocks[i][j + 1][k] == nullptr);
					}
					else {
						statements[5] = true;
					}
					if (statements[0] || statements[1] || statements[2] || statements[3] || statements[4] || statements[5]) {
						bool flag = false;
						for (auto itr = blocks.begin(); itr != blocks.end(); itr++) {
							if ((*itr).first->integer_id == ChunkBlocks[i][j][k]->ref->integer_id) {
								(*itr).second.push_back(std::pair<Block*, bool*>(ChunkBlocks[i][j][k], statements));
								flag = true;
								break;
							}
						}
						if (!flag) {
							blocks.push_back(std::pair<UBlock*, std::vector<std::pair<Block*, bool*>>>(
								ChunkBlocks[i][j][k]->ref, std::vector<std::pair<Block*, bool*>>()));
							blocks[blocks.size() - 1].second.push_back(std::pair<Block*, bool*>(ChunkBlocks[i][j][k], statements));
						}
					}
					else {
						delete[] statements;
					}
				}
			}
		}
	}
	ovsize[yoffset] = blocks.size();
	OVSizel[yoffset] = new uint32_t[blocks.size()];
	optimizedVertices[yoffset] = new float* [blocks.size()];
	ovblocks[yoffset] = new UBlock * [blocks.size()];
	VBO[yoffset] = new uint32_t[blocks.size()];
	VAO[yoffset] = new uint32_t[blocks.size()];
#define PUSH_VERTICES(x, coords) vertices.push_back(std::pair<uint32_t, Block::Coords>(x, coords))
#define PUSH_ALL_VERTICES(offset, coords) PUSH_VERTICES(6 * offset, coords);  PUSH_VERTICES(6*offset+1, coords);  PUSH_VERTICES(6 * offset + 2, coords); \
									PUSH_VERTICES(6 * offset + 3, coords); PUSH_VERTICES(6 * offset + 4, coords); PUSH_VERTICES(6 * offset + 5, coords);
	int m = 0;
	for (auto a = blocks.begin(); a != blocks.end(); a++, m++) {
		std::vector<std::pair<uint32_t, Block::Coords>> vertices; // this->vertices[vertices[i]] -> this->vertices[vertices[i] + 9] - vertice data
		for (auto blockItr = (*a).second.begin(); blockItr != (*a).second.end(); blockItr++) {
			for (size_t i = 0; i < 6; i++) {
				if ((*blockItr).second[i]) {
					PUSH_ALL_VERTICES(i, (*blockItr).first->coords);
				}
			}
			delete[](*blockItr).second;
		}
		float* x = new float[vertices.size() * 9];
		int i = 0;
		for (auto itr = vertices.begin(); itr != vertices.end(); itr++, i++) {
			x[i * 9] = this->vertices[9 * (*itr).first] + (*itr).second.x - this->x * 16;
			x[i * 9 + 1] = this->vertices[9 * (*itr).first + 1] + (*itr).second.y;
			x[i * 9 + 2] = this->vertices[9 * (*itr).first + 2] + (*itr).second.z - this->z * 16;
			for (size_t j = 3; j < 9; j++) {
				x[i * 9 + j] = this->vertices[9 * (*itr).first + j];
			}
		}
		OVSizel[yoffset][m] = vertices.size() * 9;
		optimizedVertices[yoffset][m] = x;
		ovblocks[yoffset][m] = a->first;
	}

}
void Chunk::loadVerticesToVideoMemory() {
	optimizeMutex.lock();
	for (size_t yoffset = 0; yoffset < 16; yoffset++) {
		for (size_t i = 0; i < ovsize[yoffset]; i++) {
			glGenVertexArrays(1, &(VAO[yoffset][i]));
			glGenBuffers(1, &(VBO[yoffset][i]));

			glBindBuffer(GL_ARRAY_BUFFER, VBO[yoffset][i]);
			glBufferData(GL_ARRAY_BUFFER, OVSizel[yoffset][i] * sizeof(float), optimizedVertices[yoffset][i], GL_STATIC_DRAW);

			glBindVertexArray(VAO[yoffset][i]);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(8 * sizeof(float)));
			glEnableVertexAttribArray(3);
		}
	}
	optimized2 = true;
	optimizeMutex.unlock();
}
void Chunk::deleteOptimizedRenderer(uint16_t yoffset) {
	for (size_t i = 0; i < ovsize[yoffset]; i++) {
		delete[] optimizedVertices[yoffset][i];
	}

	if (optimized2) {
		this->VAO_VBOmutex->lock();
		this->VAODeleteList->push(std::pair<uint32_t*, uint32_t>((VAO[yoffset]), ovsize[yoffset]));
		this->VBODeleteList->push(std::pair<uint32_t*, uint32_t>((VBO[yoffset]), ovsize[yoffset]));
		this->VAO_VBOmutex->unlock();
	}
	delete[] OVSizel[yoffset];
	delete[] ovblocks[yoffset];
	delete[] optimizedVertices[yoffset];
}
void Chunk::deleteOptimizedRenderer() {
	this->optimizeMutex.lock();
	if (optimized) {
		for (uint16_t j = 0; j < 16; j++) {
			deleteOptimizedRenderer(j);
		}
		delete[] VAO;
		delete[] VBO;
		delete[] OVSizel;
		delete[] optimizedVertices;
		delete[] ovblocks;
	}
	optimized = false;
	this->optimizeMutex.unlock();
	if (optimized2) {
		optimized2 = false;
	}
}
void Chunk::Save() {

}
bool Chunk::PlaceBlock(int32_t x, int32_t y, int32_t z, UBlock* block, Chunk** nearby) {
	x -= this->x * 16;
	z -= this->z * 16;
	if (ChunkBlocks[x][y][z] != nullptr) {
		return false;
	}
	ChunkBlocks[x][y][z] = new Block(nullptr, 0, block, Block::Coords(x + this->x * 16, y, z + this->z * 16));
	deleteOptimizedRenderer(y / 16);
	optimizeRenderer(y / 16, nearby);
	return true;
}
// returns an item id
void Chunk::BreakBlock(int32_t x, int32_t y, int32_t z, Chunk** nearby) {
	x -= this->x * 16;
	z -= this->z * 16;
	if (this->ChunkBlocks[x][y][z] == nullptr) {
		throw Exceptions::WRONG_COORDS;
	}
	uint32_t returnValue = this->ChunkBlocks[x][y][z]->Break();
	delete this->ChunkBlocks[x][y][z];
	this->ChunkBlocks[x][y][z] = nullptr;
	deleteOptimizedRenderer(y / 16);
	optimizeRenderer(y / 16, nearby);

}
void Chunk::Draw() {
	this->optimizeMutex.lock();
	if (optimized && optimized2) {
		for (size_t j = 0; j < 16; j++) {
			for (size_t i = 0; i < ovsize[j]; i++) {
				ovblocks[j][i]->BindTextures();
				glBindVertexArray(VAO[j][i]);
				glDrawArrays(GL_TRIANGLES, 0, OVSizel[j][i]);
			}
		}
	}
	this->optimizeMutex.unlock();
}
Block* Chunk::getBlock(int32_t x, int32_t y, int32_t z) {
	return ChunkBlocks[x - this->x * 16][y][z - this->z * 16];
}
#endif
// 0 0 0  
// 1 1 1
// 2 2 2
//
//
//