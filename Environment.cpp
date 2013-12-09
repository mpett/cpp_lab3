/// Description: An Environment is basicly a room in the game world. 
///              It can contain a multitude of things such as characters, items and exits. 
///              It will typically have a description which will be displayed upon entering the room with a character controlled by the player.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-09

#include <algorithm>
#include "Environment.h"
#include <sstream>
using std::string;
using std::stringstream;

namespace GameLogic
{
	// Destructor.
	Environment::~Environment()
	{
		//TODO: Destroy, destroy, destroy!

		for(auto it = miscItems_.begin(); it != miscItems_.end(); it++)
		{
			if(it->second != nullptr)
			{
				delete it->second;
			}
		}
		for(auto it = equipables_.begin(); it != equipables_.end(); it++)
		{
			if(it->second != nullptr)
			{
				delete it->second;
			}
		}
		for(auto it = consumables_.begin(); it != consumables_.end(); it++)
		{
			if(it->second != nullptr)
			{
				delete it->second;
			}
		}
		for(auto it = characters_.begin(); it != characters_.end(); it++)
		{
			if(it->second != nullptr)
			{
				delete it->second;
			}
		}
		exits_.clear();
		miscItems_.clear();
		equipables_.clear();
		consumables_.clear();
		characters_.clear();
	}


	// Returns available exits in this room.
	string Environment::directions() const 
	{
		stringstream ss;
		ss << "| ";
		for(auto exit = exits_.begin(); exit != exits_.end(); exit++)
		{
			ss << exit->first << " | ";
		}
		return ss.str();
	}
	
	// Returns a random exit in this room.
	string Environment::getRandomDirection() const
	{
		auto item = exits_.begin();
		std::advance( item, rand()%exits_.size());
		return item->first;
	}

	// Returns requirements for all directions leaving this room. 
	// Requirements are (at the moment) only item/Character names.
	// Requirements are ; separated.
	string Environment::roomRequirement() const
	{
		return directionRequirement_;
	}

	// Returns requirements for the direction from this room. 
	// Requirements are (at the moment) only item/Character names.
	string Environment::directionRequirement(string dir) const
	{
		std::transform(dir.begin(), dir.end(), dir.begin(), ::toupper); // To caps

		string delimiter = ";";
		size_t pos = 0;
		string currReq;
		string readLine = directionRequirement_;

		// Find the direction
		if ((pos = readLine.find(dir)) != std::string::npos) 
		{
			readLine.erase(0, pos);
			// Next find direction delimiter
			if((pos = readLine.find(delimiter)) != std::string::npos)
			{
				readLine.erase(pos, string::npos);
			}
				
			delimiter = ",";
			bool first = true;
			// Now it is time to check requirements
			while((pos = readLine.find(delimiter)) != std::string::npos)
			{
				// Remove direction from string, only interested in requirements
				if(first) 
				{
					readLine.erase(0, pos + delimiter.length());
					first = false;
					break;
				}
			}
		}
		return readLine;
	}

	// Returns a string containing contents of the current environment, with characters, items and exits.
	string Environment::getRoomContent(const Character * currentChar) const
	{
		stringstream ss;
		ss << "Characters: ";

		// Characters
		if(characters_.size() > 0)
		{
			for(auto character = characters_.begin(); character != characters_.end(); character++)
			{
				if(currentChar != character->second && character->second->isAlive())
				{
					ss << character->second->getName() << " (" << character->second->getType() << ")" << " ";
				}
			}
		}

		ss << endl;
		ss << "Items: ";

		// Items
		if(miscItems_.size() > 0 || equipables_.size() > 0 || consumables_.size() > 0)
		{

			if(miscItems_.size() > 0)
			{
				for(auto item = miscItems_.begin(); item != miscItems_.end(); item++)
				{
					ss << item->second->getName() << " (" << item->second->getType() << ")" << " ";
				}
			}

			if(equipables_.size() > 0)
			{
				for(auto item = equipables_.begin(); item != equipables_.end(); item++)
				{
					ss << item->second->getName() << " (" << item->second->getType() << ")" << " ";
				}
			}

			if(consumables_.size() > 0)
			{
				for(auto item = consumables_.begin(); item != consumables_.end(); item++)
				{
					ss << item->second->getName() << " (" << item->second->getType() << ")" << " ";
				}
			}
			ss << endl;
		}

		// Exits 
		ss << endl;
		ss << "Exits: " << directions() << endl;

		return ss.str();

	}

