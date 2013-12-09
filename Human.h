/// Description: Header file for Human class, inherits from Character class.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-08
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
		virtual ~Human();

		 // Constructor for character. Set every variable.
		 Human(bool controllable, std::string name, std::string type, double weight, int strength, int currentHealth, int maxHealth, int minDamage, int maxDamage, int currentCarried, int maxCarried, Equipable * currentArmor, Equipable * currentWeapon, bool canPerformAction, bool atNewRoom, Environment* room)
			 : Character(controllable, name, type, weight, strength, currentHealth, maxHealth, minDamage, maxDamage, currentCarried, maxCarried, currentArmor, currentWeapon, canPerformAction, atNewRoom,  room)
		 { 
			 // TODO: Add Human specific initialization here
		 }

		// Constructor. Generate some of the variables automatically.
		Human(bool controllable, std::string name, int strength, int currentHealth, int maxHealth, int minDamage, int maxDamage, int currentCarried, int maxCarried, Environment* room)
			: Character(controllable, name, "Human", (((double)rand()/RAND_MAX)*(HUMAN_MAX_WEIGHT-HUMAN_MIN_WEIGHT)+HUMAN_MIN_WEIGHT), strength, currentHealth, maxHealth, minDamage, maxDamage, currentCarried, maxCarried, room)
		{
			// TODO: Add Human specific initialization here
		}
		
		// Constructor. Generate some of the variables automatically (set type explicitly).
		Human(bool controllable, std::string name, std::string type, int strength, int currentHealth, int maxHealth, int minDamage, int maxDamage, int currentCarried, int maxCarried, Environment* room)
			: Character(controllable, name, type, (((double)rand()/RAND_MAX)*(HUMAN_MAX_WEIGHT-HUMAN_MIN_WEIGHT)+HUMAN_MIN_WEIGHT), strength, currentHealth, maxHealth, minDamage, maxDamage, currentCarried, maxCarried, room)
		{
			// TODO: Add Human specific initialization here
		}
	};
}