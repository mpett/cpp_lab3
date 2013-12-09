/// Description: Header file for GoalRoom class.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-08
#pragma once
#include "Environment.h"
#include "Character.h"
#include <string>

namespace GameLogic
{
	class GoalRoom : public Environment
	{
	public:
		// Destructor.
		~GoalRoom();

		// Constructor.
		GoalRoom(std::string description) : Environment(description, "Goal Room"){}

		// Process event that may occur upon entering environment.
		void onEntry(Character& character);

		// Process event that may occur when leaving environment.
		void onLeave(Character& character);

		// Process event that may occur while moving in room.
		void turnEvent(Character& character);
	};
}