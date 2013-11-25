/// Description: A Goblin is a small creature that is quite aggresive in a non-threatening way.
///              They are typically uncontrollable (i.e. aggressive NPC).
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-22

#include "Goblin.h"
#include <iostream>
using std::cout;
using std::cin;
using std::cerr;
using std::endl;

namespace GameLogic
{
	// Destructor
	Goblin::~Goblin()
	{
		//TODO: Free Goblin related allocated memory here
	}

	// Does an action with the Goblin. 
	// An action can be anything from fight, go, talk etc.
	void Goblin::action()
	{
		cerr << "Im a Goblin - I want to kill you!" << endl;
	}

	// Damages the Goblin with a certian amount of points.
	void Goblin::takeDamage(int damage)
	{
		currentHealth_ -= damage;
		cout << "Ouch! Was hit for " << damage << " points of damage!" << endl;
	}

	// Prints Goblin help message.
	bool Goblin::help(std::string)
	{
		cout << "Help message for Goblin." << endl;
		return false;
	}
}