#ifndef WORLD_H
#define WORLD_H
#include "PerlinNoise.h"
#include "Block.h"
#include <direct.h>
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
	UBlock*** ovblocks = nullptr;
	uint32_t **VBO = nullptr;
	uint32_t **VAO = nullptr;
	uint32_t **OVSizel = nullptr;
	uint32_t *ovsize = nullptr;
	bool optimized = false;
public:
	const int32_t x, z;
	std::pair<int32_t, int32_t> getCoords() {
		return std::pair<int32_t, int32_t>(x, z);
	}
	glm::mat4 getModel() {
		glm::mat4 model = glm::mat4(1.0f);
		return ::glm::translate(model, glm::vec3(x * 16, 0, z * 16));
	}
	enum Exceptions {WRONG_COORDS};
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

Blocks, not included in this file(by x y z coordinates) are counted as minecraft:air(id is 0).
*/
	Block**** ChunkBlocks;
	Chunk(uint8_t* data, uint32_t data_size, std::vector<UBlock>* blocks, int32_t x, int32_t z);
	Chunk(const Chunk& b);
	Chunk(const Chunk* b);
	void optimizeRenderer(Chunk** nearby);
	//nearby new Chunk*[4]
	//Chunk[0] - Chunk[x-1][z]
	//Chunk[1] - Chunk[x][z-1]
	//Chunk[2] - Chunk[x+1][z]
	//Chunk[3] - Chunk[x][z+1]
	void optimizeRenderer(uint16_t yoffset, Chunk** nearby);
	void deleteOptimizedRenderer(uint16_t yoffset);
	void deleteOptimizedRenderer();
	void Save();
	void PlaceBlock(int32_t x, int32_t y, int32_t z, UBlock* block, Chunk** nearby);
	// returns an item id
	void BreakBlock(int32_t x, int32_t y, int32_t z, Chunk** nearby);
	void Draw();
	Block* getBlock(uint8_t x, uint8_t y, uint8_t z);
	~Chunk();
};
// std::pow(2, 32) - 1
#define pow2232m1 4294967295
class World {
public:
	std::vector<UBlock>* blocks = nullptr;
	TextureLoader* tloader = nullptr;
	std::string name;
	const uint64_t seed;
	enum class Exceptions {
		CHUNK_SIZE_OVER_16MB = 0
	};
	PerlinNoise noise;
	std::vector <Chunk*> chunks;
private:
	World(TextureLoader* tloader, std::vector<UBlock>* blocks, std::string name, uint64_t s) : seed(s), noise(s) {
		this->tloader = tloader;
		this->blocks = blocks;
	}
public:
	Block* getBlock(int32_t x, int32_t y, int32_t z) {
		for (std::vector<Chunk*>::iterator itr = chunks.begin(); itr != chunks.end(); itr++) {
			if (((*itr)->x * 16 < x and x < (*itr)->x * 16 + 16) and ((*itr)->z * 16 < z and z < (*itr)->z * 16 + 16)) {
				return (*itr)->getBlock(x, y, z);
			}
		}
	}
	void breakBlock(int32_t x, int32_t y, int32_t z) {
		for (std::vector<Chunk*>::iterator itr = chunks.begin(); itr != chunks.end(); itr++) {
			if (((*itr)->x * 16 < x and x < (*itr)->x * 16 + 16) and ((*itr)->z * 16 < z and z < (*itr)->z * 16 + 16)) {
				//(*itr)->BreakBlock(x, y, z);
			}
		}
	}

