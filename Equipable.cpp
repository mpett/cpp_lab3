/// Description: An equipable item may be equipped by a character.
///				 When equipped, the character may be passed on an effect.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-27
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

	// Returns true if item is equipped.
	bool Equipable::isEquipped() const
	{
		return equipped_;
	}

	// Returns a description of the requirement.
	string Equipable::getRequirementDesc() const
	{
		return requirementDesc_;
	}

	// Assigns left-hand Equipable to right-hand Equipable.
	Equipable& Equipable::operator=(const Equipable& env)
	{
		//TODO
		return *this;
	}
	
}

