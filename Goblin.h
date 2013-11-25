/// Description: Header file for Goblin class, inherits from Character class.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-24
#pragma once
#include <string>
#include "Character.h"

namespace GameLogic
{
	static const double GOBLIN_MIN_WEIGHT = 5;
	static const double GOBLIN_MAX_WEIGHT = 25;

	class Goblin : public Character
	{
	public:
		// Destructor
		~Goblin();

		 // Constructor for character. Set every variable.
		 Goblin(bool controllable, std::string name, std::string type, double weight, int strength, int currentHealth, int maxHealth, int minDamage, int maxDamage, int currentCarried, int maxCarried, Environment* room)
			 : Character(controllable, name, type, weight, strength, currentHealth, maxHealth, minDamage, maxDamage, currentCarried, maxCarried, room)
		 { 
			 // TODO: Add Goblin specific initialization here
		 }

		// Constructor 
		Goblin(bool controllable, std::string name, std::string type, int strength, int currentHealth, int maxHealth, int minDamage, int maxDamage, int currentCarried, int maxCarried, Environment* room)
			: Character(controllable, name, type, (((double)rand()/RAND_MAX)*(GOBLIN_MAX_WEIGHT-GOBLIN_MIN_WEIGHT)+GOBLIN_MIN_WEIGHT), strength, currentHealth, maxHealth, minDamage, maxDamage, currentCarried, maxCarried, room)
		{
			// TODO: Add Goblin specific initialization here
		}

		// Does an action with the Goblin. 
		// An action can be anything from fight, go, talk etc.
		void action();

		// Damages the Goblin with a certian amount of points.
		void takeDamage(int damage);

		// Prints Goblin help message.
		bool help(std::string);
	};
}