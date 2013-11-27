/// Description: An armor with no magical or unordinary effects. Simple increases the max health of the user when equipped.
///
/// Authors: Martin Pettersson, Christoffer Wiss
///	Version: 2013-11-27
#include "NormalArmor.h"

namespace GameLogic
{
	// Returns true if the character passes the requirements of equipping this item.
	bool NormalArmor::requirementCheck(const Character& character) const
	{
		return (character.getStrength() >= strengthReq_);
	}

	// Applies the effect the item has on the character.
	void NormalArmor::onEquip(Character& character)
	{
		character.setMaxHealth(character.getMaxHealth()+maxHealthIncr_);
		character.equipArmor(this);
		equipped_ = true;
	}

	// Removes the effect the item has on the character.
	void NormalArmor::onUnequip(Character& character)
	{
		character.setMaxHealth(character.getMaxHealth()-maxHealthIncr_);
		character.unequipArmor();
		equipped_ = false;
	}
}

