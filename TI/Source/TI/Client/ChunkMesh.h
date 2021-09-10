#pragma once

#include <vector>
#include <map>

#include <glm/glm.hpp>

#include <TI/Renderer/Mesh.h>
#include <TI/Renderer/Material.h>

class Chunk;

class ChunkMesh
{
	enum class Face
	{
		Front,
		Back,
		Left,
		Right,
		Top,
		Bottom
	};

public:
	ChunkMesh(const Chunk* const chunk);

	ChunkMesh(const ChunkMesh&) = delete;
	ChunkMesh(ChunkMesh&&) = delete;

	Mesh& getMesh();
	Material& getMaterial();

	void rebuildMesh();

	const glm::vec3& getPosition() const;

private:
	bool isFaceNextToAir(Face face, const glm::ivec3& blockPosition);

	void setFace(Face face, glm::ivec3 position);

private:
	unsigned long long chunkSize;

	Mesh mesh;
	Material material;

	std::vector<float> data;
	std::vector<unsigned int> elements;
	size_t elementsCount;

	const Chunk* const chunk;
};
