/// Description: A Spiked Ball is similar to a potion, in a bad way however...
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-29

#include "SpikedBall.h"

namespace GameLogic
{
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
}

