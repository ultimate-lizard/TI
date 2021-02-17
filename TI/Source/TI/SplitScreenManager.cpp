#include "SplitScreenManager.h"

#include <TI/Application.h>
#include <TI/Renderer/Renderer.h>
#include <TI/Client/LocalClient.h>

SplitScreenManager::SplitScreenManager(Application* app) :
	host(nullptr),
	app(app),
	orientation(SplitScreenOrientation::Vertical)
{

}

void SplitScreenManager::setHost(LocalClient* host)
{
	this->host = host;
}

void SplitScreenManager::addGuest(LocalClient* guest)
{
	if (!guest)
	{
		return;
	}

	guests.push_back(guest);
}

void SplitScreenManager::setOrientation(SplitScreenOrientation orientation)
{
	this->orientation = orientation;
}

void SplitScreenManager::displayHost()
{
	if (host)
	{
		auto renderer = app->getRenderer();
		if (renderer)
		{
			auto viewport = renderer->getViewport(host->getViewportId());
			auto size = app->getWindow()->getSize();
			viewport->setSize(size);

			for (auto guest : guests)
			{
				auto viewport = renderer->getViewport(guest->getViewportId());
				if (!viewport)
				{
					renderer->createViewport(guest->getViewportId());
					viewport = renderer->getViewport(guest->getViewportId());
				}

				viewport->setEnabled(false);
			}
		}
	}
}

void SplitScreenManager::displayGuest(LocalClient* guest)
{

}

void SplitScreenManager::displayAll()
{
	int screenCount = 1 + static_cast<int>(guests.size());

	if (host)
	{
		auto renderer = app->getRenderer();
		if (renderer)
		{
			auto viewport = renderer->getViewport(host->getViewportId());

			viewport->setEnabled(true);

			glm::ivec2 size = app->getWindow()->getSize();

			if (orientation == SplitScreenOrientation::Vertical)
			{
				size.y /= screenCount;
			}
			if (orientation == SplitScreenOrientation::Horizontal)
			{
				size.x /= screenCount;
			}

			glm::ivec2 pos = { 0, 0 };

			viewport->setSize(size);
			viewport->setPosition(pos);

			for (auto guest : guests)
			{
				auto viewport = renderer->getViewport(guest->getViewportId());
				if (!viewport)
				{
					renderer->createViewport(guest->getViewportId());
					viewport = renderer->getViewport(guest->getViewportId());
				}

				viewport->setSize(size);

				if (orientation == SplitScreenOrientation::Vertical)
				{
					pos.y += size.y;
				}
				if (orientation == SplitScreenOrientation::Horizontal)
				{
					pos.x += size.x;
				}
				
				viewport->setPosition(pos);

				viewport->setEnabled(true);
			}
		}
	}
}
