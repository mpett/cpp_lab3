/// Description: Header file for NormalArmor, inherits from Equipable class.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-07
#pragma once
#include "Equipable.h"
#include "Character.h"

namespace GameLogic
{
	class Character; // Forward declaration solves circular dependancies

	class NormalArmor : public Equipable
	{
	public:
		// NormalArmor constructor.
		NormalArmor(std::string name, int price, double weight, int maxHealthIncr, int strengthReq) : Equipable(name, "Normal Armor", price, weight)
		{
			maxHealthIncr_ = maxHealthIncr;
			strengthReq_ = strengthReq;
			std::stringstream ss;
			ss << "Character must have a strength of " << strengthReq_;
			requirementDesc_ =  ss.str();
		};

		// NormalArmor constructor.
		NormalArmor(std::string name, std::string type, int price, double weight, bool equipped, int maxHealthIncr, int strengthReq, int id) : Equipable(name, type, price, weight, equipped, id)
		{
			maxHealthIncr_ = maxHealthIncr;
			strengthReq_ = strengthReq;
			std::stringstream ss;
			ss << "Character must have a strength of " << strengthReq_;
			requirementDesc_ =  ss.str();
		}
		
		// Returns a detailed string representation of this item.
		// Used when saving the game information to file.
		virtual std::string printItem() const;

		// Returns true if the character passes the requirements of equipping this item.
		bool requirementCheck(const Character& character) const;

		// Applies the effect the item has on the character.
		void onEquip(Character& character);

		// Removes the effect the item has on the character.
		void onUnequip(Character& character);	

	private:
		int maxHealthIncr_;
		int strengthReq_;
	};
}

