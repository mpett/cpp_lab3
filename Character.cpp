/// Description: A Character is able to move within the world and interact with it.
///              It can be controllable (user can issue commands to it) or un-controllable (command is randomly generated).
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-12-07
#include <algorithm>
#include <sstream>
#include "Character.h"
#include "GameEngine.h"
using std::string;

namespace GameLogic
{		
		// Destructor to character.
		Character:: ~Character()
		{
			if(currentEquippedArmor_ != nullptr)
			{
				currentEquippedArmor_ = nullptr;
			}
			if(currentEquippedWeapon_ != nullptr)
			{
				currentEquippedWeapon_ = nullptr;
			}
		
			for(auto it = carriedMiscItems_.begin(); it != carriedMiscItems_.end(); it++)
			{
				if(it->second != nullptr)
				{
					delete it->second;
				}
			}
			for(auto it = carriedEquipables_.begin(); it != carriedEquipables_.end(); it++)
			{
				if(it->second != nullptr)
				{
					delete it->second;
				}
			}
			for(auto it = carriedConsumables_.begin(); it != carriedConsumables_.end(); it++)
			{
				if(it->second != nullptr)
				{
					delete it->second;
				}
			}
			for(auto it = carriedCharacters_.begin(); it != carriedCharacters_.end(); it++)
			{
				if(it->second != nullptr)
				{
					delete it->second;
				}
			}
			carriedMiscItems_.clear();
			carriedEquipables_.clear();
			carriedConsumables_.clear();
			carriedCharacters_.clear();
		}

		// Drops the loot of the character into the room (if dropLoot is specified as true).
		// NOTE: Character must be removed from room and deleted in GameEngine game loop.
		void Character::killCharacter(bool dropLoot)
		{
			if(dropLoot)
			{
				// Drop carriedMiscItems
				for(auto it = carriedMiscItems_.begin(); it != carriedMiscItems_.end(); it++)
				{
					if(it->second != nullptr)
					{
						currentRoom_->addMiscItem(it->second);
						it->second = nullptr; // Make sure we do not delete this item since it is now in the room
					}
				}
				
				// Drop carrieEquipables
				for(auto it = carriedEquipables_.begin(); it != carriedEquipables_.end(); it++)
				{
					if(it->second != nullptr)
					{
						currentRoom_->addEquipable(it->second);
						it->second = nullptr; // Make sure we do not delete this item since it is now in the room
					}
				}

				// Drop equipped armor & weapon
				if(currentEquippedArmor_ != nullptr)
				{
					currentEquippedArmor_->onUnequip(*this);
					unequipArmor(); // Make sure we do not delete this item since it is now in the room
				}
				if(currentEquippedWeapon_ != nullptr)
				{
					currentEquippedWeapon_->onUnequip(*this);
					unequipWeapon(); // Make sure we do not delete this item since it is now in the room
				}
			
				// Drop carriedConsumables
				for(auto it = carriedConsumables_.begin(); it != carriedConsumables_.end(); it++)
				{
					if(it->second != nullptr)
					{
						currentRoom_->addConsumable(it->second);
						it->second = nullptr; // Make sure we do not delete this item since it is now in the room
					}
				}
				// Drop carriedCharacters
				for(auto it = carriedCharacters_.begin(); it != carriedCharacters_.end(); it++)
				{
					if(it->second != nullptr)
					{
						currentRoom_->addCharacter(it->second);
						it->second = nullptr; // Make sure we do not delete this item since it is now in the room
					}
				}
			}
		}

		// Returns the name of the character.
		string Character::getName() const
		{
			return name_;
		}

		// Returns the type of the character.
		string Character::getType() const
		{
			return type_;
		}

		// Returns weight of character inclduing weight of his carried items.
		double Character::getTotalWeight() const
		{
			return weight_ + currentCarried_;
		}

		// Returns weight of character excluding weight of his carried items.
		double Character::getCharacterWeight() const
		{
			return weight_;
		}

