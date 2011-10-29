#include "Arbiter.h"
#include "RigidBody.h"
#include "Shape.h"
#include "Vector.h"
#include "RigidBody.h"
#include <fstream>

using namespace std;

Collision polyCollision(RigidBody* bodyA, RigidBody* bodyB);

extern std::ofstream fout;

Arbiter::Arbiter(RigidBody* a, RigidBody* b)
{
	if (a < b)
	{
		bodyA = a;
		bodyB = b;
	}
	else
	{
		bodyA = b;
		bodyB = a;
	}

	// Check collision between bodies
	Collision collision = polyCollision(bodyA, bodyB);

	friction = sqrtf(a->GetFriction() * b->GetFriction());

	// Transfer the collision points to the contact list
	normal = collision.normal;
	for(int i = 0; i < collision.contactList.size(); i++)	{
		Contact* contact = new Contact;	// :WARNING: mem leak!
		contact->Pn = 0.0f;
		*contact = collision.contactList[i];
		contact->overlap = collision.overlap;
		contactList.push_back(contact);	// :WARNING: could cause ptr error!
	}

	// :HACK: This gets set to false when two bodies no longer intersect
	active = true;
}

Arbiter::~Arbiter()
{

}

void Arbiter::Update(std::vector<Contact*> newContacts)
{
	std::vector<Contact*> tmpContacts;

	//contactList.clear();
	for(int i = 0; i < newContacts.size(); i++)
	{
		int k = -1;

		// Loop through to see if it's an old contact
		for(int j = 0; j < contactList.size(); j++)
		{
			if(contactList[j]->feature.value == newContacts[i]->feature.value)
			{
				k = j;
				break;
			}
		}

		// Old contact
		if(k > -1 && 1 == 0)	// :NOTE: might work better with only new contact!
		{
			//if(i == 3)
			//	fout << "Pn: " << contactList[k]->Pn << endl;

			Contact* c = newContacts[i];
			c->Pn = contactList[k]->Pn;
			c->Pnb = contactList[k]->Pnb;
			c->Pt = contactList[k]->Pt;

			tmpContacts.push_back(c);

		}
		else // New contact
		{
			newContacts[i]->Pn = 0.0f;
			newContacts[i]->Pt = 0.0f;
			newContacts[i]->Pnb = 0.0f;
			tmpContacts.push_back(newContacts[i]);
		}
	}

	contactList.clear();
	contactList = tmpContacts;
}

void Arbiter::PreStep(float inv_dt)
{
	const float k_allowedPenetration = 0.4f;
	// :TODO: Add position correction test
	float k_biasFactor = 0.2f;

	for(int i = 0; i < contactList.size(); i++)
	{
		/* Precompute normal mass, tangent mass, and bias. */

		// Normal mass
		Vector rap = contactList[i]->pos - bodyA->GetPosition();
		Vector rbp = contactList[i]->pos - bodyB->GetPosition();

		Vector tmp1 = rap.cross(normal);
		Vector tmp2 = rbp.cross(normal);

		/*
			kNormal = 1⁄ma + 1⁄mb + (rap × n)2 ⁄ Ia + (rbp × n)2 ⁄ Ib
		*/

		float kNormal = bodyA->GetInvMass() + bodyB->GetInvMass();
		kNormal += tmp1.dot(tmp1) * bodyA->GetInvInertia() + tmp2.dot(tmp2) * bodyB->GetInvInertia();
		contactList[i]->massNormal = 1 / kNormal;

		// Tangent mass (friction)
		Vector tangent = normal.cross(1);
		tmp1 = rap.cross(tangent);
		tmp2 = rbp.cross(tangent);

		float kTangent = bodyA->GetInvMass() + bodyB->GetInvMass();
		kTangent += tmp1.dot(tmp1) * bodyA->GetInvInertia() + tmp2.dot(tmp2) * bodyB->GetInvInertia();
		contactList[i]->massTangent = 1 / kTangent;

		contactList[i]->bias = k_biasFactor * inv_dt * maximum(0, contactList[i]->overlap - k_allowedPenetration);

		// Apply impulse
		Vector P = contactList[i]->Pn * normal;// + contactList[i]->Pt * tangent;

		/*bodyA->SetVelocity(bodyA->GetVelocity() - (bodyA->GetInvMass()) * P);
		bodyA->SetAngularVelocity(bodyA->GetAngularVelocity() - (bodyA->GetInvInertia()) * contactList[i]->ra.cross(P).z);	// Note the .z, x and y is 0

		bodyB->SetVelocity(bodyB->GetVelocity() + (bodyB->GetInvMass()) * P);
		bodyB->SetAngularVelocity(bodyB->GetAngularVelocity() + (bodyB->GetInvInertia()) * contactList[i]->rb.cross(P).z);*/
	}
}
	
