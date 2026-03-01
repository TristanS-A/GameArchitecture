#pragma once

#include <SDL_ttf.h>

#include <Trackable.h>

#include <string>

class GraphicsSystem;

class Font:public Trackable
{
	friend class GraphicsBuffer;//allows GraphicsBuffer functions to call getAllegroFontAlignFlag
public:
	enum Alignment
	{
		LEFT,
		CENTER,
		RIGHT
	};

	Font(const std::string& filename,int size);
	~Font();

	int getSize() const { return mSize; };

private:
	TTF_Font * mpFont = nullptr;
	int mSize=0;
};
