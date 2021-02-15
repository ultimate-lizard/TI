#include "FrameBuffer.h"

#include <iostream>

#include <glad/glad.h>

FrameBuffer::FrameBuffer(int width, int height) :
	width(width),
	height(height),
	framebufferId(0),
	renderbufferId(0)
{
	
}

FrameBuffer::FrameBuffer() :
	FrameBuffer(0, 0)
{

}

FrameBuffer::~FrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &framebufferId);
}

void FrameBuffer::init()
{
	glGenFramebuffers(1, &framebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);

	texture.create(width, height);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.getId(), 0);

	glGenRenderbuffers(1, &renderbufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbufferId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbufferId);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << glGetError() << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::setSize(int width, int height)
{
	this->width = width;
	this->height = height;
}

void FrameBuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
}

void FrameBuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Texture& FrameBuffer::getTexture()
{
	return texture;
}

//unsigned int FrameBuffer::getTextureId() const
//{
//	return textureId;
//}
//
//void FrameBuffer::bindTexture()
//{
//	glBindTexture(GL_TEXTURE_2D, textureId);
//}
