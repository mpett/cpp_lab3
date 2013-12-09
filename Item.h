/// Description: Header file for Item class.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-09
#pragma once
#include <string>

namespace GameLogic
{
	// Global Item ID.
	static int ITEM_ID;

	class Item 
	{
	public:
		// Constructor of Item class.
		Item(std::string name, std::string type, int price, double weight, int id) : name_(name), type_(type), price_(price), weight_(weight), myId_(id){}

		// Constructor of Item class.
		Item(std::string name, std::string type, int price, double weight) : name_(name), type_(type), price_(price), weight_(weight){myId_ = ITEM_ID; ITEM_ID++;}

		// Returns the unique id of this item.
		int getId() const;

		// Returns the name for this item.
		std::string getName() const; 

		// Returns the type for this item.
		std::string getType() const;

		// Returns a detailed string representation of this item.
		// Used when saving the game information to file.
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

	protected:
		std::string name_;
		std::string type_;
		int price_;
		double weight_;	
		int myId_;
	};
}

