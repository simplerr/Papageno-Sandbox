#include "Shape.h"

Shape::Shape()
{
	mSides = 0;
	mBoundingBox.left = 0;
	mBoundingBox.right = 0;
	mBoundingBox.bottom = 0;
	mBoundingBox.top = 0;
	mRotation = 0;
	mDragPos = Vector(0, 0);

	// Rotate around origin as standard
	mRotationAxis = Vector(0, 0, 0);
}

// The order in which the points gets added is important
void Shape::addPoint(Vector point)
{
	// Calculate the bounding box
	if(point.x < getLocalRect().left)
		mBoundingBox.left = point.x;
	else if(point.x > getLocalRect().right)
		mBoundingBox.right = point.x;

	if(point.y < getLocalRect().top)
		mBoundingBox.top = point.y;
	else if(point.y > getLocalRect().bottom)
		mBoundingBox.bottom = point.y;

	mSides++;

	pointList.push_back(point);
}

int Shape::getWidth()
{
	return mBoundingBox.right - mBoundingBox.left;
}

int	Shape::getHeight()
{
	return mBoundingBox.bottom - mBoundingBox.top;
}


Rect Shape::getWorldRect(void)	
{
	Rect r = mBoundingBox;

	r.left += getOrigin().x;
	r.right += getOrigin().x;
	r.top += getOrigin().y;
	r.bottom += getOrigin().y;

	return r;
}

void Shape::rotate(float rotation)
{
	// Rotate the shape with D3DXVec4Transform
	D3DXMATRIX rotationMatrix;
	D3DXVECTOR4 vector;
	vector.z = 0;
	vector.w = 1;
	D3DXMatrixRotationZ(&rotationMatrix, rotation);

	// So the bounding box will get updated properly
	resetBoundingBox();

	// For each vertex
	for(int i = 0; i < pointList.size(); i++)	
	{
		vector.x = pointList[i].x - mRotationAxis.x;	// Specifies axis to rotate around (local coordinates)
		vector.y = pointList[i].y - mRotationAxis.y;	// Specifies axis to rotate around (local coordinates)

		D3DXVec4Transform(&vector,  &vector, &rotationMatrix);

		pointList[i].x = vector.x + mRotationAxis.x;
		pointList[i].y = vector.y + mRotationAxis.y;

		// Update the bounding box
		updateBoundingBox(pointList[i]);
	}

	// For the drag pos
	vector.x = mDragPos.x - mRotationAxis.x;
	vector.y = mDragPos.y - mRotationAxis.y;

	D3DXVec4Transform(&vector,  &vector, &rotationMatrix);

	mDragPos.x = vector.x + mRotationAxis.x;
	mDragPos.y = vector.y + mRotationAxis.y;

	mRotation += rotation;
	if(mRotation > 2*PI)
		mRotation = rotation;

	// Calculate new order of points
	/*for(int i = 0; i < pointList.size(); i++)
	{
		for(int j = 0; j < pointList.size(); j++)
		{
			if(pointList[j].x < pointList[i].x && pointList[j].y < pointList[i].y)
				pointList[i] = pointList[j];
		}
	}*/
}

void Shape::resetRotation()
{
	rotate(-mRotation);
}

void Shape::updateBoundingBox(Vector point)
{
	if(point.x < getLocalRect().left)
		mBoundingBox.left = point.x;
	else if(point.x > getLocalRect().right)
		mBoundingBox.right =point.x;

	if(point.y < getLocalRect().top)
		mBoundingBox.top = point.y;
	else if(point.y > getLocalRect().bottom)
		mBoundingBox.bottom = point.y;
}

void Shape::resetBoundingBox()
{
	mBoundingBox.bottom = 0;
	mBoundingBox.top = 0;
	mBoundingBox.left = 0;
	mBoundingBox.right = 0;
}

void Shape::scale(float scalor)
{
	resetBoundingBox();
	for(int i = 0; i < pointList.size(); i++)	{
		pointList[i].x *= scalor;
		pointList[i].y *= scalor;
		updateBoundingBox(pointList[i]);
	}
}

void Shape::flipHorizontal(void)
{
	/*for(int i = 0; i < pointList.size();i++)
	{
		// right of middle
		if(pointList[i].x > aabb.right/2)	
			pointList[i].x = pointList[i].x - 2*(pointList[i].x - (aabb.right/2));
		else if(pointList[i].x < aabb.right/2)
			pointList[i].x = pointList[i].x + 2*((aabb.right/2) - pointList[i].x);
	}*/
}