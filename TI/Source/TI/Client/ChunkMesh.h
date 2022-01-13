#pragma once

#include <vector>
#include <map>
#include <array>

#include <glm/glm.hpp>

#include <TI/Renderer/Mesh.h>
#include <TI/Renderer/Material.h>
#include <TI/Server/Block.h>

class Chunk;
class Plane;

class ChunkMesh
{

public:
	ChunkMesh(const Chunk* const chunk, const Plane* const plane);

	ChunkMesh(const ChunkMesh&) = delete;
	ChunkMesh(ChunkMesh&&) = delete;

	// Mesh& getMesh();

	void rebuild();
	void updateBlock(const glm::uvec3& position);

	void buildGreedy();

	const glm::vec3& getPosition() const;

	std::vector<float> data;
	std::vector<unsigned int> elements;

private:
	bool isFaceNextToAir(BlockFace face, const glm::uvec3& blockPosition);
	void setFace(BlockFace face, const glm::uvec3& position);
	void setFace(BlockFace face, const glm::uvec3& position, const glm::uvec3& size);
	void copyDataToMesh();

private:
	size_t chunkSize;

	// std::unique_ptr<Mesh> mesh;

	std::map<std::pair<size_t, BlockFace>, std::array<float, 20>> faceVerticesMap;

	const Chunk* const chunk;
	const Plane* const plane;
};
