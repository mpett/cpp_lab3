/// Description: A DungeonRoom is a dark hallow place full with rats and skeletons.  
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-24

#include "DungeonRoom.h"
#include <iostream>
using std::string;
using std::cout;
using std::endl;

namespace GameLogic
{
	// Destructor.
	DungeonRoom::~DungeonRoom()
	{
		// TODO: Add stuff here if needed
	}

	// Process event that may occur upon entering environment.
	void DungeonRoom::onEntry(Character& character)
	{
		if(character.isControllable())
		{
			cout << character.getName() << " entered dungeon room." << endl;
		}
	}

	// Process event that may occur when leaving environment.
	void DungeonRoom::onLeave(Character& character)
	{
		if(character.isControllable())
		{
			cout << character.getName() << " left dungeon room." << endl;
		}
	}

	// Process event that may occur while moving in room.
	void DungeonRoom::turnEvent(Character& character)
	{
		if(character.isControllable())
		{
			cout << "New turn" << endl;
		}
	}

	// Assigns left-hand Environment to right-hand Environment.
	DungeonRoom& DungeonRoom::operator=(const DungeonRoom& env)
	{
		return *this;
	}
}