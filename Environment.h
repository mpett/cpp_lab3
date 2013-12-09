/// Description: Header file for Environment class.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-09
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
		virtual ~Environment();

		// Constructor.
		Environment(std::string description, std::string type) : type_(type), description_(description){}

		// Returns available exits in this room.
		std::string directions() const;

		// Returns a random exit in this room.
		std::string getRandomDirection() const;
		
		/// Checks if character has fulfilled the direction requirement(s).
		/// (i.e. if he/she has all of the items/characters listed)
		bool checkDirectionRequirement(std::string dir, Character &character);
		
		// Returns requirements for all directions leaving this room. 
		// Requirements are (at the moment) only item/Character names.
		// Requirements are ; separated.
		std::string roomRequirement() const;

		// Returns requirements for the direction from this room. 
		// Requirements are (at the moment) only item/Character names.
		std::string directionRequirement(std::string) const;

		// Returns a string containing contents of the current environment, with characters, items and exits.
		std::string getRoomContent(const Character * character) const;

		// Returns the description of this room.
		std::string getDescription(const Character * currentChar) const;

		// Returns a detailed string representation of this room.
		// Used when saving the game information to file.
		virtual std::string printEnvironment() const;

		// Returns the neighboring environment to an exit direction.
		Environment* neighbor(std::string direction) const;

		// Sets the description of this room.
		void setDescription(std::string description);

		// Returns the description (short) of this room.
		std::string getShortDescription() const;

		// Sets the direction requirement string (all directions requirements) of this room.
		// E.g NORTH,key; SOUTH,ball
		//     NORTH,key,ball,sword; SOUTH,ball
		// Requirements are (at the moment) only item/Character names.
		// Requirements are ; separated.
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
		
		// Returns a random character.
		std::string getRandomCharacter() const;
		
		// Returns a random miscItem.
		std::string getRandomMiscItem() const;
		
		// Returns a random consumable.
		std::string getRandomConsumable() const;
		
		// Returns a random equipable.
	    std::string getRandomEquipable() const;
		
		// Returns true if the room with the Character contains another player (i.e. controllable character)
		bool containsPlayer() const;

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

		// Returns the multimap of consumables for room
		std::multimap<std::string, Consumable*>& getConsumables();

		// Returns the multimap of equipables for room
		std::map<std::string, Equipable*>& getEquipables();

		// Returns the multimap of misc items for room
		std::multimap<std::string, Item*>& getMiscItems();

		// Goes through all Characters and enables them to do actions for the next turn.
		void enableCharacterActions();

		// Returns the type of this room.
		std::string getType() const;

		// Returns the map of all exits.
		std::map<std::string, Environment*>& getExits();
		
	protected:
		std::string type_;
		std::string description_;
		std::string directionRequirement_;
		std::map<std::string, Environment*> exits_;

		// Inventory
		std::multimap<std::string, Item*> miscItems_;
		std::map<std::string, Equipable*> equipables_;
		std::multimap<std::string, Consumable*> consumables_;
		std::map<std::string, Character*> characters_;
	};
}

