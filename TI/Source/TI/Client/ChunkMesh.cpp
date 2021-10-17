#include "ChunkMesh.h"

#include <iostream>
#include <array>

#include <glad/glad.h>

#include <TI/Server/Chunk.h>
#include <TI/Server/Plane.h>

// TODO: Change chunk pointer to reference
ChunkMesh::ChunkMesh(const Chunk* const chunk, const Plane* plane) :
	chunkSize(0),
	chunk(chunk),
	elementsCount(0),
	plane(plane),
	mesh(std::make_unique<Mesh>())
{
	rebuildMesh();
}

Mesh& ChunkMesh::getMesh()
{
	return *mesh.get();
}

void ChunkMesh::rebuildMesh()
{
	faceVerticesMap.clear();
	elementsCount = 0;

	if (chunk)
	{
		chunkSize = chunk->getSize();

		const std::vector<unsigned int>& blocks = chunk->getBlocks();

		for (unsigned int i = 0; i < blocks.size(); ++i)
		{
			int index = i;
			int z = index / (chunkSize * chunkSize);
			index -= (z * chunkSize * chunkSize);
			int y = index / chunkSize;
			int x = index % chunkSize;

			if (blocks[i] != 0) // if block is not air
			{
				glm::ivec3 position = { x, y, z };

				for (int i = 0; i < 6; ++i)
				{
					Face face = static_cast<Face>(i);
					if (isFaceNextToAir(face, position))
					{
						setFace(face, position);
						++elementsCount;
					}
				}
			}
		}

		if (!mesh->isDynamic())
		{
			MeshBuilder builder;
			size_t s = (static_cast<size_t>(chunkSize) * chunkSize * chunkSize) * 5 * sizeof(float) * 36;
			size_t e = (static_cast<size_t>(chunkSize) * chunkSize * chunkSize) * 3 * sizeof(unsigned int) * 36;
			mesh = builder.buildDyanmic(s, e);
		}

		std::vector<float> data;
		std::vector<unsigned int> elements;
		for (auto& faceDataMapPair : faceVerticesMap)
		{
			for (size_t i = 0; i < faceDataMapPair.second.size(); ++i)
			{
				data.push_back(faceDataMapPair.second[i]);
			}

			// Calculate and push indices
			std::array<unsigned int, 6> faceIndices = {
				0, 1, 2,
				0, 2, 3,
			};
			for (unsigned int& index : faceIndices)
			{
				if (elements.size())
				{
					index += elements.size() / 6 * 4;
				}
			}

			elements.insert(elements.end(), faceIndices.begin(), faceIndices.end());
		}

		mesh->setBufferSubData(0, data);
		mesh->setElementsSubData(0, elements);

		mesh->setPositionsCount(data.size() / 5);
		mesh->setIndicesCount(elements.size());
	}
}

const glm::vec3& ChunkMesh::getPosition() const
{
	return chunk->getPosition();
}

void ChunkMesh::updateBlock(const glm::uvec3& updatedBlockPosition)
{
	// TODO: getBlockNextToFace
	size_t index = (static_cast<size_t>(updatedBlockPosition.z) * chunkSize * chunkSize) + (static_cast<size_t>(updatedBlockPosition.y) * chunkSize) + updatedBlockPosition.x;

	for (unsigned int i = 0; i < 6; ++i)
	{
		Face face = static_cast<Face>(i);
		faceVerticesMap.erase({ index, face });
	}

	std::vector<glm::uvec3> checkedPositions;
	if (updatedBlockPosition.x - 1 >= 0)
	{
		checkedPositions.push_back({ updatedBlockPosition.x - 1, updatedBlockPosition.y, updatedBlockPosition.z });
	}
	if (updatedBlockPosition.x + 1 < chunkSize)
	{
		checkedPositions.push_back({ updatedBlockPosition.x + 1, updatedBlockPosition.y, updatedBlockPosition.z });
	}
	if (updatedBlockPosition.y - 1 >= 0)
	{
		checkedPositions.push_back({ updatedBlockPosition.x, updatedBlockPosition.y - 1, updatedBlockPosition.z });
	}
	if (updatedBlockPosition.y + 1 < chunkSize)
	{
		checkedPositions.push_back({ updatedBlockPosition.x, updatedBlockPosition.y + 1, updatedBlockPosition.z });
	}
	if (updatedBlockPosition.z - 1 >= 0)
	{
		checkedPositions.push_back({ updatedBlockPosition.x, updatedBlockPosition.y, updatedBlockPosition.z - 1});
	}
	if (updatedBlockPosition.z + 1 < chunkSize)
	{
		checkedPositions.push_back({ updatedBlockPosition.x, updatedBlockPosition.y, updatedBlockPosition.z + 1});
	}

	for (const glm::uvec3& checkedBlockPosition : checkedPositions)
	{
		if (chunk->getBlock(checkedBlockPosition) != 0)
		{
			size_t idx = (static_cast<size_t>(checkedBlockPosition.z) * chunkSize * chunkSize) + (static_cast<size_t>(checkedBlockPosition.y) * chunkSize) + checkedBlockPosition.x;
			for (int i = 0; i < 6; ++i)
			{
				Face face = static_cast<Face>(i);
				if (isFaceNextToAir(face, checkedBlockPosition))
				{
					setFace(face, checkedBlockPosition);
				}
				else
				{
					faceVerticesMap.erase({ idx, face });
				}
			}
		}
	}

	// Send data to OpenGL
	std::vector<float> data;
	std::vector<unsigned int> elements;

	for (auto& faceDataMapPair : faceVerticesMap)
	{
		for (size_t i = 0; i < faceDataMapPair.second.size(); ++i)
		{
			data.push_back(faceDataMapPair.second[i]);
		}

		// Calculate and push indices
		std::array<unsigned int, 6> faceIndices = {
			0, 1, 2,
			0, 2, 3,
		};
		for (unsigned int& index : faceIndices)
		{
			if (elements.size())
			{
				index += elements.size() / 6 * 4;
			}
		}

		elements.insert(elements.end(), faceIndices.begin(), faceIndices.end());
	}

	mesh->setBufferSubData(0, data);
	mesh->setElementsSubData(0, elements);

	mesh->setPositionsCount(data.size() / 5);
	mesh->setIndicesCount(elements.size());
}

