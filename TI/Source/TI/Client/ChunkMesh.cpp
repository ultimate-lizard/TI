#include "ChunkMesh.h"

#include <iostream>
#include <array>

#include <glad/glad.h>

#include <TI/Server/Chunk.h>
#include <TI/Server/Plane.h>
#include <TI/Util/Utils.h>

// TODO: Change chunk pointer to reference
ChunkMesh::ChunkMesh(const Chunk* const chunk, const Plane* const plane) :
	chunkSize(chunk->getSize()), // Fix the hardcode
	chunk(chunk),
	plane(plane)
	// mesh(std::make_unique<Mesh>())
{
	// rebuild();
	buildGreedy();
}

//Mesh& ChunkMesh::getMesh()
//{
//	return *mesh.get();
//}

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
				//position.x += chunk->getPosition().x;
				//position.y += chunk->getPosition().y;
				//position.z += chunk->getPosition().z;

				for (size_t i = 0; i < 6; ++i)
				{
					BlockFace face = static_cast<BlockFace>(i);
					if (isFaceNextToAir(face, position))
					{
						setFace(face, position, { 1, 1, 1 });
					}
				}
			}
		}

		//if (!mesh->isDynamic())
		//{
		//	MeshBuilder builder;
		//	mesh = builder.buildDyanmic(0, 0);
		//}

		copyDataToMesh();
	}
}

void ChunkMesh::copyDataToMesh()
{
	//if (!mesh->isDynamic())
	//{
	//	MeshBuilder builder;
	//	mesh = builder.buildDyanmic(0, 0);
	//}

	data.clear();
	elements.clear();

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

	/*mesh->setBufferSize(static_cast<unsigned int>(data.size()) * sizeof(float));
	mesh->setElementsSize(static_cast<unsigned int>(elements.size()) * sizeof(unsigned int));

	mesh->setBufferSubData(0, data);
	mesh->setElementsSubData(0, elements);

	mesh->setPositionsCount(data.size() / 5);
	mesh->setIndicesCount(elements.size());*/

	// std::cout << mesh->getIndicesCount() << std::endl;
}

const glm::vec3& ChunkMesh::getPosition() const
{
	return chunk->getPosition();
}

