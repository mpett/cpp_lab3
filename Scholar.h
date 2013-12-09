/// Description: Header file for Scholar class, inherits from Human class.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-08
#pragma once
#include <string>
#include "Human.h"

namespace GameLogic
{
	class Scholar : public Human
	{
	public:
		// Destructor
		virtual ~Scholar();

		 // Constructor for character. Set every variable.
		 Scholar(bool controllable, std::string name, std::string type, double weight, int strength, int currentHealth, int maxHealth, int minDamage, int maxDamage, int currentCarried, int maxCarried, Equipable * currentArmor, Equipable * currentWeapon, bool canPerformAction, bool atNewRoom, Environment* room)
			 : Human(controllable, name, type, weight, strength, currentHealth, maxHealth, minDamage, maxDamage, currentCarried, maxCarried, currentArmor, currentWeapon, canPerformAction, atNewRoom, room)
		 { 
			 // TODO: Add Human specific initialization here
		 }
		
		// Constructor. Generate some of the variables automatically (set type explicitly).
		Scholar(bool controllable, std::string name, int strength, int currentHealth, int maxHealth, int minDamage, int maxDamage, int currentCarried, int maxCarried, Environment* room)
			: Human(controllable, name, "Scholar", strength, currentHealth, maxHealth, minDamage, maxDamage, currentCarried, maxCarried, room)
		{
			// TODO: Add Human specific initialization here
		}
		
		// Sets which object that the scholar is looking for.
		void setSearchedObject(std::string object);
		
		// Returns the searched obj string
		std::string getSearchedObject();
		
		// Sets which object that the scholar will reward us with.
		void setRewardObject(std::string object);
		
		// Returns the reward obj string
		std::string getRewardObject();
		
		// Does an action with the character. 
		// An action can be anything from fight, go, talk etc.
		// This should typically only be called on NPCs.
		void action();
		
		// Returns a detailed string representation of this character.
		// Used when saving the game information to file.
		virtual std::string printCharacter() const;
		
	private:
		std::string searchedObj_;
		std::string rewardObj_;
	};
}