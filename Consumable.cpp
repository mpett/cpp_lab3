/// Description: A consumable is an item that can be consumed, either by using it or on pick up.
/// When an item is consumed it may apply an effect on its consuming character.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-09
#include "Consumable.h"
#include <sstream>
using std::string;

namespace GameLogic
{
	// Returns a detailed string representation of this item.
	// Used when saving the game information to file.
	// Format:
	// (base item members)
	// nrUses
	// consumedOnPickup
	string Consumable::printItem() const
	{
		std::stringstream ss;
		ss << Item::printItem(); 							// Start with printing item part of the Consumable
		ss << nrUses_ << "\n" << ((consumedOnPickUp_)?1:0) << "\n"; // Consumable part
		
		return ss.str();
	}

	// Returns true if the consumable was consumed.
	bool Consumable::isConsumed() const
	{
		return nrUses_ <= 0;
	}

	// Returns health affected by item.
	int Consumable::getHealthAffected() const
	{
		return 0;
	}
	
	// Returns the number of charges/uses of this consumable item.
	int Consumable::getNrUses() const
	{
		return nrUses_;
	}

	// Sets the number of charges/uses of this consumable item.
	void Consumable::setNrUses(int uses)
	{
		nrUses_ = uses;
	}

	// Returns true if this item is consumed and destroyed when picked up.
	bool Consumable::isConsumedOnPickup() const
	{
		return consumedOnPickUp_;
	}
}

