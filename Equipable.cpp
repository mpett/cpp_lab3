/// Description: An equipable item may be equipped by a character.
///				 When equipped, the character may be passed on an effect.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-20
#include "Equipable.h"
using std::string;

namespace GameLogic
{
	// Returns a detailed string representation of this item.
	string Equipable::printItem() const
	{
		//TODO
		return "This is not equipable.";
	}

	// Assigns left-hand Equipable to right-hand Equipable.
	Equipable& Equipable::operator=(const Equipable& env)
	{
		//TODO
		return *this;
	}
	
}

