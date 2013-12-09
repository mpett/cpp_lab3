/// Description: Header file for Strength Potion class, inherits from Consumable.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-08
#pragma once
#include "Consumable.h"

namespace GameLogic
{
	class Character; // Forward declaration solves circular dependancies

	class StrengthPotion : public Consumable
	{
	public:
		// StrengthPotion constructor.
		StrengthPotion(std::string name, int price, double weight, int nrUses, bool consumedOnPickUp, int strengthAffected) : Consumable(name, "Strength Potion", price, weight, nrUses, consumedOnPickUp), strengthAffected_(strengthAffected){}

		// StrengthPotion constructor.
		StrengthPotion(std::string name, std::string type, int price, double weight, int nrUses, bool consumedOnPickUp, int strengthAffected, int id) : Consumable(name, type, price, weight, nrUses, consumedOnPickUp, id)
		{
			strengthAffected_ = strengthAffected;
		}
		
		// Returns a detailed string representation of this item.
		// Used when saving the game information to file.
		virtual std::string printItem() const;

		// Applies the effect a potion has on the character.
		void applyEffect(Character& character);

		// Returns health affected.
		int getStrengthAffected() const;

	private:
		int strengthAffected_;
	};

}

