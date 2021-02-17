#pragma once

#include <TI/Client/Client.h>
#include <TI/Util/Config.h>

class InputHandler;
class IController;

class LocalClient : public Client
{
public:
	LocalClient(Application* app, const std::string& name = "");

	void update(float dt) override;

	void connect() override;
	void receiveServerConnectionResponse(ServerConnectionResponse response) override;

	void possesEntity(Entity* entity) override;

	InputHandler* const getInputHandler() const;
	IController* const getController() const;

	void shutdown() override;

	void setViewportId(unsigned int id);
	unsigned int getViewportId() const;

private:
	void loadConfig();
	void loadMappings();

	std::unique_ptr<InputHandler> inputHandler;
	std::unique_ptr<IController> playerController;

	Config config;

	unsigned int viewportId;
};
