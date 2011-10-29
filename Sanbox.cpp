#include "Sandbox.h"
#include "Graphics.h"
#include <math.h>
#include "DirectInput.h"
#include "Shape.h"
#include "RigidBody.h"
#include "World.h"
#include <time.h>

bool PointInsideShape(Vector point, Shape* shape);

Sandbox::Sandbox()
{
	mWorld = new World();
	mWorld->Init();

	// :HACK: Should scale texture instead
	RigidBody* floor = new RigidBody(200, 770, 400, 200);
	floor->setTexture("imgs\\dirt_grass.bmp");
	floor->SetMass(0);
	floor->setStatic(true);
	mWorld->Add(floor);

	RigidBody* floor2 = new RigidBody(600, 770, 400, 200);
	floor2->setTexture("imgs\\dirt_grass.bmp");
	floor2->SetMass(0);
	floor2->setStatic(true);
	mWorld->Add(floor2);

	RigidBody* floor3 = new RigidBody(1000, 770, 400, 200);
	floor3->setTexture("imgs\\dirt_grass.bmp");
	floor3->SetMass(0);
	floor3->setStatic(true);
	mWorld->Add(floor3);

	mMouseBox.connect(&Sandbox::mouseBox, this);

	mActiveBody = NULL;
	mRotationCircle = NULL;

	mLastRotation = 0;

	mRotationTexture = gGraphics->loadTexture("imgs\\rotation_circle.bmp");
	mBkgdTexture = gGraphics->loadTexture("imgs\\plain_bkgd.bmp");

	mRotating = false;
	SCALE_FACTOR = 0.06;

	srand(time(NULL));
}
	
Sandbox::~Sandbox()
{
	delete mWorld;
	delete mRotationCircle;
	ReleaseCOM(mRotationTexture);
	ReleaseCOM(mBkgdTexture);
}

void Sandbox::update(float dt)
{
	mWorld->Update(dt);
	mMouseBox.update(dt);

	stateChange();

	if(mWorld->GetPaused())			// The paused state of the sandbox
		pausedUpdate(dt);
	else if(!mWorld->GetPaused())	// The dynamic state of the sandbox
		dynamicUpdate(dt);

	if(gDInput->keyPressed(DIK_ESCAPE))
		callback(MENU);
}

void Sandbox::pausedUpdate(float dt)
{
	// Delete bodies with Ctrl - press
	if(gDInput->mouseButtonPressed(0) && gDInput->keyDown(DIK_LCONTROL))
	{
		RigidBody* deleteObject = mWorld->GetBody(gDInput->getCursorPos());
		
		if(deleteObject != NULL)
		{
			mWorld->Remove(deleteObject);
		}
	}
	else if(gDInput->mouseButtonPressed(0))
	{
		// Get which object was pressed
		if(mActiveBody != NULL)
		{
			if(!mRotationCircle->pointInside(gDInput->getCursorPos()))	// Not if the rotation box was pressed
				mActiveBody = mWorld->GetBody(gDInput->getCursorPos());
		}
		else
			mActiveBody = mWorld->GetBody(gDInput->getCursorPos());
		
		// Create and setup the rotation circle
		if(mActiveBody != NULL)	
		{
			// :TODO: Make the rotation circle work even when pressed outside the object!
			mLastRotation = atan2f(mActiveBody->GetPosition().y - gDInput->getCursorPos().y, mActiveBody->GetPosition().x - gDInput->getCursorPos().x);

			// Delete the old one
			delete mRotationCircle;

			// Create the rotation circle
			mRotationCircle = new HollowCircle(mActiveBody->GetPosition().x, mActiveBody->GetPosition().y, 60, 30, "imgs\\rotation_circle.bmp");
			mRotationCircle->setRotation(mActiveBody->GetRotation());
			mRotating = mRotationCircle->pointInside(gDInput->getCursorPos());
		}
	}

	// Update the active body
	if(mActiveBody != NULL)
	{
		if(gDInput->mouseButtonDown(0))
		{
			if(mRotating) // If rotation area
			{	
				// Rotate the active body
				float rotation = atan2f(mActiveBody->GetPosition().y - gDInput->getCursorPos().y, mActiveBody->GetPosition().x - gDInput->getCursorPos().x);
				mActiveBody->Rotate(rotation - mLastRotation);
				mLastRotation = rotation;

				// Rotate the rotation box
				mRotationCircle->setRotation(mActiveBody->GetRotation());
			}
			else // If move area
			{
				mActiveBody->move(gDInput->mouseDX(), gDInput->mouseDY());
				mRotationCircle->move(gDInput->mouseDX(), gDInput->mouseDY());
			}

			// When a object gets moved or rotated it's speed gets resetted
			mActiveBody->SetVelocity(0, 0);
			mActiveBody->SetAngularVelocity(0);
		}

		if(gDInput->mouseDZ() != 0)
		{
			if(gDInput->mouseDZ() > 0)
				mActiveBody->scale(1.0f + SCALE_FACTOR);
			if(gDInput->mouseDZ() < 0)
				mActiveBody->scale(1.0f - SCALE_FACTOR);
		}
	}
}
	
