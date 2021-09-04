#pragma once

#include <vector>
#include <map>

#include <glm/glm.hpp>

class Chunk;
class Mesh;
class Material;

class ChunkMesh
{
public:
	ChunkMesh(const Chunk* const chunk);

	ChunkMesh(const ChunkMesh&) = delete;
	ChunkMesh(ChunkMesh&&) = delete;

	~ChunkMesh();

	void setBlock(const glm::ivec3 position);

	Mesh* getMesh();
	Material* getMaterial();

private:
	unsigned int chunkSize;

	Mesh* mesh;
	Material* material;

	/*std::vector<glm::vec3> positions;
	std::vector<glm::vec2> uvs;*/

	std::map<unsigned int, std::vector<float>> blocks;
};
