#pragma once
#include "Unit.h"
#include "EventListener.h"
#include <vector>
#include <tuple>

class World : public Unit , public EventListener
{
public:
	World(vector<SDL_Rect> collisionRects, vector<Animation> animations, Vector2D location, float speed, float animationSpeed, int startingAnimationIndex);
	~World();

	void init();
	void cleanup();
	void addWorldUnits(vector<tuple<Unit*, vector<string>>> worldEnemyUnits, vector<tuple<Unit*, vector<string>>> worldNPCUnits);
	void setWorldBaracades(vector<SDL_Rect> baracadeRects);
	void removeAndDeleteEnemy(tuple<Unit*, vector<string>> unitToDelete);
	void removeNextBaracade();

	void handleEvent(const Event& theEvent);
	void worldMoveUp();
	void worldMoveDown();
	void worldMoveRight();
	void worldMoveLeft();
	void handleCollision(Unit* unitToHandleCollisionFor);
	void handleCollisionResponse(SDL_Rect collisionRect, Unit* unitToHandleCollisionFor);

	void setCanMoveOnXAxis(bool canMoveOnX) { mCanMoveOnX = canMoveOnX; };
	void setCanMoveOnYAxis(bool canMoveOnY) { mCanMoveOnY = canMoveOnY; };
	void setWorldSpeed(float speed) { mSpeed = speed; };
	bool getAllEnemeisDefeated() { return mWorldEnemys.size() == 0; };

	void draw(GraphicsBuffer* destBuffer, GraphicsBuffer* hud);
	void update(float deltaTime, Unit* playerRect);

	string getSaveData();
	void loadData(string fileData);
private:
	vector<tuple<Unit*, vector<string>>> mWorldEnemys = {};
	vector<tuple<Unit*, vector<string>>> mWorldNPCUnits = {};
	vector<SDL_Rect> mCollisionRects = {};
	vector<SDL_Rect> mBaracadeRects = {};
	bool mCanMoveOnX = false;
	bool mCanMoveOnY = false;
	int mTotalEnemies = 0;

	tuple<Unit*, vector<string>> mCurrUnitOverlappingPlayer;
};

