#pragma once

#include <vector>
#include <map>
#include <array>

#include <glm/glm.hpp>

#include <TI/Renderer/Mesh.h>
#include <TI/Renderer/Material.h>

class Chunk;
class Plane;

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
	ChunkMesh(const Chunk* const chunk, const Plane* plane);

	ChunkMesh(const ChunkMesh&) = delete;
	ChunkMesh(ChunkMesh&&) = delete;

	Mesh& getMesh();

	// TODO: Refactor this. This is a very slow function and can't be called when a player places blocks
	void rebuildMesh();

	const glm::vec3& getPosition() const;

	void updateBlock(const glm::uvec3& position);

private:
	bool isFaceNextToAir(Face face, const glm::ivec3& blockPosition);

	void setFace(Face face, glm::ivec3 position);

private:
	unsigned int chunkSize;

	std::unique_ptr<Mesh> mesh;

	//std::vector<float> data;
	//std::vector<unsigned int> elements;
	std::map<std::pair<size_t, Face>, std::array<float, 20>> faceVerticesMap;

	size_t elementsCount;

	const Chunk* const chunk;
	const Plane* plane;
};
