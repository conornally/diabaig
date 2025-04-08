#include "diabaig.h"

static int _do_applypotion(Entity *item, Entity *potion)
{
	int status=RETURN_UNDEF;
	if( item && potion)
	{
		item->_o.potion_effect[0]=potion->_o.which;
		item->_o.potion_effect[1]=POTION_EFFECT_TIME;
		use(potion);
		status=RETURN_SUCCESS;
	}
	else status=RETURN_FAIL;
	return status;

}

int apply_potion()
{
	int status=RETURN_UNDEF;
	Entity *item;
	Entity *potion=menuselect(POTION, "select potion to apply");
	char *potionname, menu_string[64];
	if(potion && potion->_o.type==POTION)
	{
		potion=split_stack(potion);
		potionname=strdup(getname(potion));
		sprintf(menu_string,"select weapon to apply %s to",potionname);
		item=menuselect(WEAPON, menu_string);
		if(item && item->_o.type==WEAPON)
		{
			if(_do_applypotion(item,potion)==RETURN_SUCCESS)
			{
				msg("%s applied to %s",potionname,getname(item));
				status=RETURN_SUCCESS;
			}
		}
		else
		{
			status=RETURN_FAIL;
			msg("you can't apply %s to that",potionname);
		}
		free(potionname);
	}
	else msg("invalid selection, not a potion");
	return status;
}
