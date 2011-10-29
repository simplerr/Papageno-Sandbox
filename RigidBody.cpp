#include "RigidBody.h"
#include "Graphics.h"
#include "enums.h"

RigidBody::RigidBody(float x, float y, int width, int height)
{
	// Create a Rectangle shape - origin middle
	mShape = new Shape();
	mShape->setOrigin(Vector(x, y, 0));
	mShape->addPoint(Vector(-width/2, -height/2, 0));	// top - left
	mShape->addPoint(Vector(-width/2, height/2, 0));	// bottom - left
	mShape->addPoint(Vector(width/2, height/2, 0));   // bottom - right
	mShape->addPoint(Vector(width/2, -height/2, 0));	// top - right

	setStatic(false);
	mAlive = true;

	SetMass(1);
	SetVelocity(0, 0);
	SetAngularVelocity(0.0f);
	SetForce(Vector(0, 0, 0));
	SetTorque(Vector(0, 0, 0));
	SetMomentum(Vector(0, 0, 0));
	SetFriction(1.0f);
	SetSleeping(false);
	SetRepeatX(false);
}

RigidBody::RigidBody(Rect rect)
{
	float width = (rect.right - rect.left);
	float height = (rect.bottom - rect.top);

	// Create a Rectangle shape - origin middle
	mShape = new Shape();
	mShape->setOrigin(Vector(rect.left + width/2, rect.top + height/2, 0));
	mShape->addPoint(Vector(-width/2, -height/2, 0));	// top - left
	mShape->addPoint(Vector(-width/2, height/2, 0));	// bottom - left
	mShape->addPoint(Vector(width/2, height/2, 0));   // bottom - right
	mShape->addPoint(Vector(width/2, -height/2, 0));	// top - right

	setStatic(false);
	mAlive = true;

	SetMass(1);
	SetVelocity(0, 0);
	SetAngularVelocity(0.0f);
	SetForce(Vector(0, 0, 0));
	SetTorque(Vector(0, 0, 0));
	SetMomentum(Vector(0, 0, 0));
	SetFriction(1.0f);
	SetSleeping(false);
}
	
RigidBody::~RigidBody()
{
	// Dtor
	delete mShape;
	ReleaseCOM(mTexture);
}

void RigidBody::setTexture(string textureSource)
{
	mTexture = gGraphics->loadTexture(textureSource);
}

void RigidBody::draw()
{
	if(mRepeatX)
	{
		// scale it in the x direction
		D3DXMATRIX texScaling;
		D3DXMatrixScaling(&texScaling, getWidth()/256, 1.0f, 0.0f);
		HR(gd3dDevice->SetTransform(D3DTS_TEXTURE0, &texScaling));

		gGraphics->drawTexturedShape(*getShape(), mTexture);

		// restore to std scaling
		D3DXMatrixScaling(&texScaling, 1.0f, 1.0f, 0.0f);
		HR(gd3dDevice->SetTransform(D3DTS_TEXTURE0, &texScaling));
	}
	else
		gGraphics->drawTexturedShape(*getShape(), mTexture);
}

void RigidBody::ApplyForce(Vector force, Vector pos)
{
	SetForce(GetForce() + force);

	// Torque = r x F
	//SetTorque(force.cross(pos));
	SetTorque((pos - GetPosition()).cross(force));
}

void RigidBody::ApplyGravity(float m)
{

}