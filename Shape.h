#ifndef SHAPE_H
#define SHAPE_H

#include <vector>
#include <crtdbg.h>
#include <string>
#include "d3dUtil.h"
#include "Enums.h"
#include "Vector.h"
#include "Arbiter.h"

using namespace Mathematics;

struct Point
{
	Point()	{};
	Point(double ax, double ay)	{ x = ax; y = ay;};
	Point operator-(Point p) const	{
		Point tmp(x - p.x, y - p.y);
		return tmp;
	}

	D3DXVECTOR3 toVec3(void)	{
		D3DXVECTOR3 vec = D3DXVECTOR3(x, y, 0);
		return vec;
	}
	double x;
	double y;
};

class Contact
{
public:
	Contact(){Pn = 0; Pt = 0; Pnb = 0;}

	// The arbiter holds the normal
	float Pn;	// normal impulse
	float Pt;	// tangent impulse
	float Pnb;	// normal impulse for position bias
	float massNormal;
	float massTangent;
	float bias;
	float overlap;
	Vector ra;
	Vector rb;
	Vector	pos;
	FeaturePair feature;
};

// Replaces MTV
class Collision
{
public:
	Collision(){};
	Collision(double d, Vector axis)	{
		pushX = d*axis.x;
		pushY = d*axis.y;
		normal = axis;
	}

	void inverse()	{
		pushX *= -1;
		pushY *= -1;
		normal.x *= -1;
		normal.y *= -1;
	}

	bool	collision;
	double	pushX;
	double	pushY;
	float	overlap;
	Vector normal;
	std::vector<Contact> contactList;
};

/// Defines the geomtry of each RigidBody
class Shape
{
public:
	Shape();

	void addPoint(Vector point);
	void flipHorizontal(void);

	/// Moves the origin
	void move(float dx, float dy)	{mPosition.x += dx; mPosition.y += dy;}
	
	/// Scales by percent, ie scalor = 1.1 scales with 10%
	void scale(float scalor);

	/// Rotation is in radians
	void rotate(float rotation);

	/// Gets called on scaling and rotation, used for broadcast collision detection
	void updateBoundingBox(Vector point);

	void resetBoundingBox();
	void resetRotation();
	
	void setOrigin(Vector position)		{mPosition = position;}
	void setRotationAxis(Vector axis)	{mRotationAxis = axis;}
	void setDragPos(Vector pos)			{mDragPos = pos;}

	int		getHeight();
	int		getWidth();
	Rect	getWorldRect(void);
	Rect	getLocalRect(void)		{return mBoundingBox;}
	Vector	getOrigin()				{return mPosition;}
	Vector	GetPosition()			{return mPosition;}
	Vector	getDragPos()			{return mDragPos;}
	int		getSides()				{return mSides;}
	float	getRotation()			{return mRotation;}

	// Makes it at a lot cleaner than having it private
	std::vector<Vector> pointList;
private:
	Vector	mPosition;
	Vector	mRotationAxis;
	Vector	mDragPos;
	int		mSides;
	Rect	mBoundingBox;
	float	mRotation;	
};	// Class
#endif