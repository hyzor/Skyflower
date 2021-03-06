#ifndef SKYFLOWER_GUIELEMENT_H
#define SKYFLOWER_GUIELEMENT_H

//#include <DirectXMath.h>
#include "Graphics/GraphicsEngineImpl.h"

using namespace DirectX;

/*
	Simple container class encapsulating a Texture2D and related data to be used when rendering it, such as position, origin, scale etc.
*/

class GUIElement
{
private:
	Texture2D* mTexture;
	Draw2DInput mDrawInput;

	bool mIsVisible;
	bool mFromFile;
	std::string mFile;
	int mId;

public:
	GUIElement();
	GUIElement(const GUIElement& other);
	~GUIElement();

	void* operator new(size_t size)
	{
		void* p = _aligned_malloc(size, 16);

		if (!p)
			throw std::bad_alloc();

		return p;
	}

	void operator delete(void* p)
	{
		_aligned_free(p);
	}

	//Destroy GUI element
	void Destroy(GraphicsEngine* gEngine);

	//Draw this particular GUI element using the texture it points to as well as the other data it contains
	void Draw(GraphicsEngine* gEngine);

	//Upload data to the texture pointed to by this GUI element
	void UploadTextureData(const void* data);

	//Standard set-functions
	void SetTexture(Texture2D* tex) { mTexture = tex; }
	void SetDrawInput(Draw2DInput* input) { mDrawInput = *input; }
	void SetVisible(bool flag) { mIsVisible = flag; }
	void SetFromFile(bool flag) { mFromFile = flag; }
	void SetFile(std::string file){ mFile = file; }
	void SetID(int id){ mId = id; }

	//Standard get-functions
	Texture2D* GetTexture() const { return mTexture; }
	Draw2DInput* GetDrawInput() { return &mDrawInput; }
	bool GetVisible() const { return mIsVisible; }
	bool GetFromFile() const { return mFromFile; }
	std::string GetFile() const { return mFile; }
	int GetID() const { return mId; }
};

class TextElement
{
public:
	TextElement();
	TextElement(std::string text, int x, int y, Vec3 color = Vec3::Zero(), float scale = 1.0f, float alpha = 1.0f);
	void Draw(GraphicsEngine* gEngine);

	std::string text;
	int x, y;
	float scale;
	float alpha;
	Vec3 color;
	


};

#endif