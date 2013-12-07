/// Description: Implementation of Potion class. A potion can apply an effect on the character's health.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-07
#include "Potion.h"
#include <sstream>
using std::string;

namespace GameLogic
{
	// Returns a detailed string representation of this item.
	// Used when saving the game information to file.
	string Potion::printItem() const
	{
		std::stringstream ss;
		ss << Consumable::printItem(); 	// Start with printing consumable part
		ss << healthAffected_ << "\n";  // Potion part
		
		return ss.str();
	}

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

	// Returns health affected.
	int Potion::getHealthAffected() const
	{
		return healthAffected_;
	}

}

