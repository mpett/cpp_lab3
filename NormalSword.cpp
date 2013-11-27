/// Description: A sword with no magical or unordinary effects. Simple increases the damge of the user when equipped.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-27
#include "NormalSword.h"

namespace GameLogic
{
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

