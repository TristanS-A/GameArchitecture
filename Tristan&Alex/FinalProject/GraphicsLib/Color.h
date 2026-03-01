#pragma once

#include <SDL.h>

#include <Trackable.h>

class GraphicsSystem;

class Color :public Trackable
{
	friend class GraphicsBuffer;//allows GraphicsBuffer functions to call getAllegroColorFromColor
public:
	Color() {};//default constructor
	Color(int r, int g, int b, int a = 255);//construct Color from int values
	Color(float r, float g, float b, float a = 1.0f);//construct Color from float values

	//implement these functions for real
	int getR() const { return mRed; };
	int getG() const { return mGreen; };
	int getB() const { return mBlue; };
	int getA() const { return mAlpha; };

private:
	//use variable(s) to hold values to represent color
	int mRed = 0;
	int mGreen = 0;
	int mBlue = 0;
	int mAlpha = 0;

	//map a Color to an ALLEGRO_COLOR
	static SDL_Color getSDLColorFromColor(const Color& color);
	static Uint32 getUint32ColorFromColor(const Color& color);

};
