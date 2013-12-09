/// Description: Implementation of Strength Potion class. A strength potion can apply an effect on the character's strength.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-09
#include "StrengthPotion.h"
#include <sstream>
using std::string;

namespace GameLogic
{
	// Returns a detailed string representation of this item.
	// Used when saving the game information to file.
	string StrengthPotion::printItem() const
	{
		std::stringstream ss;
		ss << Consumable::printItem(); 	  // Start with printing consumable part
		ss << strengthAffected_ << "\n";  // StrengthPotion part
		
		return ss.str();
	}

	// Applies the effect a potion has on the character.
	void StrengthPotion::applyEffect(Character& character)
	{
		character.setStrength(character.getStrength()+strengthAffected_);
		character.setMaxCarried(character.getMaxCarried()+strengthAffected_*5);
		nrUses_--;
		if(character.isControllable())
		{
			cout << "Your strength was modified by " << strengthAffected_ << " points." << endl;
			cout << "Your max. capacity was modified by " << strengthAffected_*5 << " points." << endl;
		}
	}

	// Returns health affected.
	int StrengthPotion::getStrengthAffected() const
	{
		return strengthAffected_;
	}
}