void Arbiter::ApplyImpulse()
{
	for(int i = 0; i < contactList.size(); i++)
	{
		Contact* c = contactList[i];
		c->ra = c->pos - bodyA->GetPosition();
		c->rb = c->pos - bodyB->GetPosition();

		// Relative velocity at contact point
		Vector vap = bodyA->GetVelocity() + c->ra.cross(bodyA->GetAngularVelocity());
		Vector vbp = bodyB->GetVelocity() + c->rb.cross(bodyB->GetAngularVelocity());

		float dv = normal.dot(vap - vbp);

		float dPn = c->massNormal * (dv + c->bias);	//:TODO: Add bias velocity

		float Pn0 = c->Pn;
		c->Pn = max(Pn0 + dPn, 0);
		dPn = c->Pn - Pn0;

		impulse = dPn;

		//fout << "impulse: " << impulse << endl;

		// Apply contact impulse
		Vector Pn = dPn * normal;

		bodyA->SetVelocity(bodyA->GetVelocity() - (bodyA->GetInvMass()) * Pn);
		bodyA->SetAngularVelocity(bodyA->GetAngularVelocity() - (bodyA->GetInvInertia()) * c->ra.cross(Pn).z);	// Note the .z, x and y is 0

		bodyB->SetVelocity(bodyB->GetVelocity() + (bodyB->GetInvMass()) * Pn);
		bodyB->SetAngularVelocity(bodyB->GetAngularVelocity() + (bodyB->GetInvInertia()) * c->rb.cross(Pn).z);

		/* Friction impulse */

		// Relative velocity at contact point
		vap = bodyA->GetVelocity() + Vector(0, 0, bodyA->GetAngularVelocity()).cross(c->ra);
		vbp = bodyB->GetVelocity() + Vector(0, 0, bodyB->GetAngularVelocity()).cross(c->rb);

		Vector tangent = normal.cross(1);
		dv = tangent.dot(vap - vbp) *-1;	// :NOTE: *-1 dunno why
		float dPt = c->massTangent * (-dv);

		// Clamp friction impulse
		float maxPt = friction * dPn;
		dPt = clamp(dPt, -maxPt, maxPt);

		/*float oldTangentImpulse = c->Pt;
		c->Pt = clamp(oldTangentImpulse + dPt, -maxPt, maxPt);
		dPt = c->Pt - oldTangentImpulse;*/

		// Apply contact impulse
		Vector Pt = dPt * tangent;

		bodyA->SetVelocity(bodyA->GetVelocity() - (bodyA->GetInvMass()) * Pt);
		bodyA->SetAngularVelocity(bodyA->GetAngularVelocity() - (bodyA->GetInvInertia()) * c->ra.cross(Pt).z);	// Note the .z, x and y is 0

		bodyB->SetVelocity(bodyB->GetVelocity() + (bodyB->GetInvMass()) * Pt);
		bodyB->SetAngularVelocity(bodyB->GetAngularVelocity() + (bodyB->GetInvInertia()) * c->rb.cross(Pt).z);
	}
}