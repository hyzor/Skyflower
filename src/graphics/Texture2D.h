#ifndef GRAPHICS_TEXTURE2D_H
#define GRAPHICS_TEXTURE2D_H

class Texture2D
{
public:
	virtual ~Texture2D() {}

	virtual void UploadData(const void *data) = 0;
};

#endif
