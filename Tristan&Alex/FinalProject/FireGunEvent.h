#pragma once
#include "GameEvent.h"
#include <Vector2D.h>

class FireGunEvent : public GameEvent
{
public:
	FireGunEvent(Vector2D shootAtPoint, Vector2D shotFromPoint, bool shotByPlayer);
	~FireGunEvent();

	Vector2D getShootAtPoint() const;
	Vector2D getShotFromPoint() const;
	bool getShotByPlayer() const;

private:
	Vector2D mShootAtPoint = ZERO_VECTOR2D;
	Vector2D mShotFromPoint = ZERO_VECTOR2D;
	bool mShotByPlayer;
};

