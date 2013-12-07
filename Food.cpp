/// Description: Implementation of Food class. Food can apply an effect on the character's health.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-07
#include "Food.h"
using std::string;

namespace GameLogic
{
	// Returns a detailed string representation of this item.
	// Used when saving the game information to file.
	string Food::printItem() const
	{
		std::stringstream ss;
		ss << Consumable::printItem(); 	// Start with printing consumable part
		ss << healthAffected_ << "\n";  // Food part
		
		return ss.str();
	}

	// Applies the effect Food has on the character.
	void Food::applyEffect(Character& character)
	{
		if((character.getCurrentHealth() + healthAffected_) >= character.getMaxHealth())
		{
			character.setCurrentHealth(character.getMaxHealth());
			nrUses_--;
			if(character.isControllable())
			{
				cout << "This was very yummy!" << endl;
			}
		} else
		{
			character.setCurrentHealth(character.getCurrentHealth() + healthAffected_);
			nrUses_--;	
			if(character.isControllable())
			{
				if(healthAffected_ < 0)
				{
					cout << "YUCK! The food was rotten. It damaged your health for " << healthAffected_ << " points." << endl;
				} else if(healthAffected_ > 0)
				{
					cout << "Yummy! You were healed for " << healthAffected_ << " health points" << endl;
				} else
				{
					cout << "This is strange. The food tastes just like air." << endl;
				}
			}
		}
	}

	// Returns health affected.
	int Food::getHealthAffected() const 
	{
		return healthAffected_;
	}

}

