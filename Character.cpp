/// Description: A Character is able to move within the world and interact with it.
///              It can be controllable (user can issue commands to it) or un-controllable (command is randomly generated).
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-24
#include <algorithm>
#include "Character.h"
using std::string;

namespace GameLogic
{
		// Destructor to character.
		Character:: ~Character()
		{
			if(currentEquippedArmor_ != nullptr)
			{
				delete currentEquippedArmor_;
			}
			if(currentEquippedWeapon_ != nullptr)
			{
				delete currentEquippedWeapon_;
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

		// Removes the character from its room (drops its items as loot if specified) and then deletes it by running its destructor.
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
					currentRoom_->addEquipable(currentEquippedArmor_);
					currentEquippedArmor_ = nullptr; // Make sure we do not delete this item since it is now in the room
				}
				if(currentEquippedWeapon_ != nullptr)
				{
					currentRoom_->addEquipable(currentEquippedWeapon_);
					currentEquippedWeapon_ = nullptr; // Make sure we do not delete this item since it is now in the room
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
			currentRoom_->removeCharacter(name_);
			delete this;
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

		// Prints a description of the current room; lists all characters, items and exits (in the room).
		bool Character::look(string)
		{
			if(controllable_)
			{
				cout << currentRoom_->getDescription(this) << endl;
			}

			return false;
		}

		// Returns a string with detailed information about this character.
		string Character::printCharacter() const
		{
			//TODO:
			return "This is not a character.";
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
		bool Character::equipArmor(Equipable* armor)
		{
			if(armor->requirementCheck(*this))
			{
				if(currentEquippedArmor_ != nullptr)
				{
					currentEquippedArmor_->onUnequip(*this);
				}
				currentEquippedArmor_ = armor; // TODO: Is ok?
				currentEquippedArmor_->onEquip(*this);
				return true;
			}
			else
			{
				return false;
			}
		}

		// Equips a weapon. If another weapon is currently equipped, then this will be unequipped and added to the inventory.
		bool Character::equipWeapon(Equipable* weapon)
		{
			if(weapon->requirementCheck(*this))
			{
				if(currentEquippedWeapon_ != nullptr)
				{
					currentEquippedWeapon_->onUnequip(*this);
				}
				currentEquippedWeapon_ = weapon; // TODO: Is ok?
				currentEquippedWeapon_->onEquip(*this);
				return true;
			}
			else
			{
				return false;
			}
		}

		// Tries to move from the room in a certain direction. Returns true if successful.
		bool Character::go(string direction)
		{
			string originalDirection = direction;											 // Retain original message for output
			std::transform(direction.begin(), direction.end(),direction.begin(), ::toupper); // To caps
			direction.erase(remove(direction.begin(),direction.end(),' '),direction.end());  // Trim whitespaces

			Environment * nextRoom = currentRoom_->neighbor(direction);
			if(nextRoom != nullptr)
			{
				setCurrentRoom(nextRoom, true); // TODO: Is ok?
				if(controllable_) cout << "You went" << originalDirection << endl;
				currentRoom_->onEntry(*this);
				return true;
			}
			else
			{
				if(controllable_) cout << "You cannot go" << originalDirection << endl;
				return false;
			}
		}

		// Drops the item with the name specified in string. 
		bool Character::drop(string itemString)
		{
			std::transform(itemString.begin(), itemString.end(),itemString.begin(), ::toupper); // To caps
			itemString.erase(remove(itemString.begin(),itemString.end(),' '),itemString.end());  // Trim whitespaces
			
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
				// Remove item from inventory - add to room.
				removeInvEquipable(itemString);
				currentRoom_->addEquipable(foundEquipable);

				if(controllable_)
				{
					cout << "You dropped " << foundEquipable->getName() << "." << endl;
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
		string Character::talk()
		{
			return thingsToSay_[rand() % thingsToSay_.size()];
		}

		// Initiates a conversation with a character.
		string Character::talkTo(Character& character)
		{
			return character.talk();
		}

		// Tries to hit a character with currently equipped weapon.
		int Character::fight(Character& character)
		{
			int damage = rand() % (maxDamage_ - minDamage_) + minDamage_;
			character.takeDamage(damage);
			return damage;
		}

		// Damages the character with a certian amount of points.
		void Character::takeDamage(int damage)
		{
			currentHealth_ -= damage;
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
				carriedMiscItems_.erase(key);
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
				carriedConsumables_.erase(key);
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

		// Assigns left-hand Character to right-hand Character.
		Character& Character::operator=(const Character& env)
		{
			// TODO:
			return *this;
		}
}

