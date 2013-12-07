/// Description: An AppleRoom is a predictable place full of apples.  
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-29

#include "AppleRoom.h"
#include "Food.h"
#include <iostream>
using std::string;
using std::cout;
using std::endl;

namespace GameLogic
{
	// Destructor.
	AppleRoom::~AppleRoom()
	{
		// TODO: Add stuff here if needed
	}

	// Process event that may occur upon entering environment.
	void AppleRoom::onEntry(Character& character)
	{
		if(character.isControllable())
		{
			cout << character.getName() << " entered a room smelling of... APPLES!" << endl;
		}
	}

	// Process event that may occur when leaving environment.
	void AppleRoom::onLeave(Character& character)
	{
		if(character.isControllable())
		{
			cout << character.getName() << " left the apple room." << endl;
		}
	}

	// Process event that may occur while moving in room.
	void AppleRoom::turnEvent(Character& character)
	{
		// Time to add a new apple! :D
		if(getConsumable("Apple") == nullptr)
		{
			addConsumable(new Food("Apple"));
			if(character.isControllable())
			{
				cout << "Another apple appeared in the apple room." << endl;
			}	
		}
	}
}