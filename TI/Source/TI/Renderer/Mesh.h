#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>

// TODO: Right now the dynamic mesh has hardcoded layout. Implement passing layouts to mesh in the future?
class Mesh
{
	friend class MeshBuilder;

public:
	Mesh() = default;
	Mesh(const Mesh&) = delete;
	Mesh(Mesh&&) = delete;
	~Mesh();

	const unsigned int getVAO() const;
	const unsigned int getEBO() const;

	void setPositionsCount(size_t count);
	void setIndicesCount(size_t count);

	size_t getPositionsCount() const;
	size_t getIndicesCount() const;

	bool isDynamic() const;

	void setBufferSubData(unsigned int offset, const std::vector<float>& data);
	void setElementsSubData(unsigned int offset, const std::vector<unsigned int>& data);

	void setBufferSize(unsigned int size);
	void setElementsSize(unsigned int size);

private:
	Mesh(unsigned long long vboSize, unsigned long long eboSize);
	Mesh(std::vector<glm::vec3> positions, std::vector<glm::vec2> uvs, std::vector<unsigned int> indices);

private:
	unsigned int vbo;
	unsigned int vao;
	unsigned int ebo;

	size_t positionsCount;
	size_t indicesCount;

	bool dynamic;
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

	std::unique_ptr<Mesh> build();
	std::unique_ptr<Mesh> buildDyanmic(unsigned long long vboSize, unsigned long long eboSize);

private:
	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned int> indices;
};