bool ChunkMesh::isFaceNextToAir(Face face, const glm::ivec3& blockPosition)
{
	if (!chunk || !plane)
	{
		return true;
	}

	const std::vector<unsigned int>& blocks = chunk->getBlocks();

	glm::ivec3 position = blockPosition;

	position.x += chunk->getPosition().x;
	position.y += chunk->getPosition().y;
	position.z += chunk->getPosition().z;

	switch (face)
	{
		case Face::Front: position.z++; break;
		case Face::Back: position.z--; break;
		case Face::Left: position.x--; break;
		case Face::Right: position.x++; break;
		case Face::Top: position.y++; break;
		case Face::Bottom: position.y--; break;
	}

	if (plane->getBlock(position) != 0)
	{
		return false;
	}

	return true;
}

void ChunkMesh::setFace(Face face, glm::ivec3 position)
{
	const std::array<float, 20> FRONT_DATA {
		1.0f + position.x, 1.0f + position.y, 1.0f + position.z, 1.0f, 1.0f,
		0.0f + position.x, 1.0f + position.y, 1.0f + position.z, 0.0f, 1.0f,
		0.0f + position.x, 0.0f + position.y, 1.0f + position.z, 0.0f, 0.0f,
		1.0f + position.x, 0.0f + position.y, 1.0f + position.z, 1.0f, 0.0f,
	};

	const std::array<float, 20> BACK_DATA = {
		0.0f + position.x, 1.0f + position.y, 0.0f + position.z, 0.0f, 1.0f,
		1.0f + position.x, 1.0f + position.y, 0.0f + position.z, 1.0f, 1.0f,
		1.0f + position.x, 0.0f + position.y, 0.0f + position.z, 1.0f, 0.0f,
		0.0f + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,
	};

	const std::array<float, 20> LEFT_DATA = {
		0.0f + position.x, 1.0f + position.y, 1.0f + position.z, 1.0f, 1.0f,
		0.0f + position.x, 1.0f + position.y, 0.0f + position.z, 0.0f, 1.0f,
		0.0f + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,
		0.0f + position.x, 0.0f + position.y, 1.0f + position.z, 1.0f, 0.0f,
	};

	const std::array<float, 20> RIGHT_DATA = {
		1.0f + position.x, 1.0f + position.y, 0.0f + position.z, 0.0f, 1.0f,
		1.0f + position.x, 1.0f + position.y, 1.0f + position.z, 1.0f, 1.0f,
		1.0f + position.x, 0.0f + position.y, 1.0f + position.z, 1.0f, 0.0f,
		1.0f + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,
	};

	const std::array<float, 20> TOP_DATA = {
		1.0f + position.x, 1.0f + position.y, 1.0f + position.z, 1.0f, 1.0f,
		1.0f + position.x, 1.0f + position.y, 0.0f + position.z, 0.0f, 1.0f,
		0.0f + position.x, 1.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,
		0.0f + position.x, 1.0f + position.y, 1.0f + position.z, 1.0f, 0.0f,
	};

	const std::array<float, 20> BOTTOM_DATA = {
		1.0f + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 1.0f,
		1.0f + position.x, 0.0f + position.y, 1.0f + position.z, 1.0f, 1.0f,
		0.0f + position.x, 0.0f + position.y, 1.0f + position.z, 1.0f, 0.0f,
		0.0f + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,
	};

	std::array<unsigned int, 6> faceIndices = {
		0, 1, 2,
		0, 2, 3,
	};

	for (unsigned int& index : faceIndices)
	{
		if (this->faceVerticesMap.size())
		{
			index += this->faceVerticesMap.size() * 4;
		}
	}

	const std::array<float, 20>* faceData = nullptr;

	switch (face)
	{
	case ChunkMesh::Face::Front:
		faceData = &FRONT_DATA;
		break;
	case ChunkMesh::Face::Back:
		faceData = &BACK_DATA;
		break;
	case ChunkMesh::Face::Left:
		faceData = &LEFT_DATA;
		break;
	case ChunkMesh::Face::Right:
		faceData = &RIGHT_DATA;
		break;
	case ChunkMesh::Face::Top:
		faceData = &TOP_DATA;
		break;
	case ChunkMesh::Face::Bottom:
		faceData = &BOTTOM_DATA;
		break;
	default:
		throw std::exception();
		break;
	}

	glm::ivec3 iPosition = position;
	size_t index = (static_cast<size_t>(iPosition.z) * chunkSize * chunkSize) + (static_cast<size_t>(iPosition.y) * chunkSize) + iPosition.x;

	this->faceVerticesMap[{index, face}] = *faceData;
}
