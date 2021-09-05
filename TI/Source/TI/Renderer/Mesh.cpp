#include "Mesh.h"

#include <glad/glad.h>

Mesh::Mesh(unsigned long long size) :
	vbo(0),
	vao(0),
	ebo(0),
	positionsCount(0),
	indicesCount(0),
	size(size)
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));

	glBindVertexArray(0);
}

Mesh::Mesh(std::vector<glm::vec3> positions, std::vector<glm::vec2> uvs, std::vector<unsigned int> indices) :
	vbo(0),
	vao(0),
	ebo(0),
	positionsCount(positions.size()),
	indicesCount(indices.size()),
	size(0)
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	std::vector<float> data;
	for (size_t i = 0; i < positions.size(); ++i)
	{
		data.push_back(positions[i].x);
		data.push_back(positions[i].y);
		data.push_back(positions[i].z);

		if (!uvs.empty())
		{
			data.push_back(uvs[i].x);
			data.push_back(uvs[i].y);
		}
	}

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

	if (!indices.empty())
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
	}

	unsigned long long offset = 0;
	unsigned int stride = 3 * sizeof(float);

	if (!uvs.empty())
	{
		stride += 2 * sizeof(float);
	}

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(offset));
	offset += 3 * sizeof(float);

	if (!uvs.empty())
	{
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(offset));
		offset += 2 * sizeof(float);
	}

	glBindVertexArray(0);
}

const unsigned int Mesh::getVAO() const
{
	return vao;
}

void Mesh::setPositionsCount(size_t count)
{
	positionsCount = count;
}

void Mesh::setIndicesCount(size_t count)
{
	indicesCount = count;
}

size_t Mesh::getPositionsCount() const
{
	return positionsCount;
}

size_t Mesh::getIndicesCount() const
{
	return indicesCount;
}

bool Mesh::isDynamic() const
{
	return size != 0;
}

unsigned long long Mesh::getSize() const
{
	return size;
}

void Mesh::setBufferSubData(unsigned int offset, const std::vector<float>& data)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, offset, data.size() * sizeof(float), data.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshBuilder::setPositions(std::vector<glm::vec3> positions)
{
	this->positions = positions;
}

void MeshBuilder::setUVs(std::vector<glm::vec2> uvs)
{
	this->uvs = uvs;
}

void MeshBuilder::setIndices(std::vector<unsigned int> indices)
{
	this->indices = indices;
}

Mesh MeshBuilder::build()
{
	return Mesh(std::move(positions), std::move(uvs), std::move(indices));
}

Mesh MeshBuilder::buildDyanmic(unsigned long long size)
{
	return Mesh(size);
}
