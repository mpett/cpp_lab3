/// Description: Header file for Character class.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-24
#pragma once
#include "Item.h"
#include "Equipable.h"
#include "Consumable.h"
#include "Environment.h"
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iostream>
using std::cout;
using std::endl;

namespace GameLogic
{
	// Forward declaration(s) solves circular dependancies
	class Consumable;
	class Environment;
	class Equipable;

	class Character
	{
	public:
		// Destructor to character.
		 ~Character();

		 // Constructor for character. 
		 Character(bool controllable, std::string name, std::string type, double weight, int strength, int currentHealth, int maxHealth, int minDamage, int maxDamage, int currentCarried, int maxCarried, Environment* room) : 
			 name_(name),type_(type), weight_(weight), strength_(strength),currentHealth_(currentHealth),maxHealth_(maxHealth),minDamage_(minDamage),maxDamage_(maxDamage),currentCarried_(currentCarried), maxCarried_(maxCarried), controllable_(controllable) 
		 { 
			 currentEquippedArmor_  = nullptr;
			 currentEquippedWeapon_ = nullptr;
			 currentRoom_           = nullptr;

			 setCurrentRoom(room, false);
		 }

		// Removes the character from its room (drops its items as loot if specified) and then deletes it by running its destructor.
		virtual void killCharacter(bool dropLoot);

		// Returns the name of the character.
		std::string getName() const;

		// Returns the type of the character.
		std::string getType() const;

		// Returns weight of character inclduing weight of his carried items.
		double getTotalWeight() const;

		// Returns weight of character excluding weight of his carried items.
		double getCharacterWeight() const;

		// Prints a description of the current room; lists all characters, items and exits (in the room).
		bool look(std::string);

		// Returns a string with detailed information about this character.
		virtual std::string printCharacter() const;

		// Returns if the Character is in a new room
		bool getInNewRoom() const;

		// Returns the strength of the character.
		int getStrength() const;

		// Returns the current health of the character.
		int getCurrentHealth() const;

		// Returns the maximum health of the character.
		int getMaxHealth() const;

		// Returns the maximum damage of the character.
		int getMaxDamage() const;

		// Returns the minimum damage of the character.
		int getMinDamage() const;

		// Returns a pointer to current room that character is in.
		Environment* getCurrentRoom() const;

		// Returns a value indicating if character can perform an action.
		bool getCanPerformAction() const;

		// Returns the amount of weight currently carried.
		double getCurrentCarried() const;

		// Returns the maximum weight that the character is able to carry.
		double getMaxCarried() const;

		// Returns true if character is alive.
		virtual bool isAlive() const;

		// Returns true if character is controllable by user.
		bool isControllable() const 
		{
			return controllable_;
		}

		// Prints a help message for corresponding character.
		virtual bool help(std::string) = 0;

		// Does an action with the character. 
		// An action can be anything from fight, go, talk etc.
		virtual void action() = 0;

		// Adds an sentence to things that the character can say when talked to.
		void addThingToSay(std::string sentence);

		// Equips an armor. If another armor is currently equipped, then this will be unequipped and added to the inventory.
		bool equipArmor(Equipable* armor);

		// Equips a weapon. If another weapon is currently equipped, then this will be unequipped and added to the inventory.
		bool equipWeapon(Equipable* weapon);

		// Tries to move from the room in a certain direction.
		bool go(std::string direction);

		// Returns a sentence that the character will say.
		virtual std::string talk();

		// Initiates a conversation with a character.
		std::string talkTo(Character& character);

		// Tries to hit a character with currently equipped weapon.
		virtual int fight(Character& character);

		// Damages the character with a certian amount of points.
		virtual void takeDamage(int damage);

		// Sets the weight of the character.
		void setCharacterWeight(double weight);

		// Sets the strength of the character to input.
		void setStrength(int strength);

		// Sets the currenth health of the character to input.
		void setCurrentHealth(int health);

		// Sets the maximum health of the character to input.
		void setMaxHealth(int health);

		// Sets the maximum damage that the character can accomplish to input.
		void setMaxDamage(int damage);

		// Sets the minimum damage that the character can accomplish to input.
		void setMinDamage(int damage);

		// Sets if the Character is in a new room.
		void setInNewRoom(bool value);

		// Sets the amount of weight currently carried.
		void setCurrentCarried(double carried);

		// Sets the maximum weight that can be carried by character to input.
		void setMaxCarried(double carried);

		// Sets which room that the character is currently in.
		// (Removes character from previous room and adds it to the new room)
		//
		// leftRoom : True if character did leave its previous room (i.e. did not get placed at new room).
		void setCurrentRoom(Environment* room, bool leftRoom);

		// Sets if the Character can perform an action.
		void setCanPerformAction(bool value);

		// Drops the item with the name specified in string.
		bool drop(std::string itemstring);

		// Tries to pickup an item with the name specified in string
		bool pickup(std::string itemString);

		// Returns pointer to Item if it exist on this character. Otherwise returns nullptr.
		// If multiple entries exist, will return first occurence
		Item* getInvMiscItem(std::string key) const;

		// Returns pointer to Equipable if it exist on this character. Otherwise returns nullptr.
		// If multiple entries exist, will return first occurence
		Equipable* getInvEquipable(std::string key) const;

		// Returns pointer to Consumable if it exist on this character. Otherwise returns nullptr.
		// If multiple entries exist, will return first occurence
		Consumable* getInvConsumable(std::string key) const;

		// Returns pointer to Character if it exist on this character. Otherwise returns nullptr.
		// If multiple entries exist, will return first occurence
		Character* getInvCharacter(std::string key) const;

		// Adds the pointer to Item to inventory. 
		void addInvMiscItem(Item* item);

		// Adds the pointer to Equipable to inventory. 
		void addInvEquipable(Equipable* item);

		// Adds the pointer to Consumable to inventory. 
		void addInvConsumable(Consumable* item);

		// Adds the pointer to Character to inventory. 
		void addInvCharacter(Character* character);

		// Tries to remove the first occurence of matching pointer to Item from inventory (if it exist). 
		// Note: Will not delete the item itself, only remove it from the inventory.
		void removeInvMiscItem(std::string key);

		// Tries to remove the first occurence of matching pointer to Equipable from inventory (if it exist). 
		// Note: Will not delete the item itself, only remove it from the inventory.
		void removeInvEquipable(std::string key);

		// Tries to remove the first occurence of matching pointer to Consumable from inventory (if it exist). 
		// Note: Will not delete the item itself, only remove it from the inventory.
		void removeInvConsumable(std::string key);

		// Tries to remove the first occurence of matching pointer to Character from inventory (if it exist). 
		// Note: Will not delete the character itself, only remove it from the inventory.
		void removeInvCharacter(std::string key);

		// Assigns left-hand Character to right-hand Character.
		virtual Character& operator=(const Character& env);
		
	protected:
		std::string name_;
		std::string type_;
		double weight_;
		int strength_;
		int currentHealth_;
		int maxHealth_;
		int minDamage_;
		int maxDamage_;
		double currentCarried_;
		double maxCarried_;
		Equipable * currentEquippedArmor_;
		Equipable * currentEquippedWeapon_;
		std::vector<std::string> thingsToSay_;
		Environment * currentRoom_;
		bool canPerformAction_;
		bool controllable_;
		bool atNewRoom_;

		// Inventory
		std::multimap<std::string, Item*> carriedMiscItems_;
		std::multimap<std::string, Equipable*> carriedEquipables_;
		std::multimap<std::string, Consumable*> carriedConsumables_;
		std::map<std::string, Character*> carriedCharacters_;
	};
}

