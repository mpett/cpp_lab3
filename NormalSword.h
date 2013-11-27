/// Description: Header file for NormalSword, inherits from Equipable class.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-27
#pragma once
#include "Equipable.h"
#include "Character.h"

namespace GameLogic
{
	class Character; // Forward declaration solves circular dependancies

	class NormalSword : public Equipable
	{
	public:
		// NormalSword constructor.
		NormalSword(std::string name, int price, double weight, int minDamageIncr, int maxDamageIncr, int strengthReq) : Equipable(name, "Sword", price, weight)
		{
			minDamageIncr_ = minDamageIncr;
			maxDamageIncr_ = maxDamageIncr;
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
		int minDamageIncr_;
		int maxDamageIncr_;
		int strengthReq_;
	};
}