const Chunk* ChunkMesh::getChunk() const
{
	return chunk;
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

void ChunkMesh::buildGreedy()
{
	faceVerticesMap.clear();

	for (unsigned short axis = 0; axis < 3; ++axis)
	{
		BlockFace face = BlockFace::Front;
		switch (axis)
		{
		case 0:
			face = BlockFace::Right;
			break;
		case 1:
			face = BlockFace::Top;
			break;
		case 2:
			face = BlockFace::Front;
			break;
		}

		for (unsigned short faceOffset = 0; faceOffset < 2; ++faceOffset)
		{
			face = static_cast<BlockFace>(static_cast<int>(face) + faceOffset);

			for (unsigned int slice = 0; slice < chunkSize; ++slice)
			{
				std::vector<std::vector<unsigned int>> sliceBlocks;
				sliceBlocks.resize(chunkSize);
				for (auto& iter : sliceBlocks)
				{
					iter.resize(chunkSize);
				}

				// Fill slice with blocks that are relevant
				for (unsigned int x = 0; x < chunkSize; ++x)
				{
					for (unsigned int z = 0; z < chunkSize; ++z)
					{
						glm::uvec3 blockPosition;

						switch (axis)
						{
						case 0:
							blockPosition = { slice, x, z };
							break;
						case 1:
							blockPosition = { x, slice, z };
							break;
						case 2:
							blockPosition = { x, z, slice };
							break;
						}

						if (isFaceNextToAir(face, blockPosition))
						{
							sliceBlocks[x][z] = chunk->getBlock(blockPosition);
						}
					}
				}

				// std::pair<x/y, w/h>
				std::vector<std::pair<glm::uvec2, glm::uvec2>> quads;

				// while !done
				// If all blocks aren't relevant, skip the slice
				size_t shouldSkip = 0;
				for (size_t i = 0; i < chunkSize; ++i)
				{
					if (std::find(sliceBlocks[i].begin(), sliceBlocks[i].end(), 1) == sliceBlocks[i].end())
					{
						++shouldSkip;
					}
				}
				if (shouldSkip == chunkSize)
				{
					continue;
				}

				bool quadInProgress = false;
				size_t currentQuadIndex = 0;
				for (int y = 0; y < chunkSize; ++y)
				{
					for (int x = 0; x < chunkSize; ++x)
					{
						if (!quadInProgress)
						{
							if (sliceBlocks[x][y] == 1)
							{
								quads.resize(currentQuadIndex + 1);
								quads[currentQuadIndex] = { {x, y}, glm::uvec3(0) };
								quadInProgress = true;
							}
						}
						else
						{
							auto& currentQuad = quads[currentQuadIndex];

							if (sliceBlocks[x][y] == 1)
							{
								if (currentQuad.second.x != 0 && (x < currentQuad.first.x || x > currentQuad.first.x + currentQuad.second.x - 1))
								{
									// Fail and save previous row
									currentQuad.second.y = y - currentQuad.first.y;
									quadInProgress = false;
									++currentQuadIndex;

									for (int xx = currentQuad.first.x; xx < currentQuad.first.x + currentQuad.second.x; ++xx)
									{
										for (int yy = currentQuad.first.y; yy < currentQuad.first.y + currentQuad.second.y; ++yy)
										{
											sliceBlocks[xx][yy] = 0;
										}
									}
									x = -1;
									y = 0;
								}
							}
							else
							{
								if (currentQuad.second.x == 0 || x >= currentQuad.first.x + currentQuad.second.x)
								{
									// Move to the new row and save quad width
									if (y < chunkSize - 1)
									{
										currentQuad.second.x = x - currentQuad.first.x;
										++y;
										x = -1;
									}
									else
									{
										// Save the quad
										currentQuad.second.x = x - currentQuad.first.x;
										currentQuad.second.y = y - currentQuad.first.y + 1;
										quadInProgress = false;
										++currentQuadIndex;

										for (int xx = currentQuad.first.x; xx < currentQuad.first.x + currentQuad.second.x; ++xx)
										{
											for (int yy = currentQuad.first.y; yy < currentQuad.first.y + currentQuad.second.y; ++yy)
											{
												sliceBlocks[xx][yy] = 0;
											}
										}
										x = -1;
										y = 0;
									}
								}
								else if (x >= currentQuad.first.x && x < currentQuad.first.x + currentQuad.second.x)
								{
									// Fail and save previous row
									currentQuad.second.y = y - currentQuad.first.y;
									quadInProgress = false;
									++currentQuadIndex;

									for (int xx = currentQuad.first.x; xx < currentQuad.first.x + currentQuad.second.x; ++xx)
									{
										for (int yy = currentQuad.first.y; yy < currentQuad.first.y + currentQuad.second.y; ++yy)
										{
											sliceBlocks[xx][yy] = 0;
										}
									}
									x = -1;
									y = 0;
								}
							}
						}

						if (x < 0 || y < 0) continue;

						if (quadInProgress && sliceBlocks[x][y] == 1)
						{
							// Row/column end conditions
							if (x == chunkSize - 1 && y < chunkSize - 1)
							{
								if (quads[currentQuadIndex].second.x == 0)
								{
									quads[currentQuadIndex].second.x = x - quads[currentQuadIndex].first.x + 1;
								}
							}
							else if (x == chunkSize - 1 && y == chunkSize - 1)
							{
								// Save current quad
								if (quads[currentQuadIndex].second.x == 0)
								{
									quads[currentQuadIndex].second.x = x - quads[currentQuadIndex].first.x + 1;
								}

								quads[currentQuadIndex].second.y = y - quads[currentQuadIndex].first.y + 1;
								quadInProgress = false;

								for (int xx = quads[currentQuadIndex].first.x; xx < quads[currentQuadIndex].first.x + quads[currentQuadIndex].second.x; ++xx)
								{
									for (int yy = quads[currentQuadIndex].first.y; yy < quads[currentQuadIndex].first.y + quads[currentQuadIndex].second.y; ++yy)
									{
										sliceBlocks[xx][yy] = 0;
									}
								}
								x = -1;
								y = 0;

								++currentQuadIndex;
							}
						}
					}
				}

				// glm::uvec3 position = utils::indexToPosition(i, glm::uvec3(static_cast<unsigned int>(chunkSize)));
				for (std::pair<glm::uvec2, glm::uvec2> quad : quads)
				{
					glm::uvec3 meshPosition;
					glm::uvec3 meshSize;

					switch (axis)
					{
					case 0:
						meshPosition = { slice, quad.first.x, quad.first.y };
						meshSize = { 1.0f, quad.second.x, quad.second.y };
						break;
					case 1:
						meshPosition = { quad.first.x, slice, quad.first.y };
						meshSize = { quad.second.x, 1.0f, quad.second.y };
						break;
					case 2:
						meshPosition = { quad.first.x, quad.first.y, slice };
						meshSize = { quad.second.x, quad.second.y, 1.0f };
						break;
					}

					setFace(face, meshPosition, meshSize);
				}
			}
		}
	}

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

void ChunkMesh::setFace(BlockFace face, const glm::uvec3& inPosition, const glm::uvec3& size)
{
	glm::uvec3 position = inPosition;
	position.x += chunk->getPosition().x;
	position.y += chunk->getPosition().y;
	position.z += chunk->getPosition().z;

	const std::array<float, 20> FRONT_DATA{
		1.0f * size.x + position.x, 1.0f * size.y + position.y, 1.0f * size.z + position.z, 1.0f, 1.0f,
		0.0f + position.x, 1.0f * size.y + position.y, 1.0f * size.z + position.z, 0.0f, 1.0f,
		0.0f + position.x, 0.0f + position.y, 1.0f * size.z + position.z, 0.0f, 0.0f,
		1.0f * size.x + position.x, 0.0f + position.y, 1.0f * size.z + position.z, 1.0f, 0.0f,
	};

	const std::array<float, 20> BACK_DATA = {
		0.0f + position.x, 1.0f * size.y + position.y, 0.0f + position.z, 0.0f, 1.0f,
		1.0f * size.x + position.x, 1.0f * size.y + position.y, 0.0f + position.z, 1.0f, 1.0f,
		1.0f * size.x + position.x, 0.0f + position.y, 0.0f + position.z, 1.0f, 0.0f,
		0.0f + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,
	};

	const std::array<float, 20> LEFT_DATA = {
		0.0f + position.x, 1.0f * size.y + position.y, 1.0f * size.z + position.z, 1.0f, 1.0f,
		0.0f + position.x, 1.0f * size.y + position.y, 0.0f + position.z, 0.0f, 1.0f,
		0.0f + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,
		0.0f + position.x, 0.0f + position.y, 1.0f * size.z + position.z, 1.0f, 0.0f,
	};

	const std::array<float, 20> RIGHT_DATA = {
		1.0f * size.x + position.x, 1.0f * size.y + position.y, 0.0f + position.z, 0.0f, 1.0f,
		1.0f * size.x + position.x, 1.0f * size.y + position.y, 1.0f * size.z + position.z, 1.0f, 1.0f,
		1.0f * size.x + position.x, 0.0f + position.y, 1.0f * size.z + position.z, 1.0f, 0.0f,
		1.0f * size.x + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,
	};

	const std::array<float, 20> TOP_DATA = {
		1.0f * size.x + position.x, 1.0f * size.y + position.y, 1.0f * size.z + position.z, 1.0f, 1.0f,
		1.0f * size.x + position.x, 1.0f * size.y + position.y, 0.0f + position.z, 0.0f, 1.0f,
		0.0f + position.x, 1.0f * size.y + position.y, 0.0f + position.z, 0.0f, 0.0f,
		0.0f + position.x, 1.0f * size.y + position.y, 1.0f * size.z + position.z, 1.0f, 0.0f,
	};

	const std::array<float, 20> BOTTOM_DATA = {
		1.0f * size.x + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 1.0f,
		1.0f * size.x + position.x, 0.0f + position.y, 1.0f * size.z + position.z, 1.0f, 1.0f,
		0.0f + position.x, 0.0f + position.y, 1.0f * size.z + position.z, 1.0f, 0.0f,
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