	void placeBlock(int32_t x, int32_t y, int32_t z, UBlock* block) {
		for (std::vector<Chunk*>::iterator itr = chunks.begin(); itr != chunks.end(); itr++) {
			if (((*itr)->x * 16 < x and x < (*itr)->x * 16 + 16) and ((*itr)->z * 16 < z and z < (*itr)->z * 16 + 16)) {
				//(*itr)->PlaceBlock(x, y, z, block);

			}
		}
	}
	Chunk* getChunk(int32_t x, int32_t z) {
		for (std::vector<Chunk*>::iterator itr = chunks.begin(); itr != chunks.end(); itr++) {
			if ((*itr)->x == x and (*itr)->z == z) {
				return (*itr);
			}
		}
		return nullptr;
	}
	void loadChunk(int32_t x, int32_t z) {
		x /= 16; z /= 16;
		std::string chunk_name = std::to_string(x) +  "," + std::to_string(z) + ".chunk";
		::std::ifstream chunk("saves\\" + name + "\\chunks\\" + chunk_name, std::ifstream::binary | std::ifstream::ate);
		uint32_t chunk_size = (uint32_t)chunk.tellg();
		if (chunk_size > 16777216) { // chunk size is over 16 MB, that means that chunk is probably corrupted
			throw Exceptions::CHUNK_SIZE_OVER_16MB;
		}
		uint8_t* data = new uint8_t[chunk_size];
		chunk.get((char*)data, chunk_size);
		chunk.close();
		Chunk* returnValue = new Chunk(data, chunk_size, blocks, x, z);
		delete[] data;
		chunks.push_back(returnValue);
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
				uint8_t* data = new uint8_t[height*9];
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
				gdata = buf;
			}
		}
		Chunk* returnValue = new Chunk(gdata, size, blocks, x, z);
		delete[] gdata;
		chunks.push_back(returnValue);
	}
	static World NewWorld(TextureLoader *t, std::vector<UBlock>* blocks, std::string name) {
		_mkdir("saves"); // if save folder is not created, we shall do this 
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
		return World(t, blocks, name, seed);
	}
	static World OpenWorld(TextureLoader* t, std::vector<UBlock>* blocks, std::string name) {
		uint64_t seed;
		::std::ifstream manifest("saves\\" + name + "\\MANIFEST", std::ifstream::in | std::ifstream::binary);
		uint8_t* b = new uint8_t[8];
		manifest.get((char*)b, 8);
		seed = (*(uint64_t*)b);
		delete[] b;
		manifest.close();
		return World(t, blocks, name, seed);
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



Chunk::Chunk(uint8_t* data, uint32_t data_size, std::vector<UBlock>* blocks, int32_t x, int32_t z) : x(x), z(z) {
	this->blocks = blocks;
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
void Chunk::optimizeRenderer(Chunk **nearby) {
	optimizedVertices = new float** [16];
	ovblocks = new UBlock * *[16];
	VBO = new uint32_t * [16];
	VAO = new uint32_t * [16];
	OVSizel = new uint32_t * [16];
	ovsize = new uint32_t[16];
	for (uint16_t i = 0; i < 16; i++) {
		optimizeRenderer(i, nearby);
	}
}

//nearby new Chunk*[4]
//Chunk[0] - Chunk[x-1][z]
//Chunk[1] - Chunk[x][z-1]
//Chunk[2] - Chunk[x+1][z]
//Chunk[3] - Chunk[x][z+1]
void Chunk::optimizeRenderer(uint16_t yoffset, Chunk** nearby) {
	optimized = true;
	std::vector<std::pair<UBlock*, std::vector<std::pair<Block*, bool*>>>> blocks;
	// second is number of this blocks in chunk
	for (int32_t i = 0; i < 16; i++) {
		for (int32_t j = 16 * yoffset; j < 16 * (yoffset + 1); j++) {
			for (int32_t k = 0; k < 16; k++) {
				if (ChunkBlocks[i][j][k] != nullptr) {
					// if block nearby is minecraft:air - that means that we should process this block
					// otherwise we just skip it
					bool *statements = new bool[6]{
							(((k > 0) and (ChunkBlocks[i][j][k - 1] == nullptr)) or (nearby[1] == nullptr or (nearby[1]->ChunkBlocks[i][j][15] == nullptr))),
							(((k < 15) and (ChunkBlocks[i][j][k + 1] == nullptr)) or (nearby[3] == nullptr or (nearby[3]->ChunkBlocks[i][j][0] == nullptr))),
							(((i > 0) and (ChunkBlocks[i - 1][j][k] == nullptr)) or (nearby[0] == nullptr or (nearby[0]->ChunkBlocks[15][j][k] == nullptr))),
							(((i < 15) and (ChunkBlocks[i + 1][j][k] == nullptr)) or (nearby[2] == nullptr or (nearby[2]->ChunkBlocks[0][j][k] == nullptr))),
							(((j > 0) and (ChunkBlocks[i][j - 1][k] == nullptr)) or true),
							(((j < 255) and (ChunkBlocks[i][j + 1][k] == nullptr)) or true)
					}; // tiles which we should render
					if (statements[0] or statements[1] or statements[2] or statements[3] or statements[4] or statements[5]) {
						bool flag = false;
						for (auto itr = blocks.begin(); itr != blocks.end(); itr++) {
							if ((*itr).first->integer_id == ChunkBlocks[i][j][k]->ref->integer_id) {
								(*itr).second.push_back(std::pair<Block*, bool*>(ChunkBlocks[i][j][k], statements));
								flag = true;
								break;
							}
						}
						if (not flag) {
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
		glGenVertexArrays(1, &(VAO[yoffset][m]));
		glGenBuffers(1, &(VBO[yoffset][m]));

		glBindBuffer(GL_ARRAY_BUFFER, VBO[yoffset][m]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * 9 * sizeof(float), x, GL_STATIC_DRAW);

		glBindVertexArray(VAO[yoffset][m]);
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
void Chunk::deleteOptimizedRenderer(uint16_t yoffset) {
	for (size_t i = 0; i < ovsize[yoffset]; i++) {
		glDeleteVertexArrays(1, &(VAO[yoffset][i]));
		glDeleteBuffers(1, &(VBO[yoffset][i]));
		delete[] optimizedVertices[yoffset][i];
	}
	delete[] VAO[yoffset];
	delete[] VBO[yoffset];
	delete[] optimizedVertices[yoffset];
}
void Chunk::deleteOptimizedRenderer() {
	for (uint16_t j = 0; j < 16; j++) {
		deleteOptimizedRenderer(j);
	}
	delete[] VAO;
	delete[] VBO;
	delete[] OVSizel;
	delete[] optimizedVertices;
	delete[] ovblocks;
}
void Chunk::Save() {

}
void Chunk::PlaceBlock(int32_t x, int32_t y, int32_t z, UBlock* block, Chunk** nearby) {
	x -= this->x * 16;
	z -= this->z * 16;
	if (ChunkBlocks[x][y][z] != nullptr) {
		delete ChunkBlocks[x][y][z];
	}
	ChunkBlocks[x][y][z] = new Block(nullptr, 0, block, Block::Coords(x + this->x * 16, y, z + this->z * 16));
	deleteOptimizedRenderer(y / 16);
	optimizeRenderer(y / 16, nearby);
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
	if (optimized) {
		for (size_t j = 0; j < 16; j++) {
			for (size_t i = 0; i < ovsize[j]; i++) {
				ovblocks[j][i]->BindTextures();
				glBindVertexArray(VAO[j][i]);
				glDrawArrays(GL_TRIANGLES, 0, OVSizel[j][i]);
			}
		}
	}
}
Block* Chunk::getBlock(uint8_t x, uint8_t y, uint8_t z) {
	return ChunkBlocks[x - this->x * 16][y][z - this->z * 16];
}
Chunk::~Chunk() {
	deleteOptimizedRenderer();
	if (blocks == nullptr) {
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
#endif
// 0 0 0  
// 1 1 1
// 2 2 2
//
//
//