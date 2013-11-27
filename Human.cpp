/// Description: A Human is the most common race in almost every fantasy setting.
///				 Its statistics are fairly normal.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-24

#include "Human.h"
#include <iostream>
using std::cout;
using std::cin;
using std::cerr;
using std::endl;

namespace GameLogic
{
	// Destructor
	Human::~Human()
	{
		//TODO: Free Human related allocated memory here
	}

	// Does an action with the Human. 
	// An action can be anything from fight, go, talk etc.
	void Human::action()
	{
		cerr << "Im a Human!" << endl;
	}
}