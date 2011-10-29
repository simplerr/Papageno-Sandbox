#ifndef SANDBOX_H
#define SANDBOX_H

#include "MouseBox.h"
#include "Shape.h"
#include "GameState.h"

class World;
class RigidBody;
class HollowCircle;

class Sandbox : public GameState
{
public:
	Sandbox();
	~Sandbox();

	void update(float dt);
	void pausedUpdate(float dt);
	void dynamicUpdate(float dt);
	void draw();

	void stateChange();

	void createBody(float x, float y, int width, int height);
	void mouseBox();
private:
	World* mWorld;
	MouseBox mMouseBox;
	RigidBody* mActiveBody;
	float mLastRotation;

	HollowCircle* mRotationCircle;

	IDirect3DTexture9* mBkgdTexture;
	IDirect3DTexture9* mRotationTexture;
	
	bool mRotating;

	float SCALE_FACTOR;
};

class HollowCircle
{
public:
	HollowCircle(float x, float y, int radius, int innerRadius, string textureSource);
	~HollowCircle();

	void draw();
	void move(float dx, float dy);
	void setRotation(float rotation);

	bool pointInside(Vector point);
private:
	IDirect3DTexture9* mTexture;
	Vector mPos;
	float mRadius;
	float mInnerRadius;
	float mRotation;
};

#endif