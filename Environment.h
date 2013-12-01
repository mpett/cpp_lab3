/// Description: Header file for Environment class.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-24
#pragma once
#include "Equipable.h"
#include "Consumable.h"
#include "Character.h"
#include <string>
#include <vector>
#include <map>

namespace GameLogic
{
	class Character; // Forward declaration solves circular dependancies
	class Consumable;
	class Equipable;

	class Environment
	{
	public:
		// Destructor.
		~Environment();

		// Constructor.
		Environment(std::string description) : description_(description){}

		// Returns available exits in this room.
		std::string directions() const;

		// Returns requirements for entering this room. Requirements are space separated.
		std::string roomRequirement() const;

		// Returns a string containing contents of the current environment, with characters, items and exits.
		std::string getRoomContent(const Character * character) const;

		// Returns the description of this room.
		std::string getDescription(const Character * currentChar) const;

		// Returns a string with detailed information about the contents of this room.
		virtual std::string printEnvironment() const;

		// Returns the neighboring environment to an exit direction.
		Environment* neighbor(std::string direction) const;

		// Sets the description of this room.
		void setDescription(std::string description);

		// Sets room requirement string of this room.
		void setRoomRequirement(std::string requirement);

		// Adds a neigboring environment to this room. Takes environment reference with direction.
		void addNeigbor(Environment* neighbor, std::string direction);

		// Removes a neigboring environment to this room. Takes direction as parameter.
		void removeNeigbor(std::string direction);

		//// Process event that may occur upon entering environment.
		virtual void onEntry(Character& character) = 0;

		//// Process event that may occur when leaving environment.
		virtual void onLeave(Character& character) = 0;

		//// Process event that may occur while moving in room.
		virtual void turnEvent(Character& character) = 0;

		// Returns pointer to Item if it exists in this room. Otherwise returns nullptr.
		Item* getMiscItem(std::string key) const;

		// Returns pointer to Equipable if it exists in this room. Otherwise returns nullptr.
		Equipable* getEquipable(std::string key) const;

		// Returns pointer to Consumable if it exists in this room. Otherwise returns nullptr.
		Consumable* getConsumable(std::string key) const;

		// Returns pointer to Character if it exists in this room. Otherwise returns nullptr.
		Character* getCharacter(std::string key) const;

		// Adds the pointer to Item to room. 
		void addMiscItem(Item* item);

		// Adds the pointer to Equipable to room. 
		void addEquipable(Equipable* item);

		// Adds the pointer to Consumable to room. 
		void addConsumable(Consumable* item);

		// Adds the pointer to Character to room. 
		void addCharacter(Character* character);

		// Tries to remove matching pointer to Item from room (if it exists). 
		// Note: Will not delete the item itself, only remove it from the room.
		void removeMiscItem(std::string key);

		// Tries to remove matching pointer to Equipable from room (if it exists). 
		// Note: Will not delete the item itself, only remove it from the room.
		void removeEquipable(std::string key);

		// Tries to remove matching pointer to Consumable from room (if it exists). 
		// Note: Will not delete the item itself, only remove it from the room.
		void removeConsumable(std::string key);

		// Tries to remove matching pointer to Character from room (if it exists). 
		// Note: Will not delete the character itself, only remove it from the room.
		void removeCharacter(std::string key);

		// Returns the multimap of characters for room
		std::map<std::string, Character*>& getCharacters();

		// Goes through all Characters and enables them to do actions for the next turn.
		void enableCharacterActions();

		// Assigns left-hand Environment to right-hand Environment.
		virtual Environment& operator=(const Environment& env);
		
	protected:
		std::string description_;
		std::string roomRequirement_;
		std::map<std::string, Environment*> exits_;

		// Inventory
		std::multimap<std::string, Item*> miscItems_;
		std::map<std::string, Equipable*> equipables_;
		std::multimap<std::string, Consumable*> consumables_;
		std::map<std::string, Character*> characters_;
	};
}

