/// Description: Header file for Human class, inherits from Character class.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-24
#pragma once
#include <string>
#include "Character.h"

namespace GameLogic
{
	static const double HUMAN_MIN_WEIGHT = 45;
	static const double HUMAN_MAX_WEIGHT = 160;

	class Human : public Character
	{
	public:
		// Destructor
		~Human();

		 // Constructor for character. Set every variable.
		 Human(bool controllable, std::string name, std::string type, double weight, int strength, int currentHealth, int maxHealth, int minDamage, int maxDamage, int currentCarried, int maxCarried, Environment* room)
			 : Character(controllable, name, type, weight, strength, currentHealth, maxHealth, minDamage, maxDamage, currentCarried, maxCarried, room)
		 { 

			 // TODO: Add Human specific initialization here
		 }

		// Constructor. Generate some of the variables automatically.
		Human(bool controllable, std::string name, std::string type, int strength, int currentHealth, int maxHealth, int minDamage, int maxDamage, int currentCarried, int maxCarried, Environment* room)
			: Character(controllable, name, type, (((double)rand()/RAND_MAX)*(HUMAN_MAX_WEIGHT-HUMAN_MIN_WEIGHT)+HUMAN_MIN_WEIGHT), strength, currentHealth, maxHealth, minDamage, maxDamage, currentCarried, maxCarried, room)
		{
			// TODO: Add Human specific initialization here
		}

		// Does an action with the Human. 
		// An action can be anything from fight, go, talk etc.
		void action();

		// Damages the Human with a certian amount of points.
		void takeDamage(int damage);

		// Prints a help message with a basic description of the game.
		bool help(std::string);
	};
}