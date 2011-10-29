#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Enums.h"
#include <boost\function.hpp>
#include <boost\bind.hpp>

class GameState
{
public:
	GameState() {};
	~GameState() {};

	virtual void update(float dt) = 0;
	virtual void draw() = 0;

	boost::function<void(State newState)> callback;

	template <class T>
	void connect(void(T::*_callback)(State newState), T* _object)	{
		callback = boost::bind(_callback, _object, _1);
	}
};

#endif