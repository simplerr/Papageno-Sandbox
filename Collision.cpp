#include "World.h"
#include "Shape.h"
#include "Vector.h"
#include "RigidBody.h"

#include <fstream>

bool PointInsideShape(Vector point, Shape* shape);

#define SMALL_DEPTH 0.000f

extern std::ofstream fout;

Collision polyCollision(RigidBody* bodyA, RigidBody* bodyB)
{	
	Shape* ShapeA = bodyA->getShape();
	Shape* ShapeB = bodyB->getShape();

	Collision collision;
	collision.collision = false;

	FeaturePair feature;

	/* broadphase AABB test */
	Rect RectA = ShapeA->getWorldRect();
	Rect RectB = ShapeB->getWorldRect();

	if(RectA.left - SMALL_DEPTH > RectB.right)
		return collision;
	else if(RectA.right + SMALL_DEPTH < RectB.left)
		return collision;
	else if(RectA.top - SMALL_DEPTH > RectB.bottom)
		return collision;			
	else if(RectA.bottom + SMALL_DEPTH < RectB.top)	{
		//fout << "depth: " << RectB.top - RectA.bottom << std::endl;

		return collision;
	}

	bool b = true;
	char buffer[256];
	Vector axis = Vector(0, 0, 0);			// Axis we will project onto
	Vector projection;	// The direction of the projection
	int side;			// Current side 
	double minA, maxA, minB, maxB, axisLen, tmp, minLen = 999999, tmpDepth = 1;;			

	// Test Shape A's sides
	for(side = 0; side < ShapeA->getSides(); side++)
	{
		/* get the axis we will project on */
		/*
			axis = the normal
			n.x = -(pointA.y - pointB.y)
			n.y = pointA.x - pointB.x

			the normal should be pointing OUT from the object
			in order for the object to move by the proper MTV
		*/

		if(side == 0)	// the first point must get connected to the last
		{	
			//axis.x = -(ShapeA->pointList[ShapeA->getSides()-1].y - ShapeA->pointList[0].y);
			//axis.y = ShapeA->pointList[ShapeA->getSides()-1].x - ShapeA->pointList[0].x;
			axis.x = ShapeA->pointList[ShapeA->getSides()-1].y - ShapeA->pointList[0].y;
			axis.y = ShapeA->pointList[0].x - ShapeA->pointList[ShapeA->getSides()-1].x;
		}
		else
		{
			axis.x = ShapeA->pointList[side-1].y - ShapeA->pointList[side].y;
			axis.y = ShapeA->pointList[side].x - ShapeA->pointList[side-1].x;	// should be reversed?:O

			// axis.y gets negative on the bottom horizontal line
			// this is because we want it to point out from the object
			// not sure if it will work with other shapes though
		}

		/* normalize the axis */
		/*
			n.len = sqrt(n.x^2 + n.y^2)
			n.x = n.x / n.len
			n.y = n.y / n.len
		*/

		axisLen = std::sqrt(axis.x * axis.x + axis.y * axis.y);
		axis.x /= axisLen;
		axis.y /= axisLen;

		/* project Shape A's points on the axis to find MIN and MAX points */
		/*
			pointA.x = (pointA.x * axis.x + pointA.y * axis.y) * axis.x = dp * axis.x
			pointA.y = (pointA.x * axis.x + pointA.y * axis.y) * axis.y = dp * axis.y
			
			the dot-product is enough, graphicaly it need adjustment but mathematicly it works
			that gives us:

			pointA.x = pointA.x * axis.x + pointA.y * axis.y = dp
			pointA.y = pointA.x * axis.x + pointA.y * axis.y = dp 		
		*/

		minA = maxA = (ShapeA->pointList[0].x * axis.x + ShapeA->pointList[0].y * axis.y);

		for(int i = 1; i < ShapeA->getSides(); i++)
		{
			tmp = (ShapeA->pointList[i].x * axis.x + ShapeA->pointList[i].y * axis.y);

			if(tmp < minA)
				minA = tmp;
			else if (tmp > maxA)
				maxA = tmp;
		}

		/* correct with offset */
		tmp = ShapeA->getOrigin().x * axis.x + ShapeA->getOrigin().y * axis.y;
		minA += tmp;
		maxA += tmp;

		/* project Shape B's points on the axis to find MIN and MAX points */
		minB = maxB = (ShapeB->pointList[0].x * axis.x + ShapeB->pointList[0].y * axis.y);

		for(int i = 1; i < ShapeB->getSides(); i++)
		{		
			tmp = (ShapeB->pointList[i].x * axis.x + ShapeB->pointList[i].y * axis.y);		

			if(tmp < minB)	{
				minB = tmp;
			}
			else if (tmp > maxB)	{
				maxB = tmp;
			}
		}		

		/* correct with offset */
		tmp = ShapeB->getOrigin().x * axis.x + ShapeB->getOrigin().y * axis.y;
		minB += tmp;
		maxB += tmp;

		/* test if they intersect, if not then store the penetration depth and axis */
		if(minA - SMALL_DEPTH > maxB || maxA + SMALL_DEPTH < minB)	{
			collision.collision = false;
			return collision;
		}
		else
		{	
			// the projection axis is to be used by the player (objectB)			
			if(maxA < maxB)	
			{
				tmpDepth = maxA - minB;
				if(tmpDepth < minLen)	{
					minLen = tmpDepth;
					projection = axis;
				}	
			}
			else if(maxA > maxB)
			{
				tmpDepth = maxB - minA;			
				if(tmpDepth < minLen)	{
					minLen = tmpDepth;
					projection.x = axis.x*-1;	// WHY? - to correct for ShapeB
					projection.y = axis.y*-1;	
				}
			}
		}
	}

	/* the same thing again but now for object B, sometimes a collision from Shape A isn't a collision for Shape B */
	/*
		- see above for detailed comments
	*/

	// test Shape B's sides
	for(side = 0; side < ShapeB->getSides(); side++)
	{
		/* get the axis we will project on */
		if(side == 0)	// the first point must get connected to the last
		{			
			axis.x = ShapeB->pointList[ShapeB->getSides()-1].y - ShapeB->pointList[0].y;
			axis.y = ShapeB->pointList[0].x - ShapeB->pointList[ShapeB->getSides()-1].x;
		}
		else
		{
			axis.x = ShapeB->pointList[side-1].y - ShapeB->pointList[side].y;
			axis.y = ShapeB->pointList[side].x - ShapeB->pointList[side-1].x;	// should be reversed?:O
		}

		/* normalize the axis */	
		axisLen = std::sqrt(axis.x * axis.x + axis.y * axis.y);
		axis.x /= axisLen;
		axis.y /= axisLen;

		/* project Shape B's points on the axis to find MIN and MAX points */		
		minB = maxB = (ShapeB->pointList[0].x * axis.x + ShapeB->pointList[0].y * axis.y);

		for(int i = 1; i < ShapeB->getSides(); i++)
		{
			tmp = (ShapeB->pointList[i].x * axis.x + ShapeB->pointList[i].y * axis.y);			

			if(tmp < minB)
				minB = tmp;
			else if (tmp > maxB)
				maxB = tmp;
		}

		/* correct with offset */
		tmp = ShapeB->getOrigin().x * axis.x + ShapeB->getOrigin().y * axis.y;
		minB += tmp;
		maxB += tmp;

		/* project Shape A's points on the axis to find MIN and MAX points */	
		minA = maxA = (ShapeA->pointList[0].x * axis.x + ShapeA->pointList[0].y * axis.y);

		for(int i = 1; i < ShapeA->getSides(); i++)
		{	
			tmp = (ShapeA->pointList[i].x * axis.x + ShapeA->pointList[i].y * axis.y);	// - gets removed??

			if(tmp < minA)	{
				minA = tmp;
			}
			else if (tmp > maxA)	{
				maxA = tmp;
			}
		}

		/* correct with offset */
		tmp = ShapeA->getOrigin().x * axis.x + ShapeA->getOrigin().y * axis.y;
		minA += tmp;
		maxA += tmp;

		/* test if they intersect, if not then return false */
		if(minA - SMALL_DEPTH > maxB){// || maxA < minB)	{
			collision.collision = false;
			return collision;
		}
		else
		{	
			// get the minimal depth of intersection and the correct axis
			if(maxA < maxB)	
				tmpDepth = maxA - minB;				
				
			if(tmpDepth < minLen)	{				
				minLen = tmpDepth;
				projection = axis;	
			}
		}
	}

	// Set collision information
	collision.pushX  = minLen * projection.x;	// shouldn't be needed
	collision.pushY  = minLen * projection.y;
	collision.normal = projection;
	collision.overlap = minLen;
	collision.collision = true;

	Vector collisionPoint = Vector(0, 0, 0);

	/* find out which points that's inside the other object */
	for(int i = 0; i < ShapeA->pointList.size(); i++)
	{
		Contact contact;
		contact.pos = ShapeA->pointList[i];
		contact.pos.x += ShapeA->getOrigin().x;
		contact.pos.y += ShapeA->getOrigin().y;
		//contact.pos -= minLen * projection*-1;

		if(PointInsideShape(contact.pos, ShapeB))	{
			contact.feature.i.bodyA = false;
			contact.feature.i.point = i;
			collision.contactList.push_back(contact);
		}
	}
	// Test ShapeB's points
	for(int i = 0; i < ShapeB->pointList.size(); i++)
	{
		Contact contact;
		contact.pos = ShapeB->pointList[i];
		contact.pos.x += ShapeB->getOrigin().x;
		contact.pos.y += ShapeB->getOrigin().y;
		//contact.pos -= minLen * projection;

		if(PointInsideShape(contact.pos, ShapeA))	{
			contact.feature.i.bodyA = true;
			contact.feature.i.point = i;
			collision.contactList.push_back(contact);
		}
	}

	/* return minimum intersection depth */
	return collision;
}

bool PointInsideShape(Vector point, Shape* shape)
{
	bool b = true;
	char buffer[256];
	Vector axis = Vector(0, 0, 0);			// Axis we will project onto
	Vector projection;	// The direction of the projection
	int side;			// Current side 
	double minA, maxA, minB, maxB, axisLen, tmp, minLen = 999999, tmpDepth = 1;	

	for(side = 0; side < shape->getSides(); side++)
	{
		if(side == 0)	// the first point must get connected to the last
		{	
			axis.x = shape->pointList[shape->getSides()-1].y - shape->pointList[0].y;
			axis.y = shape->pointList[0].x - shape->pointList[shape->getSides()-1].x;
		}
		else
		{
			axis.x = shape->pointList[side-1].y - shape->pointList[side].y;
			axis.y = shape->pointList[side].x - shape->pointList[side-1].x;	// should be reversed?:O
		}

		axisLen = std::sqrt(axis.x * axis.x + axis.y * axis.y);
		axis.x /= axisLen;
		axis.y /= axisLen;

		minA = maxA = (shape->pointList[0].x * axis.x + shape->pointList[0].y * axis.y);

		for(int i = 1; i < shape->getSides(); i++)
		{
			tmp = (shape->pointList[i].x * axis.x + shape->pointList[i].y * axis.y);

			if(tmp < minA)
				minA = tmp;
			else if (tmp > maxA)
				maxA = tmp;
		}

		/* correct with offset */
		tmp = shape->getOrigin().x * axis.x + shape->getOrigin().y * axis.y;
		minA += tmp;
		maxA += tmp;

		/* project Shape B's points on the axis to find MIN and MAX points */
		minB = maxB = point.x * axis.x + point.y * axis.y;
	
		tmp = (point.x * axis.x + point.y * axis.y);		

		if(tmp < minB)
			minB = tmp;
		else if (tmp > maxB)
			maxB = tmp;	

		/* test if they intersect, if not then store the penetration depth and axis */
		if(minA - SMALL_DEPTH > maxB || maxA + SMALL_DEPTH < minB)	{
			return false;
		}
	}

	return true;
}