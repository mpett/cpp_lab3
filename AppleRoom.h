/// Description: Header file for AppleRoom class.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-29
#pragma once
#include "Environment.h"
#include "Character.h"
#include <string>

namespace GameLogic
{
	class AppleRoom : public Environment
	{
	public:
		// Destructor.
		~AppleRoom();

		// Constructor.
		AppleRoom(std::string description) : Environment(description){}

		// Process event that may occur upon entering environment.
		void onEntry(Character& character);

		// Process event that may occur when leaving environment.
		void onLeave(Character& character);

		// Process event that may occur while moving in room.
		void turnEvent(Character& character);
	};
}