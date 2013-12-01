/// Description: Header file for Apple class, inherits from Consumable.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-17
#pragma once
#include "Consumable.h"
#include "Character.h"


namespace GameLogic
{
	class Character; // Forward declaration solves circular dependancies

	class Apple : public Consumable
	{
	public:
		// Apple constructor.
		Apple() : Consumable("Apple", "Fruit", 5, 0.1, 1, false)
		{
			healthAffected_ = rand()%10-rand()%5;
		}

		// Applies the effect a potion has on the character.
		void applyEffect(Character& character);

	private:
		int healthAffected_;
	};

}

