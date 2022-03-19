#pragma once

#include <memory>
#include <map>
#include <vector>

class Mesh;
class ChunkMesh;
class Plane;

struct MultiDrawData
{
	Mesh* poolMesh;
	std::vector<int> sizes;
	std::vector<void*> offsets;
	size_t drawCount;
};

class ChunkPool
{
	struct ChunkData
	{
		size_t elementOffset;
		size_t elementSize;
		ChunkMesh* mesh;
		bool visible = true;
	};

public:

	ChunkPool(size_t vboSize, size_t eboSize);

	void insertChunkMesh(const Plane* plane, ChunkMesh* chunkMesh);
	void freeChunkMesh(size_t index);
	void setChunkMeshVisibility(size_t index, bool visible);

	MultiDrawData buildMesh();

private:
	std::unique_ptr<Mesh> poolMesh;
	
	std::map<size_t, ChunkData> chunks;
	unsigned int lastElement = 0;
	std::vector<unsigned int> elements;
	size_t cachedElementOffset = 0;

	size_t freeVertexOffset;
	size_t freeElementOffset;
	size_t vboSize;
	size_t eboSize;

	std::vector<size_t> invisibleChunkMeshes;
};
