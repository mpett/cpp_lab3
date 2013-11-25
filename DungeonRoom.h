/// Description: Header file for DungeonRoom class.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-20
#pragma once
#include "Environment.h"
#include "Character.h"
#include <string>

namespace GameLogic
{
	class DungeonRoom : public Environment
	{
	public:
		// Destructor.
		~DungeonRoom();

		// Constructor.
		DungeonRoom(std::string description) : Environment(description){}

		// Process event that may occur upon entering environment.
		void onEntry(Character& character);

		// Process event that may occur when leaving environment.
		void onLeave(Character& character);

		// Process event that may occur while moving in room.
		void turnEvent(Character& character);

		// Assigns left-hand Environment to right-hand Environment.
		virtual DungeonRoom& operator=(const DungeonRoom& env);
	};
}