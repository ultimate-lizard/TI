#include "Mesh.h"

#include <glad/glad.h>

Mesh::Mesh() :
	vbo(0),
	vao(0),
	ebo(0),
	dynamicBufferSize(0)
{
}

Mesh::Mesh(unsigned long long dynamicBufferSize) :
	vbo(0),
	vao(0),
	ebo(0),
	dynamicBufferSize(dynamicBufferSize)
{
	if (dynamicBufferSize)
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, dynamicBufferSize, nullptr, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));

		glBindVertexArray(0);
	}
}

Mesh::Mesh(std::vector<glm::vec3> positions, std::vector<unsigned int> indices) :
	vbo(0),
	vao(0),
	ebo(0),
	positions(positions),
	indices(indices),
	dynamicBufferSize(0)
{

}

Mesh::Mesh(std::vector<glm::vec3> positions, std::vector<glm::vec2> uvs, std::vector<unsigned int> indices) :
	vbo(0),
	vao(0),
	ebo(0),
	positions(positions),
	uvs(uvs),
	indices(indices),
	dynamicBufferSize(0)
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
	if (dynamicBufferSize)
	{
		// TODO: Write warning
		return;
	}

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

	glBindVertexArray(0);
}

const unsigned int Mesh::getVAO() const
{
	return vao;
}

const std::vector<unsigned int>& Mesh::getIndices() const
{
	return indices;
}

const std::vector<glm::vec3>& Mesh::getPositions() const
{
	return positions;
}

void Mesh::setBufferSubData(unsigned int subDataOffset, const std::vector<glm::vec3>& positions, const std::vector<glm::vec2>& uvs)
{
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

	if (this->positions.size() < subDataOffset + positions.size())
	{
		this->positions.resize(subDataOffset + positions.size());
	}

	if (this->uvs.size() < subDataOffset + uvs.size())
	{
		this->uvs.resize(subDataOffset + uvs.size());
	}

	int i = subDataOffset;
	for (const glm::vec3& position : positions)
	{
		this->positions[i++] = position;
	}

	i = subDataOffset;
	for (const glm::vec2& uv : uvs)
	{
		this->uvs[i++] = uv;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, subDataOffset, data.size() * sizeof(float), data.data());
}