	// Returns the description of this room.
	string Environment::getDescription(const Character * currentChar) const
	{
		stringstream ss;

		ss << description_ << endl << endl;

		ss << getRoomContent(currentChar);
		
		return ss.str();
	}

	// Returns a detailed string representation of this room.
	// Used when saving the game information to file.
	string Environment::printEnvironment() const
	{
		std::stringstream ss;
		ss << type_ << "\n" << description_ << "\n";
		
		// Find and add all equipables by name
		bool first = true;
		for(auto it = equipables_.begin(); it != equipables_.end(); it++)
		{
			if(!first) ss << ",";
			ss << it->second->getName();
			first = false;
		}
		ss << "\n";
		
		// Find and add all misc items (ids)
		first = true;
		for(auto it = miscItems_.begin(); it != miscItems_.end(); it++)
		{
			if(!first) ss << ",";
			ss << it->second->getId();
			first = false;
		}
		ss << "\n";
		
		// Find and add all consumables (ids)
		first = true;
		for(auto it = consumables_.begin(); it != consumables_.end(); it++)
		{
			if(!first) ss << ",";
			ss << it->second->getId();
			first = false;
		}
		ss << "\n";
		
		ss << directionRequirement_ << "\n";
		
		return ss.str();
	}

	// Returns the neighboring environment to an exit direction.
	Environment* Environment::neighbor(string direction) const
	{
		std::transform(direction.begin(), direction.end(),direction.begin(), ::toupper); // To caps
		direction.erase(remove(direction.begin(),direction.end(),' '),direction.end());  // Trim whitespaces

		auto it = exits_.find(direction);
		if(it == exits_.end())
			return nullptr;
		else
			return it->second;
	}

	// Sets the description of this room.
	void Environment::setDescription(string description)
	{
		description_ = description;
	}

	// Returns the description (short) of this room.
	std::string Environment::getShortDescription() const
	{
		return description_;
	}

	// Sets the direction requirement string (all directions requirements) of this room.
	// E.g NORTH,key; SOUTH,ball
	//     NORTH,key,ball,sword; SOUTH,ball
	// Requirements are (at the moment) only item/Character names.
	// Requirements are ; separated.
	void Environment::setRoomRequirement(string requirement)
	{
		std::transform(requirement.begin(), requirement.end(),requirement.begin(), ::toupper); // To caps
		directionRequirement_ = requirement;
	}
	
	/// Checks if character has fulfilled the direction requirement(s).
		/// (i.e. if he/she has all of the items/characters listed)
	bool Environment::checkDirectionRequirement(string dir, Character &character)
	{
		std::transform(dir.begin(), dir.end(),dir.begin(), ::toupper); // To caps
		
		string delimiter = ";";
		size_t pos = 0;
		string currReq;
		string readLine = directionRequirement_;

		bool reqFound = true;
		
		readLine.erase(remove(readLine.begin(),readLine.end(),' '),readLine.end());  // Trim whitespaces
		// Find the direction
		if ((pos = readLine.find(dir)) != std::string::npos) 
		{
			readLine.erase(0, pos);
			reqFound = false;
			// Next find direction delimiter
			if((pos = readLine.find(delimiter)) != std::string::npos)
			{
				readLine.erase(pos, string::npos);
			}
				
			delimiter = ",";
			bool first = true;
			// Now it is time to check requirements
			while((pos = readLine.find(delimiter)) != std::string::npos)
			{
				// Remove direction from string, only interested in requirements
				if(first) 
				{
					readLine.erase(0, pos + delimiter.length());
					first = false;
				}
				else
				{
					currReq = readLine.substr(0, pos);
					// At least one item/character must match requirement
					if(character.getInvMiscItem(currReq) == nullptr && character.getInvEquipable(currReq) == nullptr && character.getInvConsumable(currReq) == nullptr && character.getInvCharacter(currReq) == nullptr)
					{
						return false;
					}
					readLine.erase(0, pos + delimiter.length());
				}
			}

			if(readLine.find(delimiter) == string::npos)
			{
				if(readLine != "")
				{
					// At least one item/character must match requirement
					if(character.getInvMiscItem(readLine) == nullptr && character.getInvEquipable(readLine) == nullptr && character.getInvConsumable(readLine) == nullptr && character.getInvCharacter(readLine) == nullptr)
					{
						return false;
					}
				}
			}
			reqFound = true;
		}
		return reqFound;
	}

	// Adds a neigboring environment to this room. Takes environment reference with direction.
	void Environment::addNeigbor(Environment* neighbor, string direction)
	{
		std::transform(direction.begin(), direction.end(),direction.begin(), ::toupper); // To caps
		direction.erase(remove(direction.begin(),direction.end(),' '),direction.end());  // Trim whitespaces

		exits_.insert(std::pair<string,Environment*>(direction,neighbor));
	}

	// Removes a neigboring environment to this room. Takes direction as parameter.
	void Environment::removeNeigbor(string direction)
	{
		std::transform(direction.begin(), direction.end(),direction.begin(), ::toupper); // To caps
		direction.erase(remove(direction.begin(),direction.end(),' '),direction.end());  // Trim whitespaces

		auto it = exits_.find(direction);
		if(it != exits_.end())
		{
			exits_.erase(direction);
		}
	}

	// Returns pointer to Item if it exists in this room. Otherwise returns nullptr.
	Item* Environment::getMiscItem(string key) const
	{
		std::transform(key.begin(), key.end(),key.begin(), ::toupper); // To caps
		key.erase(remove(key.begin(),key.end(),' '),key.end());  // Trim whitespaces

		auto it = miscItems_.find(key);
		if(it == miscItems_.end())
			return nullptr;
		else
			return it->second;
	}

	// Returns pointer to Equipable if it exists in this room. Otherwise returns nullptr.
	Equipable* Environment::getEquipable(string key) const
	{
		std::transform(key.begin(), key.end(),key.begin(), ::toupper); // To caps
		key.erase(remove(key.begin(),key.end(),' '),key.end());  // Trim whitespaces

		auto it = equipables_.find(key);
		if(it == equipables_.end())
			return nullptr;
		else
			return it->second;
	}

	// Returns pointer to Consumable if it exists in this room. Otherwise returns nullptr.
	Consumable* Environment::getConsumable(string key) const
	{
		std::transform(key.begin(), key.end(),key.begin(), ::toupper); // To caps
		key.erase(remove(key.begin(),key.end(),' '),key.end());  // Trim whitespaces

		auto it = consumables_.find(key);
		if(it == consumables_.end())
			return nullptr;
		else
			return it->second;
	}

	// Returns pointer to Character if it exists in this room. Otherwise returns nullptr.
	Character* Environment::getCharacter(string key) const
	{
		std::transform(key.begin(), key.end(),key.begin(), ::toupper); // To caps
		key.erase(remove(key.begin(),key.end(),' '),key.end());  // Trim whitespaces

		auto it = characters_.find(key);
		if(it == characters_.end() || !it->second->isAlive())
			return nullptr;
		else
			return it->second;
	}
	
	// Returns a random character.
	string Environment::getRandomCharacter() const
	{
		if(characters_.size() > 0)
		{
			auto character = characters_.begin();
			std::advance( character, rand()%characters_.size());
			if(character->second->isAlive())
			{
				return character->first;
			}
		}
		return "";
	}
	
	// Returns a random miscItem.
	string Environment::getRandomMiscItem() const
	{
		if(miscItems_.size() > 0)
		{
			auto item = miscItems_.begin();
			std::advance( item, rand()%miscItems_.size());
			return item->first;
		}
		return "";
	}
	
	// Returns a random consumable.
	string Environment::getRandomConsumable() const
	{
		if(consumables_.size() > 0)
		{
			auto item = consumables_.begin();
			std::advance( item, rand()%consumables_.size());
			return item->first;
		}
		return "";
	}
	
	// Returns a random equipable.
	string Environment::getRandomEquipable() const
	{
		if(equipables_.size() > 0)
		{
			auto item = equipables_.begin();
			std::advance( item, rand()%equipables_.size());
			return item->first;
		}
		return "";
	}
	
	// Returns true if the room contains a player (i.e. controllable character)
	bool Environment::containsPlayer() const
	{
		// Characters
		if(characters_.size() > 0)
		{
			for(auto character = characters_.begin(); character != characters_.end(); character++)
			{
				if(character->second->isAlive() && character->second->isControllable())
				{
					return true;
				}
			}
		}
		return false;
	}

	// Adds the pointer to Item to room. 
	void Environment::addMiscItem(Item* item)
	{
		string itemString = item->getName();
		std::transform(itemString.begin(), itemString.end(),itemString.begin(), ::toupper); // To caps
		itemString.erase(remove(itemString.begin(),itemString.end(),' '),itemString.end());  // Trim whitespaces

		miscItems_.insert(std::pair<string,Item*>(itemString,item));
	}

	// Adds the pointer to Equipable to room. 
	void Environment::addEquipable(Equipable* item)
	{		
		string itemString = item->getName();
		std::transform(itemString.begin(), itemString.end(),itemString.begin(), ::toupper); // To caps
		itemString.erase(remove(itemString.begin(),itemString.end(),' '),itemString.end());  // Trim whitespaces

		equipables_.insert(std::pair<string,Equipable*>(itemString,item));
	}

	// Adds the pointer to Consumable to room. 
	void Environment::addConsumable(Consumable* item)
	{
		string itemString = item->getName();
		std::transform(itemString.begin(), itemString.end(),itemString.begin(), ::toupper); // To caps
		itemString.erase(remove(itemString.begin(),itemString.end(),' '),itemString.end());  // Trim whitespaces

		consumables_.insert(std::pair<string,Consumable*>(itemString, item));
	}

	// Adds the pointer to Character to room. 
	void Environment::addCharacter(Character* character)
	{
		string characterString = character->getName();
		std::transform(characterString.begin(), characterString.end(),characterString.begin(), ::toupper); // To caps
		characterString.erase(remove(characterString.begin(),characterString.end(),' '),characterString.end());  // Trim whitespaces

		characters_.insert(std::pair<string,Character*>(characterString, character));
	}

	// Tries to remove matching pointer to Item from room (if it exists). 
	// Note: Will not delete the item itself, only remove it from the room.
	void Environment::removeMiscItem(string key)
	{
		std::transform(key.begin(), key.end(),key.begin(), ::toupper); // To caps
		key.erase(remove(key.begin(),key.end(),' '),key.end());  // Trim whitespaces

		auto it = miscItems_.find(key);
		if(it != miscItems_.end())
		{
			miscItems_.erase(it);
		}
	}

	// Tries to remove matching pointer to Equipable from room (if it exists). 
	// Note: Will not delete the item itself, only remove it from the room.
	void Environment::removeEquipable(string key)
	{
		std::transform(key.begin(), key.end(),key.begin(), ::toupper); // To caps
		key.erase(remove(key.begin(),key.end(),' '),key.end());  // Trim whitespaces

		auto it = equipables_.find(key);
		if(it != equipables_.end())
		{
			equipables_.erase(key);
		}
	}

	// Tries to remove matching pointer to Consumable from room (if it exists). 
	// Note: Will not delete the item itself, only remove it from the room.
	void Environment::removeConsumable(string key)
	{	
		std::transform(key.begin(), key.end(),key.begin(), ::toupper); // To caps
		key.erase(remove(key.begin(),key.end(),' '),key.end());  // Trim whitespaces
		auto it = consumables_.find(key);
		if(it != consumables_.end())
		{
			consumables_.erase(it);
		}
	}

	// Tries to remove matching pointer to Character from room (if it exists). 
	// Note: Will not delete the character itself, only remove it from the room.
	void Environment::removeCharacter(string key)
	{
		std::transform(key.begin(), key.end(),key.begin(), ::toupper); // To caps
		key.erase(remove(key.begin(),key.end(),' '),key.end());  // Trim whitespaces

		auto it = characters_.find(key);
		if(it != characters_.end())
		{
			characters_.erase(key);
		}
	}

	// Returns the multimap of characters for room
	std::map<string, Character*>& Environment::getCharacters()
	{
		return characters_;
	}

	// Returns the multimap of consumables for room
	std::multimap<std::string, Consumable*>& Environment::getConsumables()
	{
		return consumables_;
	}

	// Returns the multimap of equipables for room
	std::map<std::string, Equipable*>& Environment::getEquipables()
	{
		return equipables_;
	}

	// Returns the multimap of equipables for room
	std::multimap<std::string, Item*>& Environment::getMiscItems()
	{
		return miscItems_;
	}

	// Goes through all Characters and enables them to do actions for the next turn.
	void Environment::enableCharacterActions()
	{
		for(auto it = characters_.begin(); it != characters_.end(); it++)
		{
			if(it->second->isAlive())
			{
				it->second->setCanPerformAction(true);
			}
		}
	}

	// Returns the type of this room.
	std::string Environment::getType() const
	{
		return type_;
	}

	// Returns the map of all exits.
	std::map<std::string, Environment*>& Environment::getExits()
	{
		return exits_;
	}
}
