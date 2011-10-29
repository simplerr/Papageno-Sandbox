#ifndef MOUSEBOX_H
#define MOUSEBOX_H

#include "DirectInput.h"
#include "Enums.h"
#include "Vector.h"
#include <boost\function.hpp>
#include <boost\bind.hpp>

class Rect;

class MouseBox
{
public:
	MouseBox();
	~MouseBox();

	void update(float dt);
	void draw();
	Rect getBox()	{
		return mBox;
	}

	// Callback that runs when the users releases the mouse
	boost::function<void(void)> callback;

	template <class T>
	void connect(void(T::*_callback)(void), T* _object)	{	// hack?
		callback = boost::bind(_callback, _object);
	}
private:
	Rect mBox;
	Vector mCorner;
	bool mBoxingActive;
};

#endif