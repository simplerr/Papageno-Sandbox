#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <string>
#include "Shape.h"
#include "Vector.h"

using namespace Mathematics;
class Rect;

// :NOTE: Scaling objects doesnt change the inertia tensor!

/// Class used in the physic simulation
class RigidBody
{
public:
	RigidBody(float x, float y, int width, int height);		// Same texture atm
	RigidBody(Rect rect);
	~RigidBody();

	void draw();
	void scale(float scalor)	{mShape->scale(scalor);}
	void rotate(float rotation) {mShape->rotate(rotation);}
	void resetRotation()		{mShape->resetRotation();}
	void move(float dx, float dy) {mShape->move(dx, dy);}
	void move(Vector speed)  {move(speed.x, speed.y);}

	void setTexture(string textureSource);
	void setID(int id)							{mID = id;}
	void setStatic(bool stat)					{mStatic = stat;}
	void setCollidable(bool collides)			{mCollidable = collides;}
	void kill()									{mAlive = false;}

	Shape*		getShape()		{return mShape;}	
	bool		getAlive()		{return mAlive;}
	bool		getStatic()		{return mStatic;}
	int			getID()			{return mID;}
	int			getWidth()		{return mShape->getWidth();}
	int			getHeight()		{return mShape->getHeight();}
	bool		getCollidable()			{return mCollidable;}
	float		getMomentInertia()		{return GetMass()*(getWidth()*getWidth() + getHeight() * getHeight())/12;}
	float		getAngularMomentum()	{return GetInertia()*GetAngularVelocity();}

	// New functions
	void SetPosition(Vector position)			{mPosition = position; mShape->setOrigin(position);}		// :TODO: mShape also needs to update
	void SetMomentum(Vector momentum)			{mMomentum = momentum;}
	//void SetAngularMomentum(float momentum)	{mAngularMomentum = momentum;}
	void SetVelocity(Vector velocity)			{mVelocity = velocity;			mMomentum = velocity * GetMass();}
	void SetVelocity(float x, float y)			{mVelocity = Vector(x, y, 0);	mMomentum = mVelocity * GetMass();}
	void SetAngularVelocity(float velocity)		{mAngularVelocity = velocity;}
	void SetForce(Vector force)					{mForce = force;}
	void SetTorque(Vector torque)				{mTorque = torque.z;}
	void SetTorque(float torque)				{mTorque = torque;}
	void SetMass(float mass)					{mMass = mass; mInertiaTensor = GetMass()*(getWidth()*getWidth() + getHeight() * getHeight()) / 12;}
	void SetSleeping(bool sleep)				{mSleep = sleep;}
	void SetFriction(float friction)			{mFriction = friction;}
	void SetRepeatX(bool b)						{mRepeatX = b;}
	void Rotate(float rotation)					{mShape->rotate(rotation);}
	
	void ApplyForce(Vector force, Vector pos);
	void ApplyGravity(float gravity);

	Vector GetPosition()		{return mShape->getOrigin();}
	Vector GetMomentum()		{return mMomentum;}
	Vector GetForce()			{return mForce;}
	Vector GetVelocity()		{return mVelocity;}
	float  GetRotation()		{return mShape->getRotation();}
	float  GetTorque()			{return mTorque;}
	float  GetAngularVelocity()	{return mAngularVelocity;}
	float  GetMass()			{return mMass;}
	float  GetInvMass()			{return mMass == 0 ? 0 : 1/mMass;}
	float  GetInertia()			{return mInertiaTensor;}
	float  GetInvInertia()		{return mInertiaTensor == 0 ? 0 : 1 / mInertiaTensor;}
	bool   GetSleeping()		{return mSleep;}
	float  GetFriction()		{return mFriction;}

private:
	IDirect3DTexture9 *mTexture;
	Shape	*mShape;
	
	bool	mCollidable;	// Needed? All rigids can collide?
	bool	mAlive;
	bool	mStatic;
	int		mID;

	// Primary physics states
	Vector	mPosition;			// Position of center of mass (CM)
	Vector	mMomentum;			// Momentum of body
	float	mOrientation;

	// Secondary states - calculated from primary states
	Vector	mVelocity;
	Vector	mForce;
	float	mTorque;
	float	mAngularVelocity;

	// Constant States
	float	mMass;
	float	mInertiaTensor;
	float	mInvInertia;
	float	mFriction;
	bool    mSleep;

	// Shouldn't be here
	bool	mRepeatX;
};

#endif