		// Prints the contents of the inventory for controllable character.
		bool Character::showInventory(string)
		{
			if(controllable_)
			{
				std::stringstream ss;
				ss << "---------------INVENTORY--------------\n" << endl;
				ss.precision(2);
				ss << std::fixed;

				ss << "Weight capacity: " << currentCarried_ << "/" << maxCarried_ << " Kg" << endl << endl;

				ss << "Characters:\n";
				// Characters
				if(carriedCharacters_.size() > 0)
				{
					for(auto character = carriedCharacters_.begin(); character != carriedCharacters_.end(); character++)
					{
						ss << character->second->getName() << " (" << character->second->getType() << ")" << "\t" << character->second->getTotalWeight() << "Kg" << endl;
					}
				}

				ss << endl;
				ss << "Items:\n";

				// Items
				if(carriedMiscItems_.size() > 0 || carriedEquipables_.size() > 0 || carriedConsumables_.size() > 0)
				{

					if(carriedMiscItems_.size() > 0)
					{
						for(auto item = carriedMiscItems_.begin(); item != carriedMiscItems_.end(); item++)
						{
							ss << item->second->getName() << " (" << item->second->getType() << ")" << "\t" << item->second->getWeight() << "Kg\t" << item->second->getPrice() << "$" << endl;
						}
					}

					if(carriedEquipables_.size() > 0)
					{
						for(auto item = carriedEquipables_.begin(); item != carriedEquipables_.end(); item++)
						{
							ss << item->second->getName() << " (" << item->second->getType() << ")" << "\t" << item->second->getWeight() << "Kg\t" << item->second->getPrice() << "$\t" << ((item->second->isEquipped()) ? "EQUIPPED" : "") << endl;
						}
					}

					if(carriedConsumables_.size() > 0)
					{
						for(auto item = carriedConsumables_.begin(); item != carriedConsumables_.end(); item++)
						{
							ss << item->second->getName() << " (" << item->second->getType() << ")" << "\t" << item->second->getWeight() << "Kg\t" << item->second->getPrice() << "$\t" << "Charges: " << item->second->getNrUses() << endl;
						}
					}
				}
				ss << endl;
				ss << "--------------------------------------\n";
				cout << ss.str();
			}
			return false;
		}

		// Prints a general help message.
		bool Character::help(std::string)
		{
			cout << GameLogic::helpText << endl;
			return false;
		}

		// Prints the stats (i.e. strength, health etc.) of the controllable character.
		bool Character::showStats(string)
		{
			if(controllable_)
			{
				std::stringstream ss;
				ss << "-----------------STATS----------------\n" << endl;
				ss.precision(2);
				ss << std::fixed;

				ss << "Name: " << name_ << endl;
				ss << "Health: " << currentHealth_ << "/" << maxHealth_ << " HP" << endl;
				ss << "Strength: " << strength_ << endl;
				ss << "Damage: " << minDamage_ << "-" << maxDamage_ << " DMG" << endl;
				ss << "Equipped Weapon: ";

				// Weapon
				if(currentEquippedWeapon_ == nullptr)
				{
					ss << "Unarmed";
				}
				else
				{
					ss << currentEquippedWeapon_->getName() << " (" << currentEquippedWeapon_->getType() << ")";
				}
				ss << endl;

				ss << "Equipped Armor: ";

				// Weapon
				if(currentEquippedArmor_ == nullptr)
				{
					ss << "Unarmored";
				}
				else
				{
					ss << currentEquippedArmor_->getName() << " (" << currentEquippedArmor_->getType() << ")" << endl;
				}
				ss << endl;

				ss << "Weight capacity: " << currentCarried_ << "/" << maxCarried_ << " Kg" << endl;


				ss << "--------------------------------------\n";
				cout << ss.str();
			}
			return false;
		}

		// Equips an Equipable matching text string.
		bool Character::equip(string itemString)
		{
			std::transform(itemString.begin(), itemString.end(),itemString.begin(), ::toupper); // To caps
			itemString.erase(remove(itemString.begin(),itemString.end(),' '),itemString.end());  // Trim whitespaces

			Equipable *foundEquipable = getInvEquipable(itemString);
			if(foundEquipable != nullptr) 
			{
				if(!foundEquipable->requirementCheck(*this))
				{
					if(controllable_)
					{
						cout << "In order to equip this item, you need to pass these requirements:\n" << foundEquipable->getRequirementDesc() << endl;
					}
					return false;
				}
				else
				{
					if(controllable_)
					{
						cout << "You equipped " << foundEquipable->getName() << "." << endl;
					}
					foundEquipable->onEquip(*this);
					return true;
				}
			}

			if(controllable_)
			{
				cout << "You cannot equip something which you do not own!" << endl;
			}

			return false;
		}

		// Unequips an Equipable matching text string.
		bool Character::unequip(string itemString)
		{
			std::transform(itemString.begin(), itemString.end(),itemString.begin(), ::toupper); // To caps
			itemString.erase(remove(itemString.begin(),itemString.end(),' '),itemString.end());  // Trim whitespaces

			Equipable *foundEquipable = getInvEquipable(itemString);
			if(foundEquipable != nullptr) 
			{
				if(controllable_)
				{
					cout << "You unequipped " << foundEquipable->getName() << "." << endl;
				}

				foundEquipable->onUnequip(*this);
				return true;
			}

			if(controllable_)
			{
				cout << "You cannot equip something which you do not own!" << endl;
			}

			return false;
		}

