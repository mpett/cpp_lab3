/// Description: A consumable is an item that can be consumed, either by using it or on pick up.
/// When an item is consumed it may apply an effect on its consuming character.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-24
#include "Consumable.h"
using std::string;

namespace GameLogic
{
	// Returns a detailed string representation of this item.
	string Consumable::printItem() const
	{
		//TODO
		return "This is not a consumable.";
	}

	// Returns true if the consumable was consumed.
	bool Consumable::isConsumed() const
	{
		return nrUses_ == 0;
	}

	// Returns true if this item is consumed and destroyed when picked up.
	bool Consumable::isConsumedOnPickup() const
	{
		return consumedOnPickUp_;
	}

	// Assigns left-hand Consumable to right-hand Consumable.
	Consumable& Consumable::operator=(const Consumable& env)
	{
		//TODO
		return *this;
	}
}

