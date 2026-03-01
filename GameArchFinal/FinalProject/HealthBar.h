#pragma once
#include <Trackable.h>
#include <Vector2D.h>
#include <GraphicsBuffer.h>

using namespace std;

class HealthBar : public Trackable
{
public:
	HealthBar(Vector2D location, Vector2D targetLoction, float offset);
	~HealthBar();

	void init();
	void cleanup();

	void draw(GraphicsBuffer* buffer, double scale);

private:
	Vector2D mLocation;
	int mHeight;
	int mWidth;
	GraphicsBuffer* mpHealthBarBuffer;
};
