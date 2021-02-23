#pragma once

enum class PacketId
{
	Invalid = 0,

	CConnectionRequest = 1,
	/*
		std::string clientName;
	*/

	SConnectionResponse = 2,

	SEntityInitialSync = 3,
	/*
		std::string name;
		std::string id;

		glm::vec3 position;
		glm::vec3 rotation;
	*/
	SFinishInitialEntitySync = 4,

	CFinishInitialEntitySync = 5,

	SEntitySync = 6,
	/*
		std::string id;

		glm::vec3 position;
		glm::vec3 rotation;
	*/
};
