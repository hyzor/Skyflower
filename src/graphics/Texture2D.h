#ifndef GRAPHICS_TEXTURE2D_H
#define GRAPHICS_TEXTURE2D_H

class Texture2D
{
public:
	virtual ~Texture2D() {}

	virtual void UploadData(const void *data) = 0;

	virtual unsigned int GetWidth() const = 0;
	virtual unsigned int GetHeight() const = 0;

	virtual bool IsRenderable() const = 0;
};

#endif