		// Prints a description of the current room; lists all characters, items and exits (in the room).
		bool Character::look(string)
		{
			if(controllable_)
			{
				cout << currentRoom_->getDescription(this);
			}

			return false;
		}

		// Returns a string with detailed information about this character.
		string Character::printCharacter() const
		{
			std::stringstream ss;
			ss << name_ << "\n" << type_ << "\n" << strength_ << "\n" << weight_ << "\n" << currentHealth_ << "\n" << maxHealth_ << "\n" << minDamage_ << "\n" << maxDamage_ << "\n" << currentCarried_ << "\n" << maxCarried_ << "\n";
			
			if(currentEquippedArmor_ != nullptr)
			{
				ss << currentEquippedArmor_->getName();
			}
			ss << "\n";
			
			if(currentEquippedWeapon_ != nullptr)
			{
				ss << currentEquippedWeapon_->getName();
			}
			ss << "\n";
			
			// Things to say (; seperated)
			bool first = true;
			for(auto it = thingsToSay_.begin(); it != thingsToSay_.end(); it++)
			{
				if(!first) ss << ";";
				ss << (*it);
			}
			ss << "\n";
			
			ss << ((canPerformAction_)?1:0) << "\n" << ((controllable_)?1:0) << "\n" << ((atNewRoom_)?1:0) << "\n" << aiBehavior_ << "\n";
			
			// Find and add all misc items (ids)
			first = true;
			for(auto it = carriedMiscItems_.begin(); it != carriedMiscItems_.end(); it++)
			{
				if(!first) ss << ",";
				ss << it->second->getId();
				first = false;
			}
			ss << "\n";
			
			// Find and add all consumables (ids)
			first = true;
			for(auto it = carriedConsumables_.begin(); it != carriedConsumables_.end(); it++)
			{
				if(!first) ss << ",";
				ss << it->second->getId();
				first = false;
			}
			ss << "\n";
			
			// Find and add all equipables by name
			first = true;
			for(auto it = carriedEquipables_.begin(); it != carriedEquipables_.end(); it++)
			{
				if(!first) ss << ",";
				ss << it->second->getName();
				first = false;
			}
			ss << "\n";	
			
			return ss.str();
		}

		// Returns if the Character is in a new room
		bool Character::getInNewRoom() const
		{
			return atNewRoom_;
		}

		// Returns the strength of the character.
		int Character::getStrength() const
		{
			return strength_;
		}

		// Returns the current health of the character.
		int Character::getCurrentHealth() const
		{
			return currentHealth_;
		}

		// Returns the maximum health of the character.
		int Character::getMaxHealth() const
		{
			return maxHealth_;
		}

		// Returns the maximum damage of the character.
		int Character::getMaxDamage() const
		{
			return maxDamage_;
		}

		// Returns the minimum damage of the character.
		int Character::getMinDamage() const
		{
			return minDamage_;
		}

		// Returns a pointer to current room that character is in.
		Environment* Character::getCurrentRoom() const
		{
			return currentRoom_;
		}

		// Returns a value indicating if character can perform an action.
		bool Character::getCanPerformAction() const
		{
			return canPerformAction_;
		}

		// Returns the amount of weight currently carried.
		double Character::getCurrentCarried() const
		{
			return currentCarried_;
		}

		// Returns the maximum weight that the character is able to carry.
		double Character::getMaxCarried() const
		{
			return maxCarried_;
		}

		// Returns true if character is alive.
		bool Character::isAlive() const
		{
			return (currentHealth_ > 0);
		}

		// Adds an sentence to things that the character can say when talked to.
		void Character::addThingToSay(string sentence)
		{
			thingsToSay_.push_back(sentence);
		}

		// Equips an armor. If another armor is currently equipped, then this will be unequipped and added to the inventory.
		void Character::equipArmor(Equipable* armor)
		{
			if(currentEquippedArmor_ != nullptr)
			{
				currentEquippedArmor_->onUnequip(*this);
			}
			currentEquippedArmor_ = armor;
		}

		// Unequips the armor.
		void Character::unequipArmor()
		{
			currentEquippedArmor_ = nullptr;
		}

		// Equips a weapon. If another weapon is currently equipped, then this will be unequipped and added to the inventory.
		void Character::equipWeapon(Equipable* weapon)
		{
			if(currentEquippedWeapon_ != nullptr)
			{
				currentEquippedWeapon_->onUnequip(*this);
			}
			currentEquippedWeapon_ = weapon;
		}

		// Unequips the weapon.
		void Character::unequipWeapon()
		{
			currentEquippedWeapon_ = nullptr;
		}

