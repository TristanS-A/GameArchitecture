#include "Sprite.h"

//implement me
Sprite::Sprite(GraphicsBuffer* pBuffer, const Vector2D& ulCorner, unsigned int width, unsigned int height, Vector2D displayLocation)
{
	mpGraphicsBuffer = pBuffer;
	mSourceLoc = ulCorner;
	mWidth = width;
	mHeight = height;
	mDisplayLocation = displayLocation;
}

//implement me
Sprite::Sprite()
{
	//I beleive this should be left blank because there is a
	//lack of information about what shouold go here and this 
	//constructor is never called.
}

//implement me
Sprite::~Sprite()
{
	//I don't beleive there is a need to delete anything here 
	//since the Graphics Buffer pointer is used for multiple 
	//sprites and is deleted later
}
