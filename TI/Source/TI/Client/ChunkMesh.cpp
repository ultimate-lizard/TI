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

				setBlock(glm::vec3(x, y, z));

				if (i > 32)
					break;
			}
		}
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
	// Copy block to mesh data.
	// Start: (position.z * size * size) + (position.y * size) + position.x
	// End block.size()

	std::vector<glm::vec3> positions = {
		// verticies, uvs
		// front
		glm::vec3( 0.5f + position.x,  0.5f + position.y,  0.5f + position.z),
		glm::vec3(-0.5f + position.x,  0.5f + position.y,  0.5f + position.z),
		glm::vec3(-0.5f + position.x, -0.5f + position.y,  0.5f + position.z),
		glm::vec3( 0.5f + position.x,  0.5f + position.y,  0.5f + position.z),
		glm::vec3(-0.5f + position.x, -0.5f + position.y,  0.5f + position.z),
		glm::vec3( 0.5f + position.x, -0.5f + position.y,  0.5f + position.z),

		glm::vec3(-0.5f + position.x,  0.5f + position.y, -0.5f + position.z),
		glm::vec3( 0.5f + position.x,  0.5f + position.y, -0.5f + position.z),
		glm::vec3(-0.5f + position.x, -0.5f + position.y, -0.5f + position.z),
		glm::vec3( 0.5f + position.x,  0.5f + position.y, -0.5f + position.z),
		glm::vec3( 0.5f + position.x, -0.5f + position.y, -0.5f + position.z),
		glm::vec3(-0.5f + position.x, -0.5f + position.y, -0.5f + position.z),

		glm::vec3(-0.5f + position.x,  0.5f + position.y,  0.5f + position.z),
		glm::vec3(-0.5f + position.x,  0.5f + position.y, -0.5f + position.z),
		glm::vec3(-0.5f + position.x, -0.5f + position.y, -0.5f + position.z),
		glm::vec3(-0.5f + position.x,  0.5f + position.y,  0.5f + position.z),
		glm::vec3(-0.5f + position.x, -0.5f + position.y, -0.5f + position.z),
		glm::vec3(-0.5f + position.x, -0.5f + position.y,  0.5f + position.z),
		glm::vec3( 0.5f + position.x,  0.5f + position.y, -0.5f + position.z),
		glm::vec3( 0.5f + position.x,  0.5f + position.y,  0.5f + position.z),
		glm::vec3( 0.5f + position.x, -0.5f + position.y, -0.5f + position.z),
		glm::vec3( 0.5f + position.x,  0.5f + position.y,  0.5f + position.z),
		glm::vec3( 0.5f + position.x, -0.5f + position.y,  0.5f + position.z),
		glm::vec3( 0.5f + position.x, -0.5f + position.y, -0.5f + position.z),
		glm::vec3( 0.5f + position.x,  0.5f + position.y,  0.5f + position.z),
		glm::vec3( 0.5f + position.x,  0.5f + position.y, -0.5f + position.z),
		glm::vec3(-0.5f + position.x,  0.5f + position.y, -0.5f + position.z),
		glm::vec3( 0.5f + position.x,  0.5f + position.y,  0.5f + position.z),
		glm::vec3(-0.5f + position.x,  0.5f + position.y, -0.5f + position.z),
		glm::vec3(-0.5f + position.x,  0.5f + position.y,  0.5f + position.z),
		glm::vec3( 0.5f + position.x, -0.5f + position.y, -0.5f + position.z),
		glm::vec3( 0.5f + position.x, -0.5f + position.y,  0.5f + position.z),
		glm::vec3(-0.5f + position.x, -0.5f + position.y, -0.5f + position.z),
		glm::vec3( 0.5f + position.x, -0.5f + position.y,  0.5f + position.z),
		glm::vec3(-0.5f + position.x, -0.5f + position.y,  0.5f + position.z),
		glm::vec3(-0.5f + position.x, -0.5f + position.y, -0.5f + position.z)
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
	};

	
	unsigned int offset = (position.z * chunkSize * chunkSize) + (position.y * chunkSize) + position.x;
	offset *= 180 * sizeof(float);
	mesh->setBufferSubData(offset, positions, uvs);
}

Mesh* ChunkMesh::getMesh()
{
	return mesh;
}

Material* ChunkMesh::getMaterial()
{
	return material;
}
