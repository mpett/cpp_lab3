/// Description: Header file for Item class.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-17
#pragma once
#include <string>

namespace GameLogic
{
	class Item 
	{
	public:
		// Constructor of Item class.
		Item(std::string name, std::string type, int price, double weight) : name_(name), type_(type), price_(price), weight_(weight){}

		// Returns the name for this item.
		std::string getName() const; 

		// Returns the type for this item.
		std::string getType() const;

		// Returns a detailed string representation of this item.
		virtual std::string printItem() const;

		// Returns the price for this item.
		int getPrice() const;

		// Returns the weight for this item.
		double getWeight() const;

		// Sets the name for this item.
		void setName(std::string name);

		// Sets the type for this item.
		void setType(std::string type);

		// Sets the price for this item.
		void setPrice(int price);

		// Sets the weight for this item.
		void setWeight(double weight);

		// Assigns left-hand Item to right-hand Item.
		virtual Item& operator=(const Item& env);

	protected:
		std::string name_;
		std::string type_;
		int price_;
		double weight_;	
	};
}

