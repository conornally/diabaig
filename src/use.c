#include "diabaig.h"
int use(Entity *e)
{
	if(e)
	{
		obj_info *info=getinfo(e->_o.type);
		if(info) 
		{
			info[e->_o.which].known=true;
		}
		switch(e->_o.type)
		{
			case POTION:
			case SCROLL:
			case FOOD:
				if(--e->_o.quantity<=0)
				{
					for(int i=0;i<26;i++)
					{ //remove from inventory
						if(db.inventory[i]==e->id)
							db.inventory[i]=-1;
					}
					if(objat(e->pos.x,e->pos.y)==e)
						objat(e->pos.x,e->pos.y)=NULL;
					clear_entity(e);
				}

				break;
			default:
				break;
		}
	}
	return 0;
}



void _do_weapon_effect(Entity *weapon, Entity *target, int melee)
{
	int damage=0;
	if(weapon && target)
	{
		if(melee) damage+= diceroll(weapon->_o.mod_melee);
		else damage+= diceroll(weapon->_o.mod_throw);

		if(((player->_c.flags&ISINVIS)||(target->_c.flags&ISBLIND)) && weapon->_o.which==DAGGER)
		{
			damage*=10.0;
			//msg("-->?");
		}
		if(weapon->_o.which==TORCH && !(rng(10)))
		{
			add_daemon(target, D_BURN, 3);
			if(!(target->_c.res_flags & IMMUNE_FIRE)) msg("%s is set alight",getname(target));
		}

		damage=modify_damage(target,damage,DMG_PHYSICAL);
		//target->_c.stat.hp -= damage * (int)ceil(15.0/(15.0+target->_c.stat.def));
		target->_c.stat.hp-=damage;

		if(weapon->_o.potion_effect[1]>0)
		{
			Entity *tmp=_new_obj(POTION);
			_set_potion(tmp,weapon->_o.potion_effect[0]);
			_dodrink(target,tmp);
			if(!--weapon->_o.potion_effect[1]) msg("the coating on your weapon wears off");
		}
	}
}
