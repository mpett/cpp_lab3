/// Description: Header file for Character class.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-09
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
	// Different AI behaviors - used when calling actions() with a NPC.
	enum AI_BEHAVIOR
	{
		STAND_STILL     = 0,	// NPC will only stand still with this behavior              
		PACIFIST        , 		// NPC will move around and talk with this behavior          
		PASSIVE_LOOTER  , 		// NPC will move around and pickup items with this behavior  
		AGGRESSIVE_AMOVE ,		// NPC will move around, attacking anything it encounters    
		AMOVE,            		// NPC will move around at random, attacking any player character it encounters          
		AGGRESSIVE_GUARD, 		// NPC will remain in room but will attack anything it encounters (even other NPCs) 
		GUARD,            		// NPC will remain in room, will attack any player character it encounters          
		DO_NOT_HIT_ME   , 		// NPC will remain docile (and stand still) as long as nobody hits it - then it will go to AGGRESSIVE_AMOVE behavior.
		COLLECTOR       ,       // NPC will pickup anything it encounters (and have capacity to pickup) - that includes characters so beware! 
		CRAZY           ,  		// NPC will try to do all sorts of actions with all sorts of objects
		
	};
	
	// Forward declaration(s) solves circular dependancies
	class Consumable;
	class Environment;
	class Equipable;

	class Character
	{
	public:
		// Destructor to character.
		 virtual ~Character();

		 // Constructor for character. Used for loading a character.
		 Character(bool controllable, std::string name, std::string type, double weight, int strength, int currentHealth, int maxHealth, int minDamage, int maxDamage, int currentCarried, int maxCarried, Equipable * currentArmor, Equipable * currentWeapon, bool canPerformAction, bool atNewRoom, Environment* room) : 
			 name_(name), type_(type), weight_(weight), strength_(strength), currentHealth_(currentHealth), maxHealth_(maxHealth), minDamage_(minDamage), maxDamage_(maxDamage), currentCarried_(currentCarried), maxCarried_(maxCarried), currentEquippedArmor_(currentArmor), currentEquippedWeapon_(currentWeapon), canPerformAction_(canPerformAction), controllable_(controllable), atNewRoom_(atNewRoom) 
		 {
			 aiBehavior_  = AI_BEHAVIOR::STAND_STILL; // Default
			 currentRoom_ = nullptr;
			 setCurrentRoom(room, false);
		 }
		 
		 // Constructor for character. 
		 Character(bool controllable, std::string name, std::string type, double weight, int strength, int currentHealth, int maxHealth, int minDamage, int maxDamage, int currentCarried, int maxCarried, Environment* room) : 
			 name_(name), type_(type), weight_(weight), strength_(strength), currentHealth_(currentHealth), maxHealth_(maxHealth), minDamage_(minDamage), maxDamage_(maxDamage), currentCarried_(currentCarried), maxCarried_(maxCarried), controllable_(controllable) 
		 { 
			 aiBehavior_ = AI_BEHAVIOR::STAND_STILL; // Default
			 currentEquippedArmor_  = nullptr;
			 currentEquippedWeapon_ = nullptr;
			 currentRoom_           = nullptr;

			 setCurrentRoom(room, false);
		 }

		// Drops the loot of the character into the room (if dropLoot is specified as true).
		// NOTE: Character must be removed from room and deleted in GameEngine game loop.
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

		// Returns a detailed string representation of this character.
		// Used when saving the game information to file.
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
		bool isControllable() const;

		// Prints a help message for corresponding character.
		virtual bool help(std::string);

		// Prints the contents of the inventory for controllable character.
		bool showInventory(std::string);

		// Prints the stats (i.e. strength, health etc.) of the controllable character.
		bool showStats(std::string);

		// Equips an Equipable matching text string.
		bool equip(std::string);

		// Unequips an Equipable matching text string.
		bool unequip(std::string);

		// Does an action with the character. 
		// An action can be anything from fight, go, talk etc.
		// This should typically only be called on NPCs.
		virtual void action();

		// Adds an sentence to things that the character can say when talked to.
		void addThingToSay(std::string sentence);

		// Equips an armor. If another armor is currently equipped, then this will be unequipped and added to the inventory.
		void equipArmor(Equipable* armor);

		// Unequips the armor.
		void unequipArmor();

		// Equips a weapon. If another weapon is currently equipped, then this will be unequipped and added to the inventory.
		void equipWeapon(Equipable* weapon);

		// Unequips the weapon.
		void unequipWeapon();

		// Tries to move from the room in a certain direction.
		bool go(std::string direction);

		// Performs an attack on the specified character.
		virtual bool attack(std::string character);

		// Returns a sentence that the character will say.
		virtual void talk();

		// Initiates a conversation with a character.
		bool talkTo(std::string);

		// Attempt to consume a consumable item that the character owns.
		bool consume(std::string);

		// Tries to hit a character with currently equipped weapon.
		virtual int fight(Character& character);

		// Damages the character with a certian amount of points.
		virtual void takeDamage(int damage);

		// Sets if the character is at a new room.
		void setAtNewRoom(bool val);
		
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
		
		// Sets the AI Behavior to be used when character is not controlled by a player.
		void setAIBehavior(int behavior);

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
		
		/** NPC RELEVANT FUNCTIONS **/
		// Tries to move in a random direction.
		bool tryRandomMove();
		
		// Tries to say something random.
		bool tryRandomSpeak();
		
		// Tries to attack a random target (including NPC).
		bool tryRandomAttackAll();
		
		// Tries to attack a "random" character (excluding other NPCs).
		bool tryRandomAttack();
		
		// Tries to pickup a random item in the room.
		bool tryPickupRandomItem();
		
		// Tries to pickup a random character in the room.
		bool tryPickupRandomCharacter();
		
		// Consumes a random consumable in inventory (if such exist).
		bool tryRandomConsume();

		// Returns the multimap of characters for character
		std::map<std::string, Character*>& getCharacters();

		// Returns the multimap of consumables for character
		std::multimap<std::string, Consumable*>& getConsumables();

		// Returns the multimap of equipables for character
		std::map<std::string, Equipable*>& getEquipables();

		// Returns the multimap of misc items for character
		std::multimap<std::string, Item*>& getMiscItems();
		
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
		int aiBehavior_; // Only used if uncontrollable

		// Inventory
		std::multimap<std::string, Item*> carriedMiscItems_;
		std::map<std::string, Equipable*> carriedEquipables_;
		std::multimap<std::string, Consumable*> carriedConsumables_;
		std::map<std::string, Character*> carriedCharacters_;
	};
}

