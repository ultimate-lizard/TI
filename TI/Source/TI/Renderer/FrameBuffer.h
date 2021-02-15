#pragma once

#include <TI/Renderer/Texture.h>

class FrameBuffer
{
public:
	FrameBuffer();
	FrameBuffer(int width, int height);
	~FrameBuffer();

	void init();

	void setSize(int width, int height);

	void bind();
	void unbind();
	
	Texture& getTexture();

	// unsigned int getTextureId() const;

	// void bindTexture();

private:
	int width;
	int height;

	unsigned int framebufferId;
	Texture texture;
	unsigned int renderbufferId;
	// unsigned int textureId;
};
