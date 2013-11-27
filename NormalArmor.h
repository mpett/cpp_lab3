/// Description: Header file for NormalArmor, inherits from Equipable class.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-27
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
		NormalArmor(std::string name, int price, double weight, int maxHealthIncr, int strengthReq) : Equipable(name, "Armor", price, weight)
		{
			maxHealthIncr_ = maxHealthIncr;
			strengthReq_ = strengthReq;
			std::stringstream ss;
			ss << "Character must have a strength of " << strengthReq_;
			requirementDesc_ =  ss.str();
		};

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