		// Tries to move from the room in a certain direction. Returns true if successful.
		bool Character::go(string direction)
		{
			string originalDirection = direction;											 // Retain original message for output

			Environment * nextRoom = currentRoom_->neighbor(direction);
			if(nextRoom != nullptr)
			{
				if(nextRoom->checkRoomRequirement(*this))
				{
					setCurrentRoom(nextRoom, true); // TODO: Is ok?
					if(controllable_) cout << "You went " << originalDirection << endl;
					currentRoom_->onEntry(*this);
					return true;
				}
				else
				{
					if(controllable_) 
					{
						cout << "You need to have the following items/characters to enter:" << endl;
						
						string delimiter = ";";
						size_t pos = 0;
						string currReq;
						string readLine = nextRoom->roomRequirement();
						while ((pos = readLine.find(delimiter)) != std::string::npos) 
						{
							currReq = readLine.substr(0, pos);
							cout << currReq << endl;
							readLine.erase(0, pos + delimiter.length());
						}
						if(readLine.find(delimiter) == string::npos)
						{
							if(readLine != "")
							{
								currReq = readLine;
								cout << currReq << endl;
							}
						}
					}				
					return false;
				}

			}
			else
			{
				if(controllable_) cout << "You cannot go " << originalDirection << endl;
				return false;
			}
		}
		
