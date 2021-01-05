#include "Mesh.h"

#include <glad/glad.h>

Mesh::Mesh() :
	vbo(0),
	vao(0),
	ebo(0)
{
}

Mesh::Mesh(std::vector<glm::vec3> positions, std::vector<unsigned int> indices) :
	vbo(0),
	vao(0),
	ebo(0),
	positions(positions),
	indices(indices)
{

}

Mesh::Mesh(std::vector<glm::vec3> positions, std::vector<glm::vec2> uvs, std::vector<unsigned int> indices) :
	vbo(0),
	vao(0),
	ebo(0),
	positions(positions),
	uvs(uvs),
	indices(indices)
{

}

void Mesh::setPositions(std::vector<glm::vec3> positions)
{
	this->positions = positions;
}

void Mesh::setUVs(std::vector<glm::vec2> uvs)
{
	this->uvs = uvs;
}

void Mesh::setIndices(std::vector<unsigned int> indices)
{
	this->indices = indices;
}

void Mesh::finalize()
{
	if (!vao)
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
	}

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

	int offset = 0;
	size_t stride = 3 * sizeof(float);

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

	// glBindVertexArray(0);
}
