/// Description: Header file for Dragon class, inherits from Character class.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-09
#pragma once
#include <string>
#include "Character.h"

namespace GameLogic
{
	static const double DRAGON_MIN_WEIGHT = 1000;
	static const double DRAGON_MAX_WEIGHT = 2000;

	class Dragon : public Character
	{
	public:
		// Destructor
		virtual ~Dragon();
		
		 // Constructor for character. Set every variable.
		 Dragon(bool controllable, std::string name, std::string type, double weight, int strength, int currentHealth, int maxHealth, int minDamage, int maxDamage, int currentCarried, int maxCarried, Equipable * currentArmor, Equipable * currentWeapon, bool canPerformAction, bool atNewRoom, Environment* room)
			 : Character(controllable, name, type, weight, strength, currentHealth, maxHealth, minDamage, maxDamage, currentCarried, maxCarried, currentArmor, currentWeapon, canPerformAction, atNewRoom,  room)
		 { 
			currFire_ = 0;
			fireCapacity_ = 0;
			fireRegRate_ = 0;
		 }

		// Constructor. Generate some of the variables automatically.
		Dragon(bool controllable, std::string name, int strength, int currentHealth, int maxHealth, int minDamage, int maxDamage, int currentCarried, int maxCarried, Environment* room)
			: Character(controllable, name, "Dragon", (((double)rand()/RAND_MAX)*(DRAGON_MAX_WEIGHT-DRAGON_MIN_WEIGHT)+DRAGON_MIN_WEIGHT), strength, currentHealth, maxHealth, minDamage, maxDamage, currentCarried, maxCarried, room)
		{
			currFire_ = 0;
			fireCapacity_ = 0;
			fireRegRate_ = 0;
		}
		
		// Constructor. Generate some of the variables automatically (set type explicitly).
		Dragon(bool controllable, std::string name, std::string type, int strength, int currentHealth, int maxHealth, int minDamage, int maxDamage, int currentCarried, int maxCarried, Environment* room)
			: Character(controllable, name, type, (((double)rand()/RAND_MAX)*(DRAGON_MAX_WEIGHT-DRAGON_MIN_WEIGHT)+DRAGON_MIN_WEIGHT), strength, currentHealth, maxHealth, minDamage, maxDamage, currentCarried, maxCarried, room)
		{
			currFire_ = 0;
			fireCapacity_ = 0;
			fireRegRate_ = 0;
		}

		// Returns the fire capacity.
		int getFireCapacity();

		// Sets the amount of fire that dragon can dispears on a character.
		void setFireCapacity(int val);

		// Returns the current amount of fire.
		int getFire();

		// Sets the current amount of fire.
		void setCurrFire(int val);

		// Returns the rate at which fire generates (per turn).
		int getFireGen();

		// Sets the rate at which fire generates (per turn).
		void setFireGen(int val);
		
		// Returns a detailed string representation of this character.
		// Used when saving the game information to file.
		virtual std::string printCharacter() const;
		
		// Does an action with the character. 
		// An action can be anything from fight, go, talk etc.
		// This should typically only be called on NPCs.
		virtual void action();
	
	private:
		int currFire_;
		int fireCapacity_;
		int fireRegRate_;

		// Dispenses fire upon character.
		void breathFire(Character& character);

		// Dispenses fire upon a random character in room.
		bool tryRandomFireAll();

		// Dispenses fire upon a random player character in room.
		bool tryRandomFire();
	};
}