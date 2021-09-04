#pragma once

#include <vector>

#include <glm/glm.hpp>

class Mesh
{
public:
	Mesh();
	Mesh(unsigned long long dynamicBufferSize);
	Mesh(std::vector<glm::vec3> positions, std::vector<unsigned int> indices);
	Mesh(std::vector<glm::vec3> positions, std::vector<glm::vec2> uvs, std::vector<unsigned int> indices);

	void setPositions(std::vector<glm::vec3> positions);
	void setUVs(std::vector<glm::vec2> uvs);
	void setIndices(std::vector<unsigned int> indices);

	void finalize();

	const unsigned int getVAO() const;

	unsigned int getPositionsCount() const;
	unsigned int getIndicesCount() const;
	void setPositionsCount(unsigned int positionsCount);

	// void setBufferSubData(unsigned int offset, const std::vector<glm::vec3>& positions, const std::vector<glm::vec2>& uvs);
	void setBufferSubData(unsigned int offset, const std::vector<float>& data);

private:
	unsigned int vbo;
	unsigned int vao;
	unsigned int ebo;

	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned int> indices;

	unsigned int positionsCount;
	unsigned int indicesCount;

	unsigned long long dynamicBufferSize;
};
