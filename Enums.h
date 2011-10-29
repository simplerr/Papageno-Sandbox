#ifndef ENUMS_H
#define ENUMS_H

#include <string>
using namespace std;

#define PI 3.141592654
#define RAD_TO_DEG 57.29577951
#define LEFT_NORMAL 1
#define RIGHT_NORMAL -1
#define UP_NORMAL 1
#define DOWN_NORMAL -1

#include "Vector.h"

class Rect
{
public:
	Rect(){};
	Rect(float _left, float _right, float _top, float _bottom)	{
		left = _left;
		right = _right;
		top = _top;
		bottom = _bottom;
	}

	Rect(float x, float y, int width, int height)	{
		left = x - width/2;
		right = x + width/2;
		top = y - height/2;
		bottom = y + height/2;
	}

	void move(float dx, float dy)	{
		left += dx;
		right += dx;
		top += dy;
		bottom += dy;
	}

	Vector getPos()	{
		return Vector(left + (right-left)/2, top + (bottom-top)/2);
	}

	float getWidth()	{
		return right - left;
	}
	
	float getHeight()	{
		return bottom - top;
	}

	bool pointInside(Vector point)	{
		if(point.x > left && point.x < right && point.y > top && point.y < bottom)
			return true;
		else
			return false;
	}

	float left;
	float right;
	float top;
	float bottom;
};

enum State
{
	MENU,
	SANDBOX,
	ABOUT,
	HELP,
	NONE
};

enum DrawType
{
	ANIMATION,
	STAIC
};

enum Direction
{
	UP,
	DOWN,
	LEFT, 
	RIGHT
};

enum ObjectType
{
	PLAYER,
	PLATFORM,
	ENEMY,
	PLAYERS_GUN,
	ENEMIES_GUN,
	NORMAL_PROJECTILE,
	EFFECT,
	EMITTER,
	PARTICLE_RICOCHET,
	PARTICLE_BLOOD,
	PARTICLE_FIRE
};

enum ObjectOwner
{
	OWNER_PLAYER,
	OWNER_WORLD
};

#endif