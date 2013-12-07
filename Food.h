/// Description: Header file for Food class, inherits from Consumable.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-07
#pragma once
#include "Consumable.h"
#include "Character.h"


namespace GameLogic
{
	class Character; // Forward declaration solves circular dependancies

	class Food : public Consumable
	{
	public:
		// Apple constructor.
		Food(std::string name) : Consumable(name, "Food", 5, 0.1, 1, false)
		{
			healthAffected_ = rand()%10-rand()%5;
		}

		// Apple constructor.
		Food(std::string name, std::string type, int price, double weight, int nrUses, bool consumedOnPickUp, int healthAffected, int id) : Consumable(name, type, price, weight, nrUses, consumedOnPickUp, id)
		{
			healthAffected_ = healthAffected;
		}
		
		// Returns a detailed string representation of this item.
		// Used when saving the game information to file.
		virtual std::string printItem() const;

		// Applies the effect a potion has on the character.
		void applyEffect(Character& character);

		// Returns health affected.
		int getHealthAffected() const;

	private:
		int healthAffected_;
	};

}

