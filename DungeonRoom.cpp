/// Description: A DungeonRoom is a dark hallow place full with rats and skeletons.  
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-09

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
		// TODO: Add onEntry event (if this is what one wants)
	}

	// Process event that may occur when leaving environment.
	void DungeonRoom::onLeave(Character& character)
	{
		// TODO: Add onLeave event (if this is what one wants)
	}

	// Process event that may occur while moving in room.
	void DungeonRoom::turnEvent(Character& character)
	{
		// TODO: Add turnEvent event (if this is what one wants)
	}
}