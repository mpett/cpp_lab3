/// Description: A Dragon is a fierce creature often capable of breathing fire on an unknowing opponent. 
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-09

#include "Dragon.h"
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
	string Dragon::printCharacter() const
	{
		std::stringstream ss;
		ss << Character::printCharacter();
		ss << currFire_ << "\n" << fireCapacity_ << "\n" << fireRegRate_ << "\n";
		return ss.str();
	}

	// Destructor
	Dragon::~Dragon()
	{
		//TODO: Free Dragon related allocated memory here
	}
	
	// Returns the fire capacity.
	int Dragon::getFireCapacity()
	{
		return fireCapacity_;
	}

	// Sets the amount of fire that dragon can dispears on a character.
	void Dragon::setFireCapacity(int val)
	{
		fireCapacity_ = val;
	}

	// Returns the current amount of fire.
	int Dragon::getFire()
	{
		return currFire_;
	}

	// Sets the current amount of fire.
	void Dragon::setCurrFire(int val)
	{
		currFire_ = val;
	}

	// Returns the rate at which fire generates (per turn).
	int Dragon::getFireGen()
	{
		return fireRegRate_;
	}

	// Sets the rate at which fire generates (per turn).
	void Dragon::setFireGen(int val)
	{
		fireRegRate_ = val;
	}

	// Dispenses fire upon character
	void Dragon::breathFire(Character& character)
	{
		if(character.isControllable())
		{
			cout << name_ << " released a set of flames on " << character.getName() << "!" << endl;
		}
		character.takeDamage(currFire_);
		currFire_ = 0;
	}

	// Dispenses fire upon a random character in room
	bool Dragon::tryRandomFireAll()
	{
		string tryCharacter = currentRoom_->getRandomCharacter();
		Character * character = currentRoom_->getCharacter(tryCharacter);
			
		// Avoid hitting nothing and self
		if(character != nullptr && character != this)
		{
			breathFire(*character);
			return true;
		}
		else
		{
			return false;
		}
	}

	// Dispenses fire upon a random player character in room
	bool Dragon::tryRandomFire()
	{
		string tryCharacter = currentRoom_->getRandomCharacter();
		Character * character = currentRoom_->getCharacter(tryCharacter);
			
		// Avoid hitting nothing and self (only hit other Players)
		if(character != nullptr && character != this && character->isControllable())
		{
			breathFire(*character);
			return true;
		}
		else
		{
			return false;
		}
	}
	
	// Does an action with the character. 
	// An action can be anything from fight, go, talk etc.
	// This should typically only be called on NPCs.
	void Dragon::action()
	{
		currFire_ += fireRegRate_;
		if(currFire_ > fireCapacity_) currFire_ = fireCapacity_;

		int dice = rand() % 100;
		switch(aiBehavior_)
		{
			// NPC will only stand still with this behavior 
			case(AI_BEHAVIOR::STAND_STILL):
			{
				break;
			}
			// NPC will move around and talk with this behavior
			case(AI_BEHAVIOR::PACIFIST):
			{
				// Move
				if(dice < 30)
				{
					tryRandomMove();
				}
				// Try to talk
				else if(dice < 60)
				{
					tryRandomSpeak();
				}
				break;
			}
			// NPC will move around and pickup items with this behavior
			case(AI_BEHAVIOR::PASSIVE_LOOTER):
			{
				// Move
				if(dice < 30)
				{
					tryRandomMove();
				}
				// Pickup random item
				else if(dice < 60)
				{
					tryPickupRandomItem();
				}
				break;
			}
			// NPC will move around, attacking anything it encounters
			case(AI_BEHAVIOR::AGGRESSIVE_AMOVE):
			{
				// Only attack if there are more than 1 in room (i.e. not alone)
				if(currentRoom_->getCharacters().size() > 1)
				{
					if(dice > 50 && currFire_ != 0) tryRandomFireAll();
					tryRandomAttackAll();
				}
				// Time to get a movin'
				else if(dice > 40)
				{
					tryRandomMove();
				}
				break;
			}
			// NPC will move around at random, attacking any player character it encounters   
			case(AI_BEHAVIOR::AMOVE):
			{
				bool res = false;
				// Only attack if there are more than 1 in room (i.e. not alone)
				if(currentRoom_->containsPlayer())
				{
					// 50 % chance to release fire
					if(dice > 50 && currFire_ != 0) res = tryRandomFire();
					res = tryRandomAttack() || res;				
				}
				// Time to get a movin'
				else if(dice > 40)
				{
					res = tryRandomMove();
				}
				if(!res && currentRoom_->containsPlayer()) cout << name_ << " stands still, waiting." << endl;
				break;
			}
			// NPC will remain in room but will attack anything it encounters (even other NPCs)
			case(AI_BEHAVIOR::AGGRESSIVE_GUARD):
			{
				bool res = false;
				if(dice > 50 && currFire_ != 0) res = tryRandomFireAll();
				res = tryRandomAttackAll() || res;

				if(!res && currentRoom_->containsPlayer()) cout << name_ << " stands still, waiting." << endl;
				break;
			}
			// NPC will remain in room, will attack any player character it encounters
			case(AI_BEHAVIOR::GUARD):
			{
				if(currentRoom_->containsPlayer())
				{
					bool res = false;
					// 50 % chance to release fire
					if(dice > 50 && currFire_ != 0) res = tryRandomFire();
					res = tryRandomAttack() || res;

					if(!res) cout << name_ << " stands still, waiting." << endl;
				}
				break;
			}
			// NPC will pickup anything it encounters (and have capacity to pickup) - that includes characters so beware!
			case(AI_BEHAVIOR::COLLECTOR):
			{
				// Move
				if(dice < 25)
				{
					tryRandomMove();
				}
				// Pickup random item
				else if(dice < 50)
				{
					tryPickupRandomItem();
				}
				// Pickup random character
				else if(dice < 70)
				{
					tryPickupRandomCharacter();
				}
				break;
			}
			// NPC will remain docile (and stand still) as long as nobody hits it - then it will go to AGGRESSIVE_AMOVE behavior.
			case(AI_BEHAVIOR::DO_NOT_HIT_ME):
			{
				// At the moment DO_NOT_HIT_ME is the same as STAND_STILL...until hit that is
				break;
			}
			// NPC will try to do all sorts of actions with all sorts of objects
			case(AI_BEHAVIOR::CRAZY):
			{
				bool res = false;
				// Move
				if(dice < 16)
				{
					res = tryRandomMove();
				}
				// Pickup random item
				else if(dice < 32)
				{
					tryPickupRandomItem();
				}
				// Pickup random character
				else if(dice < 48)
				{
					tryPickupRandomCharacter();
				}
				// Try attack a random character
				else if(64)
				{
					// 50 % chance to release fire
					if(dice > 50 && currFire_ != 0) res = tryRandomFireAll();
					res = tryRandomAttackAll() || res;
				}
				// Speak
				else if(80)
				{
					tryRandomSpeak();
				}
				if(!res && currentRoom_->containsPlayer()) cout << name_ << " stands still, waiting." << endl;
				break;
			}
		}
	}
}