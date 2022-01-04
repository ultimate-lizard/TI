#include "ChunkMesh.h"

#include <iostream>
#include <array>

#include <glad/glad.h>

#include <TI/Server/Chunk.h>
#include <TI/Server/Plane.h>
#include <TI/Util/Utils.h>

// TODO: Change chunk pointer to reference
ChunkMesh::ChunkMesh(const Chunk* const chunk, const Plane* const plane) :
	chunkSize(0),
	chunk(chunk),
	plane(plane),
	mesh(std::make_unique<Mesh>())
{
	rebuild();
}

Mesh& ChunkMesh::getMesh()
{
	return *mesh.get();
}

void ChunkMesh::rebuild()
{
	faceVerticesMap.clear();

	if (chunk)
	{
		chunkSize = chunk->getSize();

		const std::vector<unsigned int>& blocks = chunk->getBlocks();
		for (size_t i = 0; i < blocks.size(); ++i)
		{
			if (blocks[i] != 0) // if block is not air
			{
				glm::uvec3 position = utils::indexToPosition(i, glm::uvec3(static_cast<unsigned int>(chunkSize)));

				for (size_t i = 0; i < 6; ++i)
				{
					BlockFace face = static_cast<BlockFace>(i);
					if (isFaceNextToAir(face, position))
					{
						setFace(face, position);
					}
				}
			}
		}

		if (!mesh->isDynamic())
		{
			MeshBuilder builder;
			mesh = builder.buildDyanmic(0, 0);
		}

		copyDataToMesh();
	}
}

void ChunkMesh::copyDataToMesh()
{
	std::vector<float> data;
	std::vector<unsigned int> elements;

	for (auto& faceDataMapPair : faceVerticesMap)
	{
		for (size_t i = 0; i < faceDataMapPair.second.size(); ++i)
		{
			data.push_back(faceDataMapPair.second[i]);
		}

		std::array<unsigned int, 6> faceIndices = {
			0, 1, 2,
			0, 2, 3,
		};
		for (unsigned int& index : faceIndices)
		{
			if (elements.size())
			{
				index += static_cast<unsigned int>(elements.size()) / 6 * 4;
			}
		}

		elements.insert(elements.end(), faceIndices.begin(), faceIndices.end());
	}

	mesh->setBufferSize(static_cast<unsigned int>(data.size()) * sizeof(float));
	mesh->setElementsSize(static_cast<unsigned int>(elements.size()) * sizeof(unsigned int));

	mesh->setBufferSubData(0, data);
	mesh->setElementsSubData(0, elements);

	mesh->setPositionsCount(data.size() / 5);
	mesh->setIndicesCount(elements.size());
}

const glm::vec3& ChunkMesh::getPosition() const
{
	return chunk->getPosition();
}

void ChunkMesh::updateBlock(const glm::uvec3& position)
{
	size_t index = utils::positionToIndex(position, { chunkSize, chunkSize, chunkSize });

	for (unsigned int i = 0; i < 6; ++i)
	{
		BlockFace face = static_cast<BlockFace>(i);
		faceVerticesMap.erase({ index, face });
	}

	std::vector<glm::uvec3> checkedPositions;
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			for (int z = -1; z <= 1; ++z)
			{
				if (x && y && z)
				{
					continue;
				}

				glm::uvec3 checkedPosition(position.x + x, position.y + y, position.z + z);
				// Make sure we're in the chunk's bounds
				if (checkedPosition.x >= 0 && checkedPosition.y >= 0 && checkedPosition.z >= 0
					&& checkedPosition.x < chunk->getSize() && checkedPosition.y < chunk->getSize() && checkedPosition.z < chunk->getSize())
				{
					checkedPositions.push_back(std::move(checkedPosition));
				}
			}
		}
	}

	for (const glm::uvec3& checkedBlockPosition : checkedPositions)
	{
		if (chunk->getBlock(checkedBlockPosition) != 0)
		{
			size_t checkedIndex = utils::positionToIndex(checkedBlockPosition, { chunkSize, chunkSize, chunkSize });
			for (int i = 0; i < 6; ++i)
			{
				BlockFace face = static_cast<BlockFace>(i);
				if (isFaceNextToAir(face, checkedBlockPosition))
				{
					// check if in chunks bounds
					setFace(face, checkedBlockPosition);
				}
				else
				{
					faceVerticesMap.erase({ checkedIndex, face });
				}
			}
		}
	}

	// Send data to OpenGL
	copyDataToMesh();
}

bool ChunkMesh::isFaceNextToAir(BlockFace face, const glm::uvec3& position)
{
	glm::uvec3 absolutePosition(
		position.x + static_cast<unsigned int>(chunk->getPosition().x),
		position.y + static_cast<unsigned int>(chunk->getPosition().y),
		position.z + static_cast<unsigned int>(chunk->getPosition().z)
	);

	return plane->getNeighborBlock(absolutePosition, face) == 0 && plane->isPositionInPlaneBounds(absolutePosition);
}

void ChunkMesh::setFace(BlockFace face, const glm::uvec3& position)
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
			index += static_cast<unsigned int>(this->faceVerticesMap.size()) * 4;
		}
	}

	const std::array<float, 20>* faceData = nullptr;

	switch (face)
	{
	case BlockFace::Front:
		faceData = &FRONT_DATA;
		break;
	case BlockFace::Back:
		faceData = &BACK_DATA;
		break;
	case BlockFace::Left:
		faceData = &LEFT_DATA;
		break;
	case BlockFace::Right:
		faceData = &RIGHT_DATA;
		break;
	case BlockFace::Top:
		faceData = &TOP_DATA;
		break;
	case BlockFace::Bottom:
		faceData = &BOTTOM_DATA;
		break;
	default:
		throw std::exception();
		break;
	}

	size_t index = utils::positionToIndex(position, { chunkSize, chunkSize, chunkSize });

	this->faceVerticesMap[{index, face}] = *faceData;
}
