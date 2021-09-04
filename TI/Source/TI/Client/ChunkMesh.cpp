#include "ChunkMesh.h"

#include <TI/Server/Chunk.h>
#include <TI/Renderer/Mesh.h>
#include <TI/Renderer/Material.h>

#include <glad/glad.h>

// TODO: Switch pointer to reference
ChunkMesh::ChunkMesh(const Chunk* const chunk) :
	chunkSize(0),
	mesh(nullptr),
	material(nullptr)
{
	if (chunk)
	{
		chunkSize = chunk->getSize();

		const unsigned int BLOCK_VERTEX_COUNT = 180;
		const unsigned long long CHUNK_SIZE = chunk->getSize();
		const unsigned long long BUFFER_SIZE = (BLOCK_VERTEX_COUNT * sizeof(float)) * (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);

		mesh = new Mesh(BUFFER_SIZE);
		material = new Material;
		material->setShader("../Shaders/SampleShader.vert", "../Shaders/SampleShader.frag");
		material->setTexture("../Textures/dirt.jpg");

		// Chunk stuff
		const std::vector<unsigned int>& blocks = chunk->getBlocks();
		for (size_t i = 0; i < blocks.size(); ++i)
		{
			if (blocks[i] != 0)
			{
				int index = i;
				int z = index / (chunkSize * chunkSize);
				index -= (z * chunkSize * chunkSize);
				int y = index / chunkSize;
				int x = index % chunkSize;

				// if (y == 0)
					// setBlock(glm::ivec3(x, y, z));
			}
		}

		setBlock(glm::vec3(1, 0, 0));
		setBlock(glm::vec3(2, 0, 0));

		std::vector<float> verticesToSend;
		for (const auto& pair : this->blocks)
		{
			const std::vector<float>& vertices = pair.second;
			for (float vertex : vertices)
			{
				verticesToSend.push_back(vertex);
			}
		}

		mesh->setBufferSubData(0, verticesToSend);
		mesh->setPositionsCount(this->blocks.size() * 36);
	}
}

ChunkMesh::~ChunkMesh()
{
	if (mesh)
	{
		delete mesh;
	}

	if (material)
	{
		delete material;
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

	/*std::vector<glm::vec3> positions = {
		glm::vec3(1.0f + position.x, 1.0f + position.y, 1.0f + position.z),
		glm::vec3(0.0f + position.x, 1.0f + position.y, 1.0f + position.z),
		glm::vec3(0.0f + position.x, 0.0f + position.y, 1.0f + position.z),
		glm::vec3(1.0f + position.x, 1.0f + position.y, 1.0f + position.z),
		glm::vec3(0.0f + position.x, 0.0f + position.y, 1.0f + position.z),
		glm::vec3(1.0f + position.x, 0.0f + position.y, 1.0f + position.z),
		glm::vec3(0.0f + position.x, 1.0f + position.y, 0.0f + position.z),
		glm::vec3(1.0f + position.x, 1.0f + position.y, 0.0f + position.z),
		glm::vec3(0.0f + position.x, 0.0f + position.y, 0.0f + position.z),
		glm::vec3(1.0f + position.x, 1.0f + position.y, 0.0f + position.z),
		glm::vec3(1.0f + position.x, 0.0f + position.y, 0.0f + position.z),
		glm::vec3(0.0f + position.x, 0.0f + position.y, 0.0f + position.z),
		glm::vec3(0.0f + position.x, 1.0f + position.y, 1.0f + position.z),
		glm::vec3(0.0f + position.x, 1.0f + position.y, 0.0f + position.z),
		glm::vec3(0.0f + position.x, 0.0f + position.y, 0.0f + position.z),
		glm::vec3(0.0f + position.x, 1.0f + position.y, 1.0f + position.z),
		glm::vec3(0.0f + position.x, 0.0f + position.y, 0.0f + position.z),
		glm::vec3(0.0f + position.x, 0.0f + position.y, 1.0f + position.z),
		glm::vec3(1.0f + position.x, 1.0f + position.y, 0.0f + position.z),
		glm::vec3(1.0f + position.x, 1.0f + position.y, 1.0f + position.z),
		glm::vec3(1.0f + position.x, 0.0f + position.y, 0.0f + position.z),
		glm::vec3(1.0f + position.x, 1.0f + position.y, 1.0f + position.z),
		glm::vec3(1.0f + position.x, 0.0f + position.y, 1.0f + position.z),
		glm::vec3(1.0f + position.x, 0.0f + position.y, 0.0f + position.z),
		glm::vec3(1.0f + position.x, 1.0f + position.y, 1.0f + position.z),
		glm::vec3(1.0f + position.x, 1.0f + position.y, 0.0f + position.z),
		glm::vec3(0.0f + position.x, 1.0f + position.y, 0.0f + position.z),
		glm::vec3(1.0f + position.x, 1.0f + position.y, 1.0f + position.z),
		glm::vec3(0.0f + position.x, 1.0f + position.y, 0.0f + position.z),
		glm::vec3(0.0f + position.x, 1.0f + position.y, 1.0f + position.z),
		glm::vec3(1.0f + position.x, 0.0f + position.y, 0.0f + position.z),
		glm::vec3(1.0f + position.x, 0.0f + position.y, 1.0f + position.z),
		glm::vec3(0.0f + position.x, 0.0f + position.y, 0.0f + position.z),
		glm::vec3(1.0f + position.x, 0.0f + position.y, 1.0f + position.z),
		glm::vec3(0.0f + position.x, 0.0f + position.y, 1.0f + position.z),
		glm::vec3(0.0f + position.x, 0.0f + position.y, 0.0f + position.z)
	};

	std::vector<glm::vec2> uvs = {
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f)
	};*/

	unsigned int index = (position.z * chunkSize * chunkSize) + (position.y * chunkSize) + position.x;

	blocks.emplace(index, cubeData);

	/*for (size_t i = 0; i < positions.size(); ++i)
	{
		this->positions.push_back(positions[i]);
	}*/
	
	// 1. Find this positions in the map
	// 2. If there is no positions, just add this block
	// 3. If there is position, replace data with the new block

	/*if (this->positions.size() < this->positions.size() + (offset * positions.size()) + positions.size())
	{
		this->positions.resize(this->positions.size() + offset * positions.size() + positions.size());
	}

	if (this->uvs.size() < this->uvs.size() + (offset * uvs.size()) + uvs.size())
	{
		this->uvs.resize(this->uvs.size() + offset * uvs.size() + uvs.size());
	}

	std::copy(positions.begin(), positions.end(), this->positions.begin() + offset * positions.size());
	std::copy(uvs.begin(), uvs.end(), this->uvs.begin() + offset * uvs.size());*/

	// unsigned int offsetInBytes = offset * 180 * sizeof(float);

	// mesh->setBufferSubData(offset, positions, uvs);
}

Mesh* ChunkMesh::getMesh()
{
	return mesh;
}

Material* ChunkMesh::getMaterial()
{
	return material;
}
