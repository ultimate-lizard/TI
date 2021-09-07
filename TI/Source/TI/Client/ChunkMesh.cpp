#include "ChunkMesh.h"

#include <TI/Server/Chunk.h>

#include <glad/glad.h>

// TODO: Change chunk pointer to reference
ChunkMesh::ChunkMesh(const Chunk* const chunk) :
	chunkSize(0),
	mesh(),
	chunk(chunk)
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
		1.0f + position.x, 1.0f + position.y, 1.0f + position.z, 1.0f, 1.0f,
		0.0f + position.x, 0.0f + position.y, 1.0f + position.z, 0.0f, 0.0f,
		1.0f + position.x, 0.0f + position.y, 1.0f + position.z, 1.0f, 0.0f,
		0.0f + position.x, 1.0f + position.y, 0.0f + position.z, 0.0f, 1.0f,
		1.0f + position.x, 1.0f + position.y, 0.0f + position.z, 1.0f, 1.0f,
		0.0f + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,
		1.0f + position.x, 1.0f + position.y, 0.0f + position.z, 1.0f, 1.0f,
		1.0f + position.x, 0.0f + position.y, 0.0f + position.z, 1.0f, 0.0f,
		0.0f + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,
		0.0f + position.x, 1.0f + position.y, 1.0f + position.z, 1.0f, 1.0f,
		0.0f + position.x, 1.0f + position.y, 0.0f + position.z, 0.0f, 1.0f,
		0.0f + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,
		0.0f + position.x, 1.0f + position.y, 1.0f + position.z, 1.0f, 1.0f,
		0.0f + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,
		0.0f + position.x, 0.0f + position.y, 1.0f + position.z, 1.0f, 0.0f,
		1.0f + position.x, 1.0f + position.y, 0.0f + position.z, 0.0f, 1.0f,
		1.0f + position.x, 1.0f + position.y, 1.0f + position.z, 1.0f, 1.0f,
		1.0f + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,
		1.0f + position.x, 1.0f + position.y, 1.0f + position.z, 1.0f, 1.0f,
		1.0f + position.x, 0.0f + position.y, 1.0f + position.z, 1.0f, 0.0f,
		1.0f + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,
		1.0f + position.x, 1.0f + position.y, 1.0f + position.z, 1.0f, 1.0f,
		1.0f + position.x, 1.0f + position.y, 0.0f + position.z, 0.0f, 1.0f,
		0.0f + position.x, 1.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,
		1.0f + position.x, 1.0f + position.y, 1.0f + position.z, 1.0f, 1.0f,
		0.0f + position.x, 1.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,
		0.0f + position.x, 1.0f + position.y, 1.0f + position.z, 1.0f, 0.0f,
		1.0f + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 1.0f,
		1.0f + position.x, 0.0f + position.y, 1.0f + position.z, 1.0f, 1.0f,
		0.0f + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 0.0f,
		1.0f + position.x, 0.0f + position.y, 1.0f + position.z, 1.0f, 1.0f,
		0.0f + position.x, 0.0f + position.y, 1.0f + position.z, 1.0f, 0.0f,
		0.0f + position.x, 0.0f + position.y, 0.0f + position.z, 0.0f, 0.0f
	};

	data.insert(data.end(), cubeData.begin(), cubeData.end());
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
				if (!isBlockSurroundedBySolidBlocks(chunk, position))
				{
					setBlock(position);
				}
			}
		}

		if (!mesh.isDynamic())
		{
			MeshBuilder builder;
			unsigned long long s = (chunkSize * chunkSize * chunkSize) * 5 * sizeof(float) * 36;
			mesh = builder.buildDyanmic(s);
		}

		mesh.setBufferSubData(0, data);
		mesh.setPositionsCount((data.size() / 5) * 3);
	}
}

bool ChunkMesh::isBlockSurroundedBySolidBlocks(const Chunk* const chunk, glm::ivec3 blockPosition)
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
