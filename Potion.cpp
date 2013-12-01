/// Description: Implementation of Potion class. A potion can apply an effect on the character's health.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-29
#include "Potion.h"

namespace GameLogic
{
	// Applies the effect a potion has on the character.
	void Potion::applyEffect(Character& character)
	{
		if((character.getCurrentHealth() + healthAffected_) >= character.getMaxHealth())
		{
			character.setCurrentHealth(character.getMaxHealth());
			nrUses_--;
			if(character.isControllable())
			{
				cout << "Your health was restored to its maximum value." << endl;
			}
		} else
		{
			character.setCurrentHealth(character.getCurrentHealth() + healthAffected_);
			nrUses_--;	
			if(character.isControllable())
			{
				cout << "You were healed for " << healthAffected_ << " health points" << endl;
			}
		}
		
	}

}

