#pragma once

#include <vector>
#include <map>

#include <glm/glm.hpp>

#include <TI/Renderer/Mesh.h>
#include <TI/Renderer/Material.h>

class Chunk;

class ChunkMesh
{
public:
	ChunkMesh(const Chunk* const chunk);

	ChunkMesh(const ChunkMesh&) = delete;
	ChunkMesh(ChunkMesh&&) = delete;

	void setBlock(const glm::ivec3 position);

private:
	unsigned int chunkSize;

	Mesh mesh;
	Material material;
};
