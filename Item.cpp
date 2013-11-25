/// Description: An Item has a name, type, price and weight.
///				 It is typically carried by a character or present within an environment.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-24

#include "Item.h"
using std::string;

namespace GameLogic
{
	// Returns the name for this item.
	string Item::getName() const 
	{
		return name_;
	}

	// Returns the type for this item.
	string Item::getType() const
	{
		return type_;
	}

	// Returns a detailed string representation of this item.
	string Item::printItem() const
	{
		//TODO
		return "This is not an item.";
	}

	// Returns the price for this item.
	int Item::getPrice() const
	{
		return price_;
	}

	// Returns the weight for this item.
	double Item::getWeight() const
	{
		return weight_;
	}

	// Sets the name for this item.
	void Item::setName(string name)
	{
		name_ = name;
	}

	// Sets the type for this item.
	void Item::setType(string type)
	{
		type_ = type;
	}

	// Sets the price for this item.
	void Item::setPrice(int price)
	{
		price_ = price;
	}

	// Sets the weight for this item.
	void Item::setWeight(double weight)
	{
		weight_ = weight;
	}

	// Assigns left-hand Item to right-hand Item.
	Item& Item::operator=(const Item& env)
	{
		//TODO
		return *this;
	}
}

