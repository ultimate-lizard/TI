#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include <set>

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
		size_t vertexOffset;
		size_t elementOffset;
		size_t vertexSize;
		size_t elementSize;
		ChunkMesh* mesh;
		bool visible = true;
	};

	struct MemoryChunk
	{
		size_t size;
		size_t offset;

		inline bool operator>(const MemoryChunk& other) const { return size > other.size; }
		inline bool operator<(const MemoryChunk& other) const { return size < other.size; }
		inline bool operator==(const MemoryChunk& other) const { return size == other.size; }
	};

public:

	ChunkPool(size_t vboSize, size_t eboSize);

	void insertChunkMesh(const Plane* plane, ChunkMesh* chunkMesh);
	void freeChunkMesh(size_t index);
	bool findChunkMesh(size_t index) const;

	void setChunkMeshVisibility(size_t index, bool visible);

	MultiDrawData buildMesh();

	float analyticVboSize = 0.0f;
	float analyticEboSize = 0.0f;

private:
	std::unique_ptr<Mesh> poolMesh;
	
	std::unordered_map<size_t, ChunkData> chunks;
	unsigned int lastElement = 0;
	std::vector<unsigned int> elements;
	size_t cachedElementOffset = 0;

	size_t freeVertexOffsetEnd;
	size_t freeElementOffsetEnd;
	size_t vboSize;
	size_t eboSize;

	std::multiset<MemoryChunk> freeVertexOffsets;
	std::multiset<MemoryChunk> freeElementOffsets;
};
