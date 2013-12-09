/// Description: When entering a GoalRoom victory is assured.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-08

#include "GoalRoom.h"
#include "GameEngine.h"
#include <iostream>
using std::string;
using std::cout;
using std::endl;

namespace GameLogic
{
	// Destructor.
	GoalRoom::~GoalRoom()
	{
		// TODO: Add stuff here if needed
	}

	// Process event that may occur upon entering environment.
	void GoalRoom::onEntry(Character& character)
	{
		if(character.isControllable())
		{
			GameEngine::winGame();
		}
	}

	// Process event that may occur when leaving environment.
	void GoalRoom::onLeave(Character& character)
	{
		// This can never happen
	}

	// Process event that may occur while moving in room.
	void GoalRoom::turnEvent(Character& character)
	{
		// This can never happen
	}
}