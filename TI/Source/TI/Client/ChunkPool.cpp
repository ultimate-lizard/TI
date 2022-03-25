#include "ChunkPool.h"

#include <iostream>

#include <TI/Client/ChunkMesh.h>
#include <TI/Util/Utils.h>
#include <TI/Server/Plane.h>

ChunkPool::ChunkPool(size_t vboSize, size_t eboSize) :
	freeVertexOffsetEnd(0),
	freeElementOffsetEnd(0),
	vboSize(vboSize),
	eboSize(eboSize)
{
	MeshBuilder builder;
	poolMesh = builder.buildDyanmic(vboSize, eboSize);
}

void ChunkPool::insertChunkMesh(const Plane* plane, ChunkMesh* chunkMesh)
{
	if (chunkMesh->data.empty())
	{
		return;
	}

	size_t vertexOffset = freeVertexOffsetEnd;
	size_t elementOffset = freeElementOffsetEnd;

	bool addingNew = true;

	const size_t MIN_VERTEX_SIZE = 240;
	const size_t MIN_ELEMENT_SIZE = 72;

	size_t requiredVertexSize = chunkMesh->data.size() * sizeof(float);
	if (auto foundIter = freeVertexOffsets.lower_bound({ requiredVertexSize, 0 }); foundIter != freeVertexOffsets.end())
	{
		// std::cout << "Found free vertex memory with size " << foundIter->size << " at offset " << foundIter->offset << ".Required: " << requiredVertexSize << std::endl;

		vertexOffset = foundIter->offset;

		size_t remainingSize = foundIter->size - requiredVertexSize;
		if (remainingSize >= MIN_VERTEX_SIZE)
		{
			freeVertexOffsets.emplace(MemoryChunk{ remainingSize, vertexOffset + requiredVertexSize });
			// std::cout << "Adding remaining free chunk with size " << remainingSize << " at " << vertexOffset + requiredVertexSize << std::endl;
		}

		freeVertexOffsets.erase(foundIter);

		addingNew = false;
	}

	size_t requiredElementSize = chunkMesh->elements.size() * sizeof(unsigned int);
	if (auto foundIter = freeElementOffsets.lower_bound({ requiredElementSize, 0 }); foundIter != freeElementOffsets.end())
	{
		// std::cout << "Found free element memory with size " << foundIter->size << " at offset " << foundIter->offset << ".Required: " << requiredElementSize << std::endl;

		elementOffset = foundIter->offset;

		size_t remainingSize = foundIter->size - requiredElementSize;
		if (remainingSize >= MIN_ELEMENT_SIZE)
		{
			freeElementOffsets.emplace(MemoryChunk{ remainingSize, elementOffset + requiredElementSize });
			// std::cout << "Adding remaining free element memory with size " << remainingSize << " at " << elementOffset + requiredElementSize << std::endl;
		}

		freeElementOffsets.erase(foundIter);

		addingNew = false;
	}

	poolMesh->setBufferSubData(vertexOffset, chunkMesh->data);

	if (plane)
	{
		chunks.emplace(utils::positionToIndex(plane->positionToChunkPosition(chunkMesh->getPosition()), plane->getSize()), ChunkData{ vertexOffset, elementOffset, chunkMesh->data.size() * sizeof(float), chunkMesh->elements.size() * sizeof(unsigned int), chunkMesh });
	}
	
	// Adding new
	if (addingNew)
	{
		freeElementOffsetEnd += chunkMesh->elements.size() * sizeof(unsigned int);
		freeVertexOffsetEnd = vertexOffset + chunkMesh->data.size() * sizeof(float);
		analyticVboSize += chunkMesh->data.size() * sizeof(float);
	}
	//else
	//{
	//	std::cout << "########################VERTICES#########################" << std::endl;
	//	size_t i = 1;
	//	for (const auto& mem : freeVertexOffsets)
	//	{
	//		std::cout << i++ << ". " << mem.size << std::endl;
	//	}
	//	std::cout << "########################ELEMENTS#########################" << std::endl;
	//	i = 1;
	//	for (const auto& mem : freeElementOffsets)
	//	{
	//		std::cout << i++ << ". " << mem.size << std::endl;
	//	}
	//	std::cout << "#########################################################" << std::endl << std::endl;
	//}

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
}

void ChunkPool::freeChunkMesh(size_t index)
{
	if (auto foundIter = chunks.find(index); foundIter != chunks.end())
	{
		freeVertexOffsets.emplace(MemoryChunk{ foundIter->second.vertexSize, foundIter->second.vertexOffset });
		// std::cout << "Vertex memory with size " << foundIter->second.vertexSize << " is now available at offset " << foundIter->second.vertexOffset << std::endl;
		freeElementOffsets.emplace(MemoryChunk{ foundIter->second.elementSize, foundIter->second.elementOffset });
		// std::cout << "Element memory with size " << foundIter->second.elementSize << " is now available at offset " << foundIter->second.elementOffset << std::endl;
		chunks.erase(foundIter);
	}
}

bool ChunkPool::findChunkMesh(size_t index) const
{
	// std::cout << chunks.size() << std::endl;
	return chunks.find(index) != chunks.end();
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
	sizes.reserve(chunks.size());
	offsets.reserve(chunks.size());

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
		sizes.push_back(pair.second.mesh->elements.size());
		offsets.push_back(reinterpret_cast<void*>(pair.second.elementOffset));

		vboSize += pair.second.mesh->data.size() * sizeof(float);
	}

	poolMesh->setElementsSubData(cachedElementOffset, elements);
	cachedElementOffset += elements.size() * sizeof(unsigned int);
	analyticEboSize += elements.size() * sizeof(unsigned int);
	elements.clear();

	float eboSize = static_cast<float>(elements.size()) * sizeof(unsigned int);

	if (eboSize > this->eboSize || vboSize > this->vboSize)
	{
		throw std::exception();
	}

	return MultiDrawData { poolMesh.get(), std::move(sizes), std::move(offsets), chunks.size() - invisibleCount };
}
