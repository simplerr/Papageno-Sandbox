#ifndef ARBITER_H
#define ARBITER_H

#include <vector>
#include "Vector.h"

class RigidBody;
class Contact;

/* 
	An arbiter contains contact information about two objects
	It's Arbiters job to store each contact point
	Arbiters constructor calls a function that calculates collisions
	It's also the arbiter that apply impulses to bodies
*/

union FeaturePair
{
	struct Identifier
	{
		bool bodyA;
		int point;
	} i;

	int value;
};

class ArbiterKey
{
public:
	ArbiterKey(RigidBody* b1, RigidBody* b2)
	{
		if (b1 < b2)
		{
			body1 = b1; body2 = b2;
		}
		else
		{
			body1 = b2; body2 = b1;
		}
	}

	RigidBody* body1;
	RigidBody* body2;
};

class Arbiter
{
public:
	Arbiter(RigidBody* a, RigidBody* b);
	~Arbiter();

	void Update(std::vector<Contact*> newContacts);
	void PreStep(float inv_dt);
	void ApplyImpulse();

	std::vector<Contact*> contactList;

	RigidBody *bodyA;
	RigidBody *bodyB;

	Vector normal;
	float friction;
	float impulse;
	bool active;
};

// Don't understand why this fixes the < error
// This is used by std::set
inline bool operator < (const ArbiterKey& a1, const ArbiterKey& a2)
{
	if (a1.body1 < a2.body1)
		return true;

	if (a1.body1 == a2.body1 && a1.body2 < a2.body2)
		return true;

	return false;
}

#endif