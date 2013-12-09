/// Description: Header file for Normal Bow, inherits from Equipable class.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-08
#pragma once
#include "Equipable.h"
#include "Character.h"

namespace GameLogic
{
	class Character; // Forward declaration solves circular dependancies

	class NormalBow : public Equipable
	{
	public:
		// NormalBow constructor.
		NormalBow(std::string name, int price, double weight, int minDamageIncr, int maxDamageIncr, int strengthReq) : Equipable(name, "Normal Bow", price, weight)
		{
			minDamageIncr_ = minDamageIncr;
			maxDamageIncr_ = maxDamageIncr;
			strengthReq_ = strengthReq;
			std::stringstream ss;
			ss << "Character must have a strength of " << strengthReq_ << " and an arrow in inventory.";
			requirementDesc_ =  ss.str();
		}

		// NormalBow constructor. (Used for loading)
		NormalBow(std::string name, std::string type, int price, double weight, bool equipped, int minDamageIncr, int maxDamageIncr, int strengthReq, int id) : Equipable(name, type, price, weight, equipped, id)
		{
			minDamageIncr_ = minDamageIncr;
			maxDamageIncr_ = maxDamageIncr;
			strengthReq_ = strengthReq;
			std::stringstream ss;
			ss << "Character must have a strength of " << strengthReq_ << " and an arrow in inventory.";
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
		int minDamageIncr_;
		int maxDamageIncr_;
		int strengthReq_;
	};
}