		// Does an action with the character. 
		// An action can be anything from fight, go, talk etc.
		// This should typically only be called on NPCs.
		void Character::action()
		{
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
					// Only attack if there are more than 1 in room (i.e. not alone)
					if(currentRoom_->containsPlayer())
					{
						tryRandomAttack();
					}
					// Time to get a movin'
					else if(dice > 40)
					{
						tryRandomMove();
					}
					break;
				}
				// NPC will remain in room but will attack anything it encounters (even other NPCs)
				case(AI_BEHAVIOR::AGGRESSIVE_GUARD):
				{
					tryRandomAttackAll();
					break;
				}
				// NPC will remain in room, will attack any player character it encounters
				case(AI_BEHAVIOR::GUARD):
				{
					if(currentRoom_->containsPlayer())
					{
						tryRandomAttack();
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
					// Move
					if(dice < 16)
					{
						tryRandomMove();
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
						tryRandomAttackAll();
					}
					// Speak
					else if(80)
					{
						tryRandomSpeak();
					}
					break;
				}
			}
		}

		// Performs an attack on the specified character.
		bool Character::attack(string character)
		{
			Character * attackedChar = currentRoom_->getCharacter(character);
			if(attackedChar == nullptr)
			{
				if(controllable_)
				{
					cout << "There is no one around with that name. You stand still in confusion." << endl;
					return false;
				}
			}
			int damage = rand()%(maxDamage_ - minDamage_+1) + minDamage_;
			std::cerr << "damage was " << damage << endl;
			if(controllable_)
			{
				if(attackedChar == this) 
				{
					cout << getName() << " is obviously very confused and hit oneself for " << damage << " damage!" << endl;
				} else
				{
					cout << "You managed to hit " << attackedChar->getName() << " for " << damage << " damage!" << endl;
				}
			}
			attackedChar->takeDamage(damage);
			
			return true;
		}

		// Drops the item with the name specified in string. 
		bool Character::drop(string itemString)
		{
			Item *foundItem = getInvMiscItem(itemString);
			if(foundItem != nullptr) 
			{
				// Remove item from inventory - add to room.
				removeInvMiscItem(itemString);
				currentRoom_->addMiscItem(foundItem);

				if(controllable_)
				{
					cout << "You dropped " << foundItem->getName() << "." << endl;
				}
				return true;
			}

			Equipable *foundEquipable = getInvEquipable(itemString);
			if(foundEquipable != nullptr) 
			{
				if(!foundEquipable->isEquipped())
				{
					// Remove item from inventory - add to room.
					removeInvEquipable(itemString);
					currentRoom_->addEquipable(foundEquipable);

					if(controllable_)
					{
						cout << "You dropped " << foundEquipable->getName() << "." << endl;
					}
				}
				else
				{
					if(controllable_)
					{
						cout << "You must first unequip " << foundEquipable->getName() << " before dropping it!" << endl;
					}
					return false;
				}
				return true;
			}

			Consumable *foundConsumable = getInvConsumable(itemString);
			if(foundConsumable != nullptr) 
			{
				// Remove item from inventory - add to room.
				removeInvConsumable(itemString);
				currentRoom_->addConsumable(foundConsumable);

				if(controllable_)
				{
					cout << "You dropped " << foundConsumable->getName() << "." << endl;
				}
				return true;
			}

			Character *foundCharacter = getInvCharacter(itemString);
			if(foundCharacter != nullptr && foundCharacter != this) 
			{
				// Remove item from inventory - add to room.
				removeInvCharacter(itemString);
				foundCharacter->setCurrentRoom(currentRoom_, false);

				if(controllable_)
				{
					cout << "You dropped " << foundCharacter->getName() << "." << endl;
				}
				return true;
			}

			if(controllable_)
			{
				cout << "You cannot drop something which you do not own!" << endl;
			}

			return false;
		}

		// Tries to pickup an item with the name specified in string
		bool Character::pickup(string itemString)
		{
			std::transform(itemString.begin(), itemString.end(),itemString.begin(), ::toupper); // To caps
			itemString.erase(remove(itemString.begin(),itemString.end(),' '),itemString.end());  // Trim whitespaces
			Item *foundItem = currentRoom_->getMiscItem(itemString);
			if(foundItem != nullptr) 
			{
				if((foundItem->getWeight() + currentCarried_) >= maxCarried_)
				{
					if(controllable_)
					{
						cout << "I am overburdened. The item is still on the ground." << endl;
						return false;
					}
				}
				else
				{
					// Remove item from room.
					currentRoom_->removeMiscItem(itemString);
					addInvMiscItem(foundItem);
					if(controllable_)
					{
						cout << "You picked up " << foundItem->getName() << "." << endl;
					}
					return true;
				}
			}

			Equipable *foundEquipable = currentRoom_->getEquipable(itemString);
			if(foundEquipable != nullptr) 
			{
				if((foundEquipable->getWeight() + currentCarried_) >= maxCarried_)
				{
					if(controllable_)
					{
						cout << "I am overburdened. The item is still on the ground." << endl;
						return false;
					}
				}
				else
				{
					// Remove item from room.
					currentRoom_->removeEquipable(itemString);
					addInvEquipable(foundEquipable);
					if(controllable_)
					{
						cout << "You picked up " << foundEquipable->getName() << "." << endl;
					}
					return true;
				}
			}

			Consumable *foundConsumable = currentRoom_->getConsumable(itemString);
			if(foundConsumable != nullptr) 
			{
				if((foundConsumable->getWeight() + currentCarried_) >= maxCarried_)
				{
					if(controllable_)
					{
						cout << "I am overburdened. The item is still on the ground." << endl;
						return false;
					}
				}
				else
				{
					// Remove item from room.
					currentRoom_->removeConsumable(itemString);
					if(foundConsumable->isConsumedOnPickup())
					{
						foundConsumable->applyEffect(*this);
						if(controllable_)
						{
							cout << foundConsumable->getName() << " disappeared." << endl;
						}
						delete foundConsumable;
						return true;
					}
					addInvConsumable(foundConsumable);
					if(controllable_)
					{
						cout << "You picked up " << foundConsumable->getName() << "." << endl;
					}
					return true;
				}
			}

			Character *foundCharacter = currentRoom_->getCharacter(itemString);
			if(foundCharacter != nullptr && foundCharacter != this) 
			{
				if((foundCharacter->getTotalWeight() + currentCarried_) >= maxCarried_)
				{
					if(controllable_)
					{
						cout << "I am overburdened. I could not pick up this character." << endl;
						return false;
					}
				}
				else
				{
					// Remove character from room.
					currentRoom_->removeCharacter(itemString);
					foundCharacter->setCanPerformAction(false); 
					addInvCharacter(foundCharacter);
					if(controllable_)
					{
						cout << "You added " << foundCharacter->getName() << " to your inventory." << endl;
					}
					return true;
				}
			}

			if(controllable_)
			{
				cout << "There is nothing of that nature in here!" << endl;
			}

			return false;
		}

		// Returns a sentence that the character will say.
		void Character::talk()
		{
			if(!thingsToSay_.empty())
			{
				cout << name_ << ": " << thingsToSay_[rand() % thingsToSay_.size()] << endl;
			}
			else
			{
				cout << name_ << " has nothing to say to you." << endl;
			}
		}

		// Initiates a conversation with a character.
		bool Character::talkTo(string characterString)
		{
			characterString.erase(remove(characterString.begin(),characterString.end(),' '),characterString.end());  // Trim whitespace
			Character * character = currentRoom_->getCharacter(characterString);
			if(controllable_)
			{
				if(character == nullptr)
				{
					cout << characterString << " is not in this room." << endl;
					return false;
				}

				if(character == this) 
				{
					cout << "You speak to yourself in confusion." << endl;
					return false;
				}

				character->talk();
			}
			return false;
		}

		// Attempt to consume a consumable item that the character owns.
		bool Character::consume(string consumableString)
		{
			consumableString.erase(remove(consumableString.begin(),consumableString.end(),' '),consumableString.end());  // Trim whitespace
			Consumable * consumable = getInvConsumable(consumableString);
			if(consumable == nullptr)
			{
				if(controllable_)
				{
					cout << "There is no " << consumableString << " in your inventory to consume." << endl;
					return false;
				}
			}

			consumable->applyEffect(*this);

			if(consumable->isConsumed())
			{
				removeInvConsumable(consumableString);
				if(controllable_)
				{
					cout << consumable->getName() << " was consumed and removed from the inventory" << endl;
				}
				delete consumable;
			}

			return true;
		}

		// Tries to hit a character with currently equipped weapon.
		int Character::fight(Character& character)
		{
			int damage = rand() % (maxDamage_ - minDamage_) + minDamage_;
			character.takeDamage(damage);
			return damage;
		}

		// Damages the character with a certain amount of points.
		void Character::takeDamage(int damage)
		{
			currentHealth_ -= damage;
			if(controllable_)
			{
				cout << "Ouch! Was hit for " << damage << " points of damage!" << endl;
			}
			
			if(!isAlive())
			{
				if(controllable_)
				{
					cout << name_ << " was killed! " << endl;
					
					cout << "\nPress enter to continue . . . ";
					std::cin.sync();
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					cout << endSeperatorLine;
				}
				else
				{
					if(currentRoom_->containsPlayer())
					{
						cout << name_ << " was killed! " << endl;
						cout << "The contents of its inventory were dropped on the ground. " << endl;
					}
				}
				killCharacter(true);
			}
			// Time to get mad
			else if(aiBehavior_ == AI_BEHAVIOR::DO_NOT_HIT_ME)
			{
				aiBehavior_ = AI_BEHAVIOR::AGGRESSIVE_AMOVE;
				if(currentRoom_->containsPlayer() && !controllable_)
				{
					cout << name_ << " got frenzied! Lookout!" << endl;
				}
			}
		}

		// Sets the weight of the character.
		void Character::setCharacterWeight(double weight)
		{
			weight_ = weight;
		}

		// Sets if the Character is in a new room.
		void Character::setInNewRoom(bool value)
		{
			atNewRoom_ = value;
		}

		// Sets the strength of the character to input.
		void Character::setStrength(int strength)
		{
			strength_ = strength;
		}

		// Sets the currenth health of the character to input.
		void Character::setCurrentHealth(int health)
		{
			currentHealth_ = health;
		}

		// Sets the maximum health of the character to input.
		void Character::setMaxHealth(int health)
		{
			maxHealth_ = health;
			if(currentHealth_ > maxHealth_)
			{
				currentHealth_ = maxHealth_;
			}
		}

		// Sets the maximum damage that the character can accomplish to input.
		void Character::setMaxDamage(int damage)
		{
			maxDamage_ = damage;
		}

		// Sets the minimum damage that the character can accomplish to input.
		void Character::setMinDamage(int damage)
		{
			minDamage_ = damage;
		}

		// Sets the amount of weight currently carried.
		void Character::setCurrentCarried(double carried)
		{
			currentCarried_ = carried;
		}

		// Sets the maximum weight that can be carried by character to input.
		void Character::setMaxCarried(double carried)
		{
			maxCarried_ = carried;
		}

		// Sets which room that the character is currently in.
		// (Removes character from previous room and adds it to the new room)
		//
		// leftRoom : True if character did leave its previous room (i.e. did not get placed at new room).
		void Character::setCurrentRoom(Environment* room, bool leftRoom)
		{
			if(currentRoom_ != nullptr)
			{
				currentRoom_->removeCharacter(name_);
				if(leftRoom)
				{
					currentRoom_->onLeave(*this);
				}
			}
			currentRoom_ = room;
			currentRoom_->addCharacter(this);
			atNewRoom_ = true;
		}

		// Sets if the Character can perform an action.
		void Character::setCanPerformAction(bool value)
		{
			canPerformAction_ = value;
		}
		
		// Sets the AI Behavior to be used when character is not controlled by a player.
		void Character::setAIBehavior(int behavior)
		{
			aiBehavior_ = behavior;
		}

		// Returns pointer to Item if it exist on this character. Otherwise returns nullptr.
		// If multiple entries exist, will return first occurence
		Item* Character::getInvMiscItem(string key) const
		{
			std::transform(key.begin(), key.end(),key.begin(), ::toupper); // To caps
			key.erase(remove(key.begin(),key.end(),' '),key.end());  // Trim whitespace

			auto it = carriedMiscItems_.find(key);
			if(it == carriedMiscItems_.end())
				return nullptr;
			else
				return it->second;
		}

		// Returns pointer to Equipable if it exist on this character. Otherwise returns nullptr.
		// If multiple entries exist, will return first occurence
		Equipable* Character::getInvEquipable(string key) const
		{
			std::transform(key.begin(), key.end(),key.begin(), ::toupper); // To caps
			key.erase(remove(key.begin(),key.end(),' '),key.end());  // Trim whitespace

			auto it = carriedEquipables_.find(key);
			if(it == carriedEquipables_.end())
				return nullptr;
			else
				return it->second;
		}

		// Returns pointer to Consumable if it exist on this character. Otherwise returns nullptr.
		// If multiple entries exist, will return first occurence
		Consumable* Character::getInvConsumable(string key) const
		{
			std::transform(key.begin(), key.end(),key.begin(), ::toupper); // To caps
			key.erase(remove(key.begin(),key.end(),' '),key.end());  // Trim whitespace

			auto it = carriedConsumables_.find(key);
			if(it == carriedConsumables_.end())
				return nullptr;
			else
				return it->second;
		}

		// Returns pointer to Character if it exist on this character. Otherwise returns nullptr.
		// If multiple entries exist, will return first occurence
		Character* Character::getInvCharacter(string key) const
		{
			std::transform(key.begin(), key.end(),key.begin(), ::toupper); // To caps
			key.erase(remove(key.begin(),key.end(),' '),key.end());  // Trim whitespace

			auto it = carriedCharacters_.find(key);
			if(it == carriedCharacters_.end())
				return nullptr;
			else
				return it->second;
		}

		// Adds the pointer to Item to inventory. 
		void Character::addInvMiscItem(Item* item)
		{
			string itemString = item->getName();
			std::transform(itemString.begin(), itemString.end(),itemString.begin(), ::toupper); // To caps
			itemString.erase(remove(itemString.begin(),itemString.end(),' '),itemString.end());  // Trim whitespaces

			carriedMiscItems_.insert(std::pair<string,Item*>(itemString,item));
			currentCarried_ += item->getWeight();
		}

		// Adds the pointer to Equipable to inventory. 
		void Character::addInvEquipable(Equipable* item)
		{
			string itemString = item->getName();
			std::transform(itemString.begin(), itemString.end(),itemString.begin(), ::toupper); // To caps
			itemString.erase(remove(itemString.begin(),itemString.end(),' '),itemString.end());  // Trim whitespaces
			
			carriedEquipables_.insert(std::pair<string,Equipable*>(itemString,item));
			currentCarried_ += item->getWeight();
		}

		// Adds the pointer to Consumable to inventory. 
		void Character::addInvConsumable(Consumable* item)
		{
			string itemString = item->getName();
			std::transform(itemString.begin(), itemString.end(),itemString.begin(), ::toupper); // To caps
			itemString.erase(remove(itemString.begin(),itemString.end(),' '),itemString.end());  // Trim whitespaces

			carriedConsumables_.insert(std::pair<string,Consumable*>(itemString,item));
			currentCarried_ += item->getWeight();
		}

		// Adds the pointer to Character to inventory. 
		void Character::addInvCharacter(Character* character)
		{
			string characterString = character->getName();
			std::transform(characterString.begin(), characterString.end(),characterString.begin(), ::toupper); // To caps
			characterString.erase(remove(characterString.begin(),characterString.end(),' '),characterString.end());  // Trim whitespaces

			carriedCharacters_.insert(std::pair<string,Character*>(characterString,character));
			currentCarried_ += character->getTotalWeight();
		}

		// Tries to remove the first occurence of matching pointer to Item from inventory (if it exist). 
		// Note: Will not delete the item itself, only remove it from the inventory.
		void Character::removeInvMiscItem(string key)
		{
			std::transform(key.begin(), key.end(),key.begin(), ::toupper); // To caps
			key.erase(remove(key.begin(),key.end(),' '),key.end());  // Trim whitespace

			auto it = carriedMiscItems_.find(key);
			if(it != carriedMiscItems_.end())
			{
				currentCarried_ -= getInvMiscItem(key)->getWeight();
				carriedMiscItems_.erase(it);
			}
		}

		// Tries to remove the first occurence of matching pointer to Equipable from inventory (if it exist). 
		// Note: Will not delete the item itself, only remove it from the inventory.
		void Character::removeInvEquipable(string key)
		{
			std::transform(key.begin(), key.end(),key.begin(), ::toupper); // To caps
			key.erase(remove(key.begin(),key.end(),' '),key.end());  // Trim whitespace

			auto it = carriedEquipables_.find(key);
			if(it != carriedEquipables_.end())
			{
				currentCarried_ -= getInvEquipable(key)->getWeight();
				carriedEquipables_.erase(key);
			}
		}

		// Tries to remove the first occurence of matching pointer to Consumable from inventory (if it exist). 
		// Note: Will not delete the item itself, only remove it from the inventory.
		void Character::removeInvConsumable(string key)
		{
			std::transform(key.begin(), key.end(),key.begin(), ::toupper); // To caps
			key.erase(remove(key.begin(),key.end(),' '),key.end());  // Trim whitespace

			auto it = carriedConsumables_.find(key);
			if(it != carriedConsumables_.end())
			{
				currentCarried_ -= getInvConsumable(key)->getWeight();
				carriedConsumables_.erase(it);
			}
		}

		// Tries to remove the first occurence of matching pointer to Character from inventory (if it exist). 
		// Note: Will not delete the character itself, only remove it from the inventory.
		void Character::removeInvCharacter(std::string key)
		{
			std::transform(key.begin(), key.end(),key.begin(), ::toupper); // To caps
			key.erase(remove(key.begin(),key.end(),' '),key.end());  // Trim whitespace

			auto it = carriedCharacters_.find(key);
			if(it != carriedCharacters_.end())
			{
				currentCarried_ -= getInvCharacter(key)->getTotalWeight();
				carriedCharacters_.erase(key);
			}
		}
		
		/** HERE FOLLOWS FUNCTIONS WHICH ARE ONLY RELEVANT TO NPC **/
		
		// Tries to move in a random direction.
		bool Character::tryRandomMove()
		{
			string tryDir = currentRoom_->getRandomDirection();
			return go(tryDir);
		}
		
		// Tries to say something random.
		bool Character::tryRandomSpeak()
		{
			// Only speak if Character has something to say (and there is a Player to hear it)
			if(!thingsToSay_.empty() && currentRoom_->containsPlayer())
			{
				cout << name_ << ": " << thingsToSay_[rand() % thingsToSay_.size()] << endl;
				return true;
			}
			else
			{
				return false;
			}
		}
		
		// Tries to attack a random target (including NPC).
		bool Character::tryRandomAttackAll()
		{
			string tryCharacter = currentRoom_->getRandomCharacter();
			Character * character = currentRoom_->getCharacter(tryCharacter);
			
			// Avoid hitting nothing and self
			if(character != nullptr && character != this)
			{
				// Only display attack message if Player is there to hear it
				if(currentRoom_->containsPlayer())
				{
					cout << name_ << " attacked " << character->getName() << "!" << endl;
				}
				attack(tryCharacter);
				return true;
			}
			else
			{
				return false;
			}
		}
		
		// Tries to attack a "random" character (excluding other NPCs).
		bool Character::tryRandomAttack()
		{
			string tryCharacter = currentRoom_->getRandomCharacter();
			Character * character = currentRoom_->getCharacter(tryCharacter);
			
			// Avoid hitting nothing and self (only hit other Players)
			if(character != nullptr && character != this && character->isControllable())
			{
				cout << name_ << " attacked " << character->getName() << "!" << endl;
				attack(tryCharacter);
				return true;
			}
			else
			{
				return false;
			}
		}
		
		// Tries to pickup a random item in the room.
		bool Character::tryPickupRandomItem()
		{
			if(currentRoom_->getMiscItems().size() > 0)
			{
				string item = currentRoom_->getRandomMiscItem();
				pickup(item);
				return true;
			}
			else if(currentRoom_->getConsumables().size() > 0)
			{
				string item = currentRoom_->getRandomConsumable();
				pickup(item);
				return true;
			}
			else if(currentRoom_->getEquipables().size() > 0)
			{
				string item = currentRoom_->getRandomEquipable();
				pickup(item);
				return true;
			}
			return false;
		}
		
		// Tries to pickup a random character in the room.
		bool Character::tryPickupRandomCharacter()
		{
			string tryCharacter = currentRoom_->getRandomCharacter();
			Character * character = currentRoom_->getCharacter(tryCharacter);
			
			if(character != nullptr)
			{
				if(pickup(tryCharacter))
				{
					// Only display message if Player is there to hear it
					if(currentRoom_->containsPlayer() || character->isControllable())
					{
						cout << name_ << " picked up " << character->getName() << "!" << endl;
					}
				}
				return true;
			}
			return false;
		}

		// Returns the multimap of characters for character
		std::map<string, Character*>& Character::getCharacters()
		{
			return carriedCharacters_;
		}

		// Returns the multimap of consumables for character
		std::multimap<std::string, Consumable*>& Character::getConsumables()
		{
			return carriedConsumables_;
		}

		// Returns the multimap of equipables for character
		std::map<std::string, Equipable*>& Character::getEquipables()
		{
			return carriedEquipables_;
		}

		// Returns the multimap of equipables for character
		std::multimap<std::string, Item*>& Character::getMiscItems()
		{
			return carriedMiscItems_;
		}
		
		// Consumes a random consumable in inventory (if such exist).
		bool Character::tryRandomConsume()
		{
			// TODO:
			return false;
		}

		// Assigns left-hand Character to right-hand Character.
		Character& Character::operator=(const Character& env)
		{
			// TODO:
			return *this;
		}
}

