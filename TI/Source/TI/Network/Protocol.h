#pragma once

enum class PacketId
{
	Invalid,
	CConnectionRequest,
	/*
		std::string clientName;
	*/
	SConnectionResponse,
	SEntityInitialSync,
	/*
		std::string name;
		std::string id;

		glm::vec3 position;
		glm::vec3 rotation;
	*/
	SFinishInitialEntitySync,
	CFinishInitialEntitySync,
	SEntitySync,
	/*
		std::string id;

		glm::vec3 position;
		glm::vec3 rotation;
	*/
	CPlayerSync,
	/*
		std::string name;

		glm::vec3 position;
		glm::vec3 rotation;
	*/
};
