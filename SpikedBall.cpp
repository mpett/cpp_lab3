/// Description: A Spiked Ball is similar to a potion, in a bad way however...
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-29

#include "SpikedBall.h"
using std::string;

namespace GameLogic
{
	// Returns a detailed string representation of this item.
	// Used when saving the game information to file.
	string SpikedBall::printItem() const
	{
		std::stringstream ss;
		ss << Consumable::printItem(); // Start with printing consumable part of the SpikedBall
		ss << healthAffected_ << "\n"; // SpikedBall part
		
		return ss.str();
	}

	// Applies the effect a spiked ball has on the character.
	void SpikedBall::applyEffect(Character& character)
	{
		if(character.isControllable())
		{
			cout << "This spiked ball was indeed spiky." << endl;
		}
		character.takeDamage(healthAffected_);
		nrUses_--;	
	}

	// Returns the health affected.
	int SpikedBall::getHealthAffected() const
	{
		return healthAffected_;
	}
}

