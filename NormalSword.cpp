/// Description: A sword with no magical or unordinary effects. Simple increases the damge of the user when equipped.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-27
#include "NormalSword.h"
using std::string;

namespace GameLogic
{
	// Returns a detailed string representation of this item.
	// Used when saving the game information to file.
	string NormalSword::printItem() const
	{
		std::stringstream ss;
		ss << Equipable::printItem(); // Print Equipable part
		ss << minDamageIncr_ << "\n" << maxDamageIncr_ << "\n" << strengthReq_ << "\n"; // Print NormalSword part
		return ss.str();
	}

	// Returns true if the character passes the requirements of equipping this item.
	bool NormalSword::requirementCheck(const Character& character) const
	{
		return (character.getStrength() >= strengthReq_);
	}

	// Applies the effect the item has on the character.
	void NormalSword::onEquip(Character& character)
	{
		character.setMinDamage(character.getMinDamage()+minDamageIncr_);
		character.setMaxDamage(character.getMaxDamage()+maxDamageIncr_);
		character.equipWeapon(this);
		equipped_ = true;
	}

	// Removes the effect the item has on the character.
	void NormalSword::onUnequip(Character& character)
	{
		character.setMinDamage(character.getMinDamage()-minDamageIncr_);
		character.setMaxDamage(character.getMaxDamage()-maxDamageIncr_);
		character.unequipWeapon();
		equipped_ = false;
	}
}

