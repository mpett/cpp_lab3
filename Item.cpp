/// Description: An Item has a name, type, price and weight.
///				 It is typically carried by a character or present within an environment.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-09

#include "Item.h"
#include <sstream>
using std::string;

namespace GameLogic
{
	// Returns the unique id of this item.
	int Item::getId() const
	{
		return myId_;
	}

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
	// Used when saving the game information to file.
	// Format: 
	// Name
	// Id
	// Type
	// Price
	// Weight
	string Item::printItem() const
	{
		std::stringstream ss;
		ss << name_ << "\n" << myId_ << "\n" << type_ << "\n" << price_ << "\n" << weight_ << "\n";
		return ss.str();
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
}

