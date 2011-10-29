#ifndef World_H
#define World_H

#include <list>
#include <map>
//#include "RigidBody.h"
#include "Enums.h"
#include "Shape.h"
#include "BigMatrix.h"

enum GetType
{
	RIGHT_PRESSED,
	LEFT_PRESSED,
	INSIDE
};

class RigidBody;
class Arbiter;
class ArbiterKey;

class World
{
public:
	World();
	~World();

	void Init();	
	void Update(float dt);
	void Step(float dt);
	void Draw();
	void BroadPhase();
	void SetGravity(float gravity);

	void Add(RigidBody* body);
	void Remove(RigidBody* body);

	void Load(string source);
	void Save(string source);
	void ClearWorld();

	RigidBody* GetBody(Vector mousePos);

	void Pause();
	void Unpause();
	bool GetPaused();

private:
	std::vector<RigidBody*> mBodyList;
	std::map<ArbiterKey, Arbiter> arbiters;
	Vector mNormal;
	Vector mCollisionPoint;

	float GRAVITY;

	const double SMALL_NEGATIVE;
	const double SMALL_POSITIVE;

	bool mPaused;
};

#endif