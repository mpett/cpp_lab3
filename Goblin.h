/// Description: Header file for Goblin class, inherits from Character class.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-06
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
		virtual ~Goblin();

		 // Constructor for Goblin. Set every variable.
		 Goblin(bool controllable, std::string name, std::string type, double weight, int strength, int currentHealth, int maxHealth, int minDamage, int maxDamage, int currentCarried, int maxCarried, Equipable * currentArmor, Equipable * currentWeapon, bool canPerformAction, bool atNewRoom, Environment* room)
			 : Character(controllable, name, type, weight, strength, currentHealth, maxHealth, minDamage, maxDamage, currentCarried, maxCarried, currentArmor, currentWeapon, canPerformAction, atNewRoom, room)
		 { 
			 // TODO: Add Goblin specific initialization here
		 }

		// Constructor for Goblin. Generate some of the variables automatically.
		Goblin(bool controllable, std::string name, int strength, int currentHealth, int maxHealth, int minDamage, int maxDamage, int currentCarried, int maxCarried, Environment* room)
			: Character(controllable, name, "Goblin", (((double)rand()/RAND_MAX)*(GOBLIN_MAX_WEIGHT-GOBLIN_MIN_WEIGHT)+GOBLIN_MIN_WEIGHT), strength, currentHealth, maxHealth, minDamage, maxDamage, currentCarried, maxCarried, room)
		{
			// TODO: Add Goblin specific initialization here
		}
		
		// Constructor for Goblin. Generate some of the variables automatically (set type explicitly).
		Goblin(bool controllable, std::string name, std::string type, int strength, int currentHealth, int maxHealth, int minDamage, int maxDamage, int currentCarried, int maxCarried, Environment* room)
			: Character(controllable, name, type, (((double)rand()/RAND_MAX)*(GOBLIN_MAX_WEIGHT-GOBLIN_MIN_WEIGHT)+GOBLIN_MIN_WEIGHT), strength, currentHealth, maxHealth, minDamage, maxDamage, currentCarried, maxCarried, room)
		{
			// TODO: Add Goblin specific initialization here
		}
	};
}