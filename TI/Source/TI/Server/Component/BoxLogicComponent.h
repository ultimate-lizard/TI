#pragma once

#include <TI/Server/Component/Component.h>

class Server;

class BoxLogicComponent : public Component
{
public:
	BoxLogicComponent(Server* server);

	virtual void tick(float dt) override;
	virtual std::unique_ptr<Component> clone() const override;

protected:
	Server* server;
};