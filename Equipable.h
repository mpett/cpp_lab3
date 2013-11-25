/// Description: Header file for Equipable class, inherits from Item class.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-17
#pragma once
#include "Item.h"
#include "Character.h"

namespace GameLogic
{
	class Character; // Forward declaration solves circular dependancies

	class Equipable : public Item
	{
	public:
		// Equipable constructor.
		Equipable(std::string name, std::string type, int price, double weight) : Item(name, type, price, weight){}

		// Returns true if the character passes the requirements of equipping this item.
		virtual bool requirementCheck(const Character& character) const = 0;

		// Returns a detailed string representation of this item.
		virtual std::string printItem() const;

		// Applies the effect the item has on the character.
		virtual void onEquip(Character& character) = 0;

		// Removes the effect the item has on the character.
		virtual void onUnequip(Character& character) = 0;	

		// Assigns left-hand Equipable to right-hand Equipable.
		virtual Equipable& operator=(const Equipable& env);
	};
}

