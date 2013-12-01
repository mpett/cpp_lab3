/// Description: Header file for SpikedBall class, inherits from Consumable.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-17
#pragma once
#include "Consumable.h"
#include "Character.h"


namespace GameLogic
{
	class Character; // Forward declaration solves circular dependancies

	class SpikedBall : public Consumable
	{
	public:
		// Spiked Ball constructor.
		SpikedBall(std::string name, int price, double weight,  int healthAffected) : Consumable(name, "Spiked Ball of Pain and Death", price, weight, 1, true), healthAffected_(healthAffected){}

		// Applies the effect a spiked ball has on the character.
		void applyEffect(Character& character);

	private:
		int healthAffected_;

	};

}

