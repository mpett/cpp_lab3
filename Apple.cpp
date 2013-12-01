/// Description: Implementation of Apple class. An apple can apply an effect on the character's health.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-29
#include "Apple.h"

namespace GameLogic
{
	// Applies the effect a potion has on the character.
	void Apple::applyEffect(Character& character)
	{
		if((character.getCurrentHealth() + healthAffected_) >= character.getMaxHealth())
		{
			character.setCurrentHealth(character.getMaxHealth());
			nrUses_--;
			if(character.isControllable())
			{
				cout << "This apple was very yummy!" << endl;
			}
		} else
		{
			character.setCurrentHealth(character.getCurrentHealth() + healthAffected_);
			nrUses_--;	
			if(character.isControllable())
			{
				if(healthAffected_ < 0)
				{
					cout << "YUCK! This apple was rotten. It damaged your health for " << healthAffected_ << " points." << endl;
				} else if(healthAffected_ > 0)
				{
					cout << "Yummy! You were healed for " << healthAffected_ << " health points" << endl;
				} else
				{
					cout << "This is strange. This apple tastes just like air." << endl;
				}
				
			}
		}
		
	}

}

