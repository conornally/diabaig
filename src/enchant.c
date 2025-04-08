#include "diabaig.h"

// enchant
// disenchant
// curse

int enchant(Entity* e)
{
	int status=RETURN_FAIL;
	if(e && e->flags&ISOBJ)
	{
		e->_o.enchant_level++;
		status=RETURN_SUCCESS;
		switch(e->_o.type)
		{
			case WEAPON:
				if( e->_o.which==SHIELD || e->_o.which==TOWERSHIELD ) e->_o.mod_def++;
				else
				{
					e->_o.mod_melee[0]++;
					e->_o.mod_throw[0]++;
				}
				break;
			case ARMOUR:
				e->_o.mod_def++;
				e->_o.mod_res++;
				break;
			default: 
				e->_o.enchant_level--;
				status=RETURN_FAIL;
				break;
		}
	}
	return status;
}

int disenchant(Entity* e)
{
	int status=RETURN_FAIL;
	if(e && e->flags&ISOBJ)
	{
		e->_o.enchant_level--;
		status=RETURN_SUCCESS;
		switch(e->_o.type)
		{
			case WEAPON:
				if( e->_o.which==SHIELD || e->_o.which==TOWERSHIELD ) e->_o.mod_def = MAX(0,e->_o.mod_def);
				else
				{
					e->_o.mod_melee[0] = MAX(0,e->_o.mod_melee[0]-1);
					e->_o.mod_throw[0] = MAX(0,e->_o.mod_throw[0]-1);
				}
				break;
			case ARMOUR:
				e->_o.mod_def = MAX(0,e->_o.mod_def-1);
				e->_o.mod_res = MAX(0,e->_o.mod_res-1);
				break;
			default: 
				e->_o.enchant_level++;
				status=RETURN_FAIL;
				break;
		}
	}
	return status;
}
