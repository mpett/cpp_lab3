/// Description: Header file for Equipable class, inherits from Item class.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-09
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
		Equipable(std::string name, std::string type, int price, double weight, bool equipped, int id) : Item(name, type, price, weight,id),equipped_(equipped){}

		// Equipable constructor.
		Equipable(std::string name, std::string type, int price, double weight) : Item(name, type, price, weight){equipped_ = false;}

		// Returns true if the character passes the requirements of equipping this item.
		virtual bool requirementCheck(const Character& character) const = 0;

		// Returns a detailed string representation of this item.
		// Used when saving the game information to file.
		virtual std::string printItem() const;

		// Returns true if item is equipped.
		bool isEquipped() const;

		// Returns a description of the requirement.
		std::string getRequirementDesc() const;
		
		// Sets the decription of the requirement.
		void setRequirementDesc(std::string desc);

		// Applies the effect the item has on the character.
		virtual void onEquip(Character& character) = 0;

		// Removes the effect the item has on the character.
		virtual void onUnequip(Character& character) = 0;	

	protected:
		bool equipped_;
		std::string requirementDesc_;
	};
}

