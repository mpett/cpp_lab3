/// Description: An equipable item may be equipped by a character.
///				 When equipped, the character may be passed on an effect.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-07
#include "Equipable.h"
using std::string;

namespace GameLogic
{
	// Returns a detailed string representation of this item.
	// Used when saving the game information to file.
	string Equipable::printItem() const
	{
		std::stringstream ss;
		ss << Item::printItem(); 							 // Start with printing item part of the Equipable
		ss << ((equipped_) ? 1 : 0) << "\n" << requirementDesc_ << "\n"; // Equipable part
		
		return ss.str();
	}

	// Returns true if item is equipped.
	bool Equipable::isEquipped() const
	{
		return equipped_;
	}

	// Sets the decription of the requirement.
	void Equipable::setRequirementDesc(string desc)
	{
		requirementDesc_ = desc;
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

