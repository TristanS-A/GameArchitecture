#include "FireGunEvent.h"

FireGunEvent::FireGunEvent(Vector2D shootAtPoint, Vector2D shotFromPoint, bool shotByPlayer) : 
	GameEvent(FIRE_GUN), mShootAtPoint(shootAtPoint), mShotFromPoint(shotFromPoint), mShotByPlayer(shotByPlayer)
{
}

FireGunEvent::~FireGunEvent()
{
}

Vector2D FireGunEvent::getShootAtPoint() const
{
	return mShootAtPoint;
}

Vector2D FireGunEvent::getShotFromPoint() const
{
	return mShotFromPoint;
}

bool FireGunEvent::getShotByPlayer() const
{
	return mShotByPlayer;
}
