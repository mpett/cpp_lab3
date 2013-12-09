/// Description: Header file for Golem class, inherits from Character class.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-09
#pragma once
#include <string>
#include "Character.h"

namespace GameLogic
{
	static const double GOLEM_MIN_WEIGHT = 350;
	static const double GOLEM_MAX_WEIGHT = 460;

	class Golem : public Character
	{
	public:
		// Destructor
		virtual ~Golem();
		
		 // Constructor for character. Set every variable.
		 Golem(bool controllable, std::string name, std::string type, double weight, int strength, int currentHealth, int maxHealth, int minDamage, int maxDamage, int currentCarried, int maxCarried, Equipable * currentArmor, Equipable * currentWeapon, bool canPerformAction, bool atNewRoom, Environment* room)
			 : Character(controllable, name, type, weight, strength, currentHealth, maxHealth, minDamage, maxDamage, currentCarried, maxCarried, currentArmor, currentWeapon, canPerformAction, atNewRoom,  room)
		 { 
			 aiBehavior_ = AI_BEHAVIOR::DO_NOT_HIT_ME;
		 }

		// Constructor. Generate some of the variables automatically.
		Golem(bool controllable, std::string name, int strength, int currentHealth, int maxHealth, int minDamage, int maxDamage, int currentCarried, int maxCarried, Environment* room)
			: Character(controllable, name, "Golem", (((double)rand()/RAND_MAX)*(GOLEM_MAX_WEIGHT-GOLEM_MIN_WEIGHT)+GOLEM_MIN_WEIGHT), strength, currentHealth, maxHealth, minDamage, maxDamage, currentCarried, maxCarried, room)
		{
			aiBehavior_ = AI_BEHAVIOR::DO_NOT_HIT_ME;
		}
		
		// Constructor. Generate some of the variables automatically (set type explicitly).
		Golem(bool controllable, std::string name, std::string type, int strength, int currentHealth, int maxHealth, int minDamage, int maxDamage, int currentCarried, int maxCarried, Environment* room)
			: Character(controllable, name, type, (((double)rand()/RAND_MAX)*(GOLEM_MAX_WEIGHT-GOLEM_MIN_WEIGHT)+GOLEM_MIN_WEIGHT), strength, currentHealth, maxHealth, minDamage, maxDamage, currentCarried, maxCarried, room)
		{
			aiBehavior_ = AI_BEHAVIOR::DO_NOT_HIT_ME;
		}
		
		// Returns the target Golem is currently pursuing.
		std::string getTarget();
		
		// Sets the target the Golem is currently pursuing.
		void setTarget(std::string target);
		
		// Returns a detailed string representation of this character.
		// Used when saving the game information to file.
		virtual std::string printCharacter() const;
		
		// Does an action with the character. 
		// An action can be anything from fight, go, talk etc.
		// This should typically only be called on NPCs.
		virtual void action();
	
	private:
		std::string target_;
	};
}