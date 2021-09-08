#include "ChunkMesh.h"

#include <TI/Server/Chunk.h>

#include <glad/glad.h>

#include <iostream>

// TODO: Change chunk pointer to reference
ChunkMesh::ChunkMesh(const Chunk* const chunk) :
	chunkSize(0),
	mesh(),
	chunk(chunk),
	elementsCount(0)
{
	material.setShader("../Shaders/SampleShader.vert", "../Shaders/SampleShader.frag");
	material.setTexture("../Textures/dirt.jpg");

	rebuildMesh();
}

void ChunkMesh::setBlock(const glm::ivec3 position)
{
	std::vector<float> cubeData = {
		1.0f + position.x, 1.0f + position.y, 1.0f + position.z, 1.0f, 1.0f,
		0.0f + position.x, 1.0f + position.y, 1.0f + position.z, 0.0f, 1.0f,
		0.0f + position.x, 0.0f + position.y, 1.0f + position.z, 0.0f, 0.0f,
		1.0f + position.x, 0.0f + position.y, 1.0f + position.z, 1.0f, 0.0f,

		0.0f + position.x, 1.0f + position.y, 0.0f + position.z, 0.0f, 1.0f,
		1.0f + position.x, 1.0f + position.y, 0.0f + position.z, 1.0f, 1.0f,
		1.0f + position.x, 0.0f + position.y, 0.0f + position.z, 1.0f, 0.0f,
		0.0f + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,

		0.0f + position.x, 1.0f + position.y, 1.0f + position.z, 1.0f, 1.0f,
		0.0f + position.x, 1.0f + position.y, 0.0f + position.z, 0.0f, 1.0f,
		0.0f + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,
		0.0f + position.x, 0.0f + position.y, 1.0f + position.z, 1.0f, 0.0f,

		1.0f + position.x, 1.0f + position.y, 0.0f + position.z, 0.0f, 1.0f,
		1.0f + position.x, 1.0f + position.y, 1.0f + position.z, 1.0f, 1.0f,
		1.0f + position.x, 0.0f + position.y, 1.0f + position.z, 1.0f, 0.0f,
		1.0f + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,

		1.0f + position.x, 1.0f + position.y, 1.0f + position.z, 1.0f, 1.0f,
		1.0f + position.x, 1.0f + position.y, 0.0f + position.z, 0.0f, 1.0f,
		0.0f + position.x, 1.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,
		0.0f + position.x, 1.0f + position.y, 1.0f + position.z, 1.0f, 0.0f,

		1.0f + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 1.0f,
		1.0f + position.x, 0.0f + position.y, 1.0f + position.z, 1.0f, 1.0f,
		0.0f + position.x, 0.0f + position.y, 1.0f + position.z, 1.0f, 0.0f,
		0.0f + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,
	};

	std::vector<unsigned int> blockIndices = {
		0, 1, 2,
		0, 2, 3,

		4, 5, 6,
		4, 6, 7,

		8, 9, 10,
		8, 10, 11,

		12, 13, 14,
		12, 14, 15,

		16, 17, 18,
		16, 18, 19,

		20, 21, 22,
		20, 22, 23
	};

	for (unsigned int& index : blockIndices)
	{
		if (elements.size())
		{
			index += elements.size() / 36 * 24;
		}
	}

	data.insert(data.end(), cubeData.begin(), cubeData.end());
	elements.insert(elements.end(), blockIndices.begin(), blockIndices.end());
}

Mesh& ChunkMesh::getMesh()
{
	return mesh;
}

Material& ChunkMesh::getMaterial()
{
	return material;
}

void ChunkMesh::rebuildMesh()
{
	data.clear();
	elements.clear();
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
				//if (!isBlockSurroundedBySolidBlocks(position))
				//{
				//	setBlock(position);
				//	elementsCount++;
				//}

				for (int i = 0; i < 6; ++i)
				{
					Face face = static_cast<Face>(i);
					if (isFaceNextToAir(face, position))
					{
						setFace(face, position);
						++elementsCount;
					}
					else
					{
						// std::cout << "My " << i << " side is next to air"
					}
				}
			}
		}

		if (!mesh.isDynamic())
		{
			MeshBuilder builder;
			unsigned long long s = (chunkSize * chunkSize * chunkSize) * 5 * sizeof(float) * 36;
			unsigned long long e = (chunkSize * chunkSize * chunkSize) * 3 * sizeof(unsigned int) * 36;
			mesh = builder.buildDyanmic(s, e);
		}

		mesh.setBufferSubData(0, data);
		mesh.setElementsSubData(0, elements);

		mesh.setPositionsCount(data.size() / 5);
		mesh.setIndicesCount(elements.size());
		
		std::cout << "Mesh  vertex  draw count: " << data.size() / 5 << std::endl;
		std::cout << "Mesh elements draw count: " << elements.size() << std::endl;
	}
}

bool ChunkMesh::isFaceNextToAir(Face face, const glm::ivec3& blockPosition)
{
	if (!chunk)
	{
		return true;
	}

	const std::vector<unsigned int>& blocks = chunk->getBlocks();

	switch (face)
	{
	case Face::Front:
		if (blockPosition.z < chunkSize - 1)
		{
			glm::ivec3 position = blockPosition;
			position.z++;
			if (chunk->getBlock(position) != 0)
			{
				return false;
			}
		}
		break;

	case Face::Back:
		if (blockPosition.z > 0)
		{
			glm::ivec3 position = blockPosition;
			position.z--;
			if (chunk->getBlock(position) != 0)
			{
				return false;
			}
		}
		break;

	case Face::Left:
		if (blockPosition.x > 0)
		{
			glm::ivec3 position = blockPosition;
			position.x--;
			if (chunk->getBlock(position) != 0)
			{
				return false;
			}
		}
		break;

	case Face::Right:
		if (blockPosition.x < chunkSize - 1)
		{
			glm::ivec3 position = blockPosition;
			position.x++;
			if (chunk->getBlock(position) != 0)
			{
				return false;
			}
		}
		break;

	case Face::Top:
		if (blockPosition.y < chunkSize - 1)
		{
			glm::ivec3 position = blockPosition;
			position.y++;
			if (chunk->getBlock(position) != 0)
			{
				return false;
			}
		}
		break;

	case Face::Bottom:
		if (blockPosition.y > 0)
		{
			glm::ivec3 position = blockPosition;
			position.y--;
			if (chunk->getBlock(position) != 0)
			{
				return false;
			}
		}
		break;
	}

	return true;
}

void ChunkMesh::setFace(Face face, glm::ivec3 position)
{
	std::vector<float> frontData = {
		1.0f + position.x, 1.0f + position.y, 1.0f + position.z, 1.0f, 1.0f,
		0.0f + position.x, 1.0f + position.y, 1.0f + position.z, 0.0f, 1.0f,
		0.0f + position.x, 0.0f + position.y, 1.0f + position.z, 0.0f, 0.0f,
		1.0f + position.x, 0.0f + position.y, 1.0f + position.z, 1.0f, 0.0f,
	};

	std::vector<float> backData = {
		0.0f + position.x, 1.0f + position.y, 0.0f + position.z, 0.0f, 1.0f,
		1.0f + position.x, 1.0f + position.y, 0.0f + position.z, 1.0f, 1.0f,
		1.0f + position.x, 0.0f + position.y, 0.0f + position.z, 1.0f, 0.0f,
		0.0f + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,
	};

	std::vector<float> leftData = {
		0.0f + position.x, 1.0f + position.y, 1.0f + position.z, 1.0f, 1.0f,
		0.0f + position.x, 1.0f + position.y, 0.0f + position.z, 0.0f, 1.0f,
		0.0f + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,
		0.0f + position.x, 0.0f + position.y, 1.0f + position.z, 1.0f, 0.0f,
	};

	std::vector<float> rightData = {
		1.0f + position.x, 1.0f + position.y, 0.0f + position.z, 0.0f, 1.0f,
		1.0f + position.x, 1.0f + position.y, 1.0f + position.z, 1.0f, 1.0f,
		1.0f + position.x, 0.0f + position.y, 1.0f + position.z, 1.0f, 0.0f,
		1.0f + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,
	};

	std::vector<float> topData = {
		1.0f + position.x, 1.0f + position.y, 1.0f + position.z, 1.0f, 1.0f,
		1.0f + position.x, 1.0f + position.y, 0.0f + position.z, 0.0f, 1.0f,
		0.0f + position.x, 1.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,
		0.0f + position.x, 1.0f + position.y, 1.0f + position.z, 1.0f, 0.0f,
	};

	std::vector<float> bottomData = {
		1.0f + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 1.0f,
		1.0f + position.x, 0.0f + position.y, 1.0f + position.z, 1.0f, 1.0f,
		0.0f + position.x, 0.0f + position.y, 1.0f + position.z, 1.0f, 0.0f,
		0.0f + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,
	};

	std::vector<unsigned int> faceIndices = {
		0, 1, 2,
		0, 2, 3,

		//4, 5, 6,
		//4, 6, 7,

		//8, 9, 10,
		//8, 10, 11,

		//12, 13, 14,
		//12, 14, 15,

		//16, 17, 18,
		//16, 18, 19,

		//20, 21, 22,
		//20, 22, 23
	};

	for (unsigned int& index : faceIndices)
	{
		if (elements.size())
		{
			index += elements.size() / 6 * 4;
		}
	}

	switch (face)
	{
	case ChunkMesh::Face::Front:
		data.insert(data.end(), frontData.begin(), frontData.end());
		break;
	case ChunkMesh::Face::Back:
		data.insert(data.end(), backData.begin(), backData.end());
		break;
	case ChunkMesh::Face::Left:
		data.insert(data.end(), leftData.begin(), leftData.end());
		break;
	case ChunkMesh::Face::Right:
		data.insert(data.end(), rightData.begin(), rightData.end());
		break;
	case ChunkMesh::Face::Top:
		data.insert(data.end(), topData.begin(), topData.end());
		break;
	case ChunkMesh::Face::Bottom:
		data.insert(data.end(), bottomData.begin(), bottomData.end());
		break;
	}

	elements.insert(elements.end(), faceIndices.begin(), faceIndices.end());
}

bool ChunkMesh::isBlockSurroundedBySolidBlocks(glm::ivec3 blockPosition)
{
	size_t count = 0;

	const std::vector<unsigned int>& blocks = chunk->getBlocks();

	if (blockPosition.x < chunkSize - 1)
	{
		unsigned int index = (blockPosition.z * chunkSize * chunkSize) + (blockPosition.y * chunkSize) + blockPosition.x + 1;
		if (blocks[index] != 0)
		{
			++count;
		}
	}

	if (blockPosition.x > 0)
	{
		unsigned int index = (blockPosition.z * chunkSize * chunkSize) + (blockPosition.y * chunkSize) + blockPosition.x - 1;
		if (blocks[index] != 0)
		{
			++count;
		}
	}

	if (blockPosition.y < chunkSize - 1)
	{
		unsigned int index = (blockPosition.z * chunkSize * chunkSize) + (blockPosition.y * chunkSize + 1) + blockPosition.x;
		if (blocks[index] != 0)
		{
			++count;
		}
	}

	if (blockPosition.y > 0)
	{
		unsigned int index = (blockPosition.z * chunkSize * chunkSize) + (blockPosition.y * chunkSize - 1) + blockPosition.x;
		if (blocks[index] != 0)
		{
			++count;
		}
	}

	if (blockPosition.z < chunkSize - 1)
	{
		unsigned int index = (blockPosition.z * chunkSize * chunkSize + 1) + (blockPosition.y * chunkSize) + blockPosition.x;
		if (blocks[index] != 0)
		{
			++count;
		}
	}

	if (blockPosition.z > 0)
	{
		unsigned int index = (blockPosition.z * chunkSize * chunkSize - 1) + (blockPosition.y * chunkSize) + blockPosition.x;
		if (blocks[index] != 0)
		{
			++count;
		}
	}

	return count == 6;
}
