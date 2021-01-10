#pragma once

#include <vector>

#include <glm/glm.hpp>

class Mesh
{
public:
	Mesh();
	Mesh(std::vector<glm::vec3> positions, std::vector<unsigned int> indices);
	Mesh(std::vector<glm::vec3> positions, std::vector<glm::vec2> uvs, std::vector<unsigned int> indices);

	void setPositions(std::vector<glm::vec3> positions);
	void setUVs(std::vector<glm::vec2> uvs);
	void setIndices(std::vector<unsigned int> indices);

	void finalize();

	const unsigned int getVAO() const;
	const std::vector<unsigned int>& getIndices() const;
	const std::vector<glm::vec3>& getPositions() const;

private:
	unsigned int vbo;
	unsigned int vao;
	unsigned int ebo;

	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned int> indices;
};