void Sandbox::dynamicUpdate(float dt)
{
	// Delete bodies with Ctrl - press
	if(gDInput->mouseButtonPressed(0) && gDInput->keyDown(DIK_LCONTROL))
	{
		RigidBody* deleteObject = mWorld->GetBody(gDInput->getCursorPos());
		
		if(deleteObject != NULL)
		{
			mWorld->Remove(deleteObject);
		}
	}
	else if(gDInput->mouseButtonPressed(0))
	{
		mActiveBody = mWorld->GetBody(gDInput->getCursorPos());
		
		if(mActiveBody != NULL)
			mActiveBody->getShape()->setDragPos(gDInput->getCursorPos() - mActiveBody->GetPosition());
	}

	if(gDInput->mouseButtonReleased(0))
	{
		mActiveBody = NULL;
	}

	if(gDInput->mouseButtonDown(0) && mActiveBody != NULL)
	{
		mActiveBody->ApplyForce((gDInput->getCursorPos() - (mActiveBody->GetPosition() + mActiveBody->getShape()->getDragPos()))/1.5, gDInput->getCursorPos());
	}
}

void Sandbox::stateChange()
{
	if(gDInput->keyReleased(DIK_SPACE))	// Change state
	{
		if(mWorld->GetPaused())	{
			mActiveBody = NULL;
			mWorld->Unpause();
		}
		else if(!mWorld->GetPaused())	{
			mActiveBody = NULL;
			mWorld->Pause();
		}
	}
}

void Sandbox::draw()
{
	// Background
	gGraphics->drawGlowTexture(mBkgdTexture, 600, 400, 1200, 800, 1000, Vector(600, 400));

	// Objects in World
	mWorld->Draw();
	mMouseBox.draw();

	if(mActiveBody != NULL)	
	{
		if(!mWorld->GetPaused())
			gGraphics->drawLine(gDInput->getCursorPos(), mActiveBody->GetPosition() + mActiveBody->getShape()->getDragPos());
		else	{
			mRotationCircle->draw();
		}
	}
}

void Sandbox::mouseBox()
{
	RigidBody* body = new RigidBody(mMouseBox.getBox());
	int area = body->getWidth() * body->getHeight();
	if(area > 400)
	{
		float mass = body->getWidth() * body->getHeight() * 0.005;

		if(mass < 50)
			mass = 50;
		else if(mass < 100)
			mass = 100;
		else if(mass < 200)
			mass = 200;
		else if(mass < 400)
			mass = 400;
		else
			mass = 600;

		body->SetMass(mass);

		int random =  rand() % 2;
		if(random == 1)
			body->setTexture("imgs\\box1.bmp");
		else if(random == 0)
			body->setTexture("imgs\\box2.bmp");

		mWorld->Add(body);
	}
	else
		delete body;

	mActiveBody = NULL;
}

HollowCircle::HollowCircle(float x, float y, int radius, int innerRadius, string textureSource)
{
	mTexture = gGraphics->loadTexture(textureSource);
	mPos = Vector(x, y);
	mRadius = radius;
	mInnerRadius = innerRadius;
	mRotation = 0;
}
	
HollowCircle::~HollowCircle()
{

}

void HollowCircle::draw()
{
	gGraphics->drawHollowCircle(mTexture, mPos.x, mPos.y, mRadius, mInnerRadius, mRotation);
}

void HollowCircle::move(float dx, float dy)
{
	mPos.x += dx;
	mPos.y += dy;
}

void HollowCircle::setRotation(float rotation)
{
	mRotation = rotation;
}

bool HollowCircle::pointInside(Vector point)
{
	float dx = mPos.x - point.x;
	float dy = mPos.y - point.y;
	float dist = Mathematics::sqrt(dx*dx + dy*dy);

	if(dist < mRadius && dist > mInnerRadius)
		return true;
	else
		return false;
}