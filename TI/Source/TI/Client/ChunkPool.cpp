#include "ChunkPool.h"

#include <iostream>

#include <TI/Client/ChunkMesh.h>
#include <TI/Util/Utils.h>
#include <TI/Server/Plane.h>

static float analyticVboSize = 0;
static float analyticEboSize = 0;

ChunkPool::ChunkPool(size_t vboSize, size_t eboSize) :
	freeVertexOffset(0),
	freeElementOffset(0),
	vboSize(vboSize),
	eboSize(eboSize)
{
	MeshBuilder builder;
	poolMesh = builder.buildDyanmic(vboSize, eboSize);
}

void ChunkPool::insertChunkMesh(const Plane* plane, ChunkMesh* chunkMesh)
{
	size_t vertexOffset = freeVertexOffset;
	poolMesh->setBufferSubData(vertexOffset, chunkMesh->data);

	if (plane)
	{
		chunks.emplace(utils::positionToIndex(plane->positionToChunkPosition(chunkMesh->getPosition()), plane->getSize()), ChunkData{ freeElementOffset, chunkMesh->elements.size(), chunkMesh });
	}
	
	freeElementOffset += chunkMesh->elements.size() * sizeof(unsigned int);
	freeVertexOffset = vertexOffset + chunkMesh->data.size() * sizeof(float);

	for (size_t i = 0; i < chunkMesh->elements.size() / 6; ++i)
	{
		elements.push_back(0 + lastElement);
		elements.push_back(1 + lastElement);
		elements.push_back(2 + lastElement);
		elements.push_back(0 + lastElement);
		elements.push_back(2 + lastElement);
		elements.push_back(3 + lastElement);

		lastElement = 3 + lastElement + 1;
	}

	analyticVboSize += chunkMesh->data.size() * sizeof(float);
}

void ChunkPool::freeChunkMesh(size_t index)
{
	if (auto foundIter = chunks.find(index); foundIter != chunks.end())
	{
		chunks.erase(foundIter);
		// clear vbo
	}
}

void ChunkPool::setChunkMeshVisibility(size_t index, bool visible)
{
	if (auto foundPair = chunks.find(index); foundPair != chunks.end())
	{
		foundPair->second.visible = visible;
	}
}

MultiDrawData ChunkPool::buildMesh()
{
	std::vector<int> sizes;
	std::vector<void*> offsets;

	size_t invisibleCount = 0;

	for (auto& pair : chunks)
	{
		if (!pair.second.visible)
		{
			++invisibleCount;
			continue;
		}

		const short ATTR_COUNT = 5;
		const short VERTICES_PER_FACE = 4;
		const short ELEMENTS_PER_FACE = 6;
		sizes.push_back(pair.second.elementSize);
		offsets.push_back(reinterpret_cast<void*>(pair.second.elementOffset));

		// Analytics:
		vboSize += pair.second.mesh->data.size() * sizeof(float);
	}

	poolMesh->setElementsSubData(cachedElementOffset, elements);
	cachedElementOffset += elements.size() * sizeof(unsigned int);
	analyticEboSize += elements.size() * sizeof(unsigned int);
	elements.clear();

	float eboSize = static_cast<float>(elements.size()) * sizeof(unsigned int);

	std::cout << "EBO size: " << analyticEboSize / 1024 / 1024 << " MB" << std::endl;
	std::cout << "VBO size: " << analyticVboSize / 1024 / 1024 << " MB" << std::endl;

	if (eboSize > this->eboSize || vboSize > this->vboSize)
	{
		throw std::exception();
	}

	return MultiDrawData { poolMesh.get(), std::move(sizes), std::move(offsets), chunks.size() - invisibleCount };
}
