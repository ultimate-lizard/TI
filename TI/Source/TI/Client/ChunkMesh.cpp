#include "ChunkMesh.h"

#include <TI/Server/Chunk.h>

#include <glad/glad.h>

// TODO: Switch pointer to reference
ChunkMesh::ChunkMesh(const Chunk* const chunk) :
	chunkSize(0),
	mesh()
{
	material.setShader("../Shaders/SampleShader.vert", "../Shaders/SampleShader.frag");
	material.setTexture("../Textures/dirt.jpg");

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

			if (blocks[i])
			{
				setBlock({ x, y, z });
			}
		}

		rebuildMesh();
	}
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
	if (!mesh.isDynamic())
	{
		MeshBuilder builder;
		unsigned long long s = (chunkSize * chunkSize * chunkSize) * 5 * sizeof(float) * 36;
		mesh = builder.buildDyanmic(s);
	}

	mesh.setBufferSubData(0, data);
	mesh.setPositionsCount((chunkSize * chunkSize * chunkSize) * 108);
}
