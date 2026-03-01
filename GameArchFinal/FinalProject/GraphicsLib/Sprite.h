#pragma once

#include <Trackable.h>
#include <Vector2D.h>

class GraphicsBuffer;

class Sprite : public Trackable
{
public:
	Sprite(GraphicsBuffer* pBuffer, const Vector2D& ulCorner, unsigned int width, unsigned int height, Vector2D displayLocation = ZERO_VECTOR2D);
	Sprite();//default constructor
	~Sprite();//implement this destructor

	//implement these functions for real
	GraphicsBuffer* getBuffer() const { return mpGraphicsBuffer; };
	const Vector2D& getSourceLoc() const { return mSourceLoc; };
	unsigned int getWidth() const { return mWidth; };
	unsigned int getHeight() const { return mHeight; };
	const Vector2D& getDisplayLocation() const { return mDisplayLocation; };
	void setIsEnabled(bool enabled) { mEnabled = enabled; };
	bool isEnabled() const { return mEnabled; }
private:

	int mWidth = 0;
	int mHeight = 0;
	Vector2D mDisplayLocation = ZERO_VECTOR2D;
	bool mEnabled = true;
	GraphicsBuffer* mpGraphicsBuffer = nullptr;
	Vector2D mSourceLoc = ZERO_VECTOR2D;
};