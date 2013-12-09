/// Description: A Scholar is a human on search for something. This something is often an object.
///              Once the object has been found, the scholar typically likes to drop something as a reward.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-09

#include "Scholar.h"
#include <iostream>
using std::cout;
using std::string;
using std::cin;
using std::cerr;
using std::endl;

namespace GameLogic
{
	// Destructor
	Scholar::~Scholar()
	{
		//TODO: Free Scholar related allocated memory here
	}
	
	// Returns a detailed string representation of this character.
	// Used when saving the game information to file.
	string Scholar::printCharacter() const
	{
		std::stringstream ss;
		ss << Human::printCharacter();
		ss << searchedObj_ << "\n" << rewardObj_ << "\n";
		return ss.str();
	}
	
	// Returns the searched obj string
	string Scholar::getSearchedObject()
	{
		return searchedObj_;
	}
	
	// Sets which object that the scholar is looking for
	void Scholar::setSearchedObject(string object)
	{
		searchedObj_ = object;
	}
	
	// Sets which object that the scholar will reward us with.
	void Scholar::setRewardObject(string object)
	{
		rewardObj_ = object;
	}
	
	// Returns the reward obj string
	string Scholar::getRewardObject()
	{
		return rewardObj_;
	}
	
	// Does an action with the character. 
	// An action can be anything from fight, go, talk etc.
	// This should typically only be called on NPCs.
	void Scholar::action()
	{
		if(searchedObj_ != "")
		{
			auto chars = currentRoom_->getCharacters();
			bool foundSearchedItem = false;
			
			for(auto it = chars.begin(); it != chars.end(); it++)
			{
				if(it->second != this && it->second->isControllable() && it->second->isAlive())
				{
					auto miscItem = it->second->getInvMiscItem(searchedObj_);
					if(miscItem != nullptr)
					{
						it->second->removeInvMiscItem(searchedObj_);
						cout << name_ << ": Thank you for finding " << searchedObj_ << endl;
						addInvMiscItem(miscItem);
						foundSearchedItem = true;
						break;
					}
					
					auto consItem = it->second->getInvConsumable(searchedObj_);
					if(consItem != nullptr)
					{
						it->second->removeInvConsumable(searchedObj_);
						cout << name_ << ": Thank you for finding " << searchedObj_ << endl;
						addInvConsumable(consItem);
						foundSearchedItem = true;
						break;
					}
					
					auto equipItem = it->second->getInvEquipable(searchedObj_);
					if(equipItem != nullptr)
					{
						if(equipItem->isEquipped()) it->second->unequip(searchedObj_);
						it->second->removeInvEquipable(searchedObj_);
						cout << name_ << ": Thank you for finding " << searchedObj_ << endl;
						addInvEquipable(equipItem);
						foundSearchedItem = true;
						break;
					}
					
					auto character = it->second->getInvCharacter(searchedObj_);
					if(character != nullptr)
					{
						it->second->removeInvCharacter(searchedObj_);
						cout << name_ << ": Thank you for finding " << searchedObj_ << endl;
						character->setCurrentRoom(currentRoom_, false);
						foundSearchedItem = true;
						break;
					}
				}
			}
			
			// Drop loot and reset searched item (no longer looking for this)
			if(foundSearchedItem)
			{
				cout << name_ << ": Here, please have this " << rewardObj_ << endl;
				thingsToSay_.clear();
				thingsToSay_.push_back("Thanks again!");
				thingsToSay_.push_back("Hmm, let see..if one where to do..no, no, that would not work..");
				thingsToSay_.push_back("Ah its my good old friend, thanks again for helping me!");
				thingsToSay_.push_back("Mumble, mumble...");
				auto equipItem = getInvEquipable(rewardObj_);
				if(equipItem != nullptr)
				{
					if(equipItem->isEquipped()) unequip(rewardObj_);
				}
				drop(rewardObj_);
				searchedObj_ = ""; // No more are we looking for this
			}
		}
		
		Human::action(); // Continue to do ordinary actions
	}
}