/// Description: A Golem is a magical sentry. 
///              Once a foe has been identified it will continue to track it until either it or itself are dead.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-09

#include "Golem.h"
#include <iostream>
using std::string;
using std::cout;
using std::cin;
using std::cerr;
using std::endl;

namespace GameLogic
{
	// Returns a detailed string representation of this character.
	// Used when saving the game information to file.
	string Golem::printCharacter() const
	{
		std::stringstream ss;
		ss << Character::printCharacter();
		ss << target_ << "\n";
		return ss.str();
	}

	// Destructor
	Golem::~Golem()
	{
		//TODO: Free Golem related allocated memory here
	}
	
	// Returns the target Golem is currently pursuing.
	string Golem::getTarget()
	{
		return target_;
	}
	
	// Sets the target the Golem is currently pursuing.
	void Golem::setTarget(string target)
	{
		target_ = target;
	}
	
	// Does an action with the character. 
	// An action can be anything from fight, go, talk etc.
	// This should typically only be called on NPCs.
	void Golem::action()
	{
		bool done = false;
		bool res = false;
		if(aiBehavior_ != AI_BEHAVIOR::DO_NOT_HIT_ME)
		{
			if(target_ == "")
			{
				auto chars = currentRoom_->getCharacters();
				// Find target Player
				for(auto it = chars.begin(); it != chars.end(); it++)
				{
					if(it->second->isControllable() && it->second->isAlive())
					{
						target_ = it->second->getName();
						cout << name_ << ":" << "TARGET IS " << target_ << endl;
						done = true;
					}
				}
			}
			else
			{
				// Test if character was in room
				if(!(res = attack(target_)))
				{
					// Pursuing
					cerr << "PURSUING TARGET " << endl;
					auto exits = currentRoom_->getExits();
					for(auto it = exits.begin(); it != exits.end(); it++)
					{
						// Found target
						if(it->second->getCharacter(target_) != nullptr)
						{
							go(it->first);
							done = true;
							cout << name_ << ":" << "FOUND TARGET " << it->first << endl;
							break;
						}
					}
					// Lost target
					if(!done) 
					{
						target_ = "";
						aiBehavior_ = AI_BEHAVIOR::DO_NOT_HIT_ME;
						cout << name_ << ":" << "LOST TARGET " << endl;
					}
				}
				else
				{
					// My work here is done
					if(currentRoom_->getCharacter(target_) == nullptr)
					{
						target_ = "";
						aiBehavior_ = AI_BEHAVIOR::DO_NOT_HIT_ME;
						cout << name_ << ":" << "TARGET DOWN" << endl;
						done = true;
					}
					else
					{
						if(!res && currentRoom_->containsPlayer()) cout << name_ << " stands still, waiting." << endl;
					}
				}
			}
		}
	}
}