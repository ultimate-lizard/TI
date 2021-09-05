#pragma once

#include <vector>

#include <glm/glm.hpp>

class Mesh
{
	friend class MeshBuilder;

public:
	Mesh() = default;
	Mesh(const Mesh&) = default;
	Mesh(Mesh&&) = default;
	Mesh& operator=(const Mesh&) = default;

	const unsigned int getVAO() const;

	void setPositionsCount(size_t count);
	void setIndicesCount(size_t count);

	size_t getPositionsCount() const;
	size_t getIndicesCount() const;

	bool isDynamic() const;
	unsigned long long getSize() const;

	void setBufferSubData(unsigned int offset, const std::vector<float>& data);

private:
	Mesh(unsigned long long size);
	Mesh(std::vector<glm::vec3> positions, std::vector<glm::vec2> uvs, std::vector<unsigned int> indices);

private:
	unsigned int vbo;
	unsigned int vao;
	unsigned int ebo;

	size_t positionsCount;
	size_t indicesCount;

	unsigned long long size;
};

class MeshBuilder
{
public:
	MeshBuilder() = default;
	MeshBuilder(const Mesh&) = delete;
	MeshBuilder(MeshBuilder&&) = delete;

	void setPositions(std::vector<glm::vec3> positions);
	void setUVs(std::vector<glm::vec2> uvs);
	void setIndices(std::vector<unsigned int> indices);

	Mesh build();
	Mesh buildDyanmic(unsigned long long size);

private:
	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned int> indices;
};
