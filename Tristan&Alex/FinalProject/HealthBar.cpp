#include "HealthBar.h"

HealthBar::HealthBar(Vector2D location, Vector2D targetLoction, float offset)
{
}

HealthBar::~HealthBar()
{
}

void HealthBar::init()
{
}

void HealthBar::cleanup()
{
	
}

void HealthBar::draw(GraphicsBuffer* buffer, double scale)
{
	buffer->draw(mLocation, *mpHealthBarBuffer, scale);
}
