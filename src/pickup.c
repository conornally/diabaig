#include "diabaig.h"

int pickup()
{
	int status=RETURN_UNDEF;
	int inv_i, type;
	Entity *item;
	coord p=player->pos;
	item=objat(p.x,p.y);
	if(item)
	{
		type=item->_o.type;

		//special case GOLD
		if(item->_o.type==GOLD)
		{
			db.gold+=item->_o.quantity;
			if(item->_o.which==0)
			{
				if(item->_o.quantity>1) msg("you found %d pieces of gold", item->_o.quantity);
				else msg("you found a piece of gold");
			}
			else
			{
				if(item->_o.quantity <30)  		msg("you found a dragon scale");
				else if(item->_o.quantity <50)  msg("you found a shiny dragon scale");
				else							msg("you found a pristine dragon scale");
			}
			
			Entity *drag=getdragon();
			if(drag && (drag->_c.flags&ISSLEEP) && drag->pos.z==db.cur_level)
			{
				_daemon *d=search_daemon(drag,D_SLEEP);
				msg("the sound of you stealing gold wakes %s the %s",dragonname,dragon_mod);
				d->time=1;
			}

			clear_entity(item);
			return RETURN_SUCCESS;
		}


		//check stackable
		if(item->_o.flags & CANSTACK)
		{
			for(inv_i=0;inv_i<26;inv_i++)
			{
				if(db.inventory[inv_i]!=-1)
				{
					Entity *tmp;
					tmp=&db.objects[ db.inventory[inv_i] ];
					if((tmp->_o.type==type) && (tmp->_o.which==item->_o.which))
					{
						tmp->_o.quantity+=item->_o.quantity;
						msg("you picked up %s (%c)",getname(item), inv_i+'a');
						objat(p.x,p.y)=NULL;
						clear_entity(item);
						return RETURN_SUCCESS;
					}
				}
			}
		} //if a stackable item did not stack here, it will fall through

		for(inv_i=0;inv_i<26;inv_i++)
		{
			if(db.inventory[inv_i]==-1)
			{
				db.inventory[inv_i]=item->id;	
				objat(p.x,p.y)=NULL;
				status=RETURN_SUCCESS;
				break;
			}
		}

		//msg after pickup
		if(inv_i==26) msg("inventory full");
		else
		{
			msg("you picked up %s (%c)",getname(item), inv_i+'a');
		}
	}
	return status;
}


void _drop(Entity *item)
{
	Entity *tmp;
	if(item)
	{
		_do_dequip(item->id);
		for(int i=0;i<26;i++)
		{
			if(db.inventory[i]==item->id)
			{
				db.inventory[i]=-1;
			}
		}
		tmp=objat(player->pos.x, player->pos.y);
		if(tmp && tmp->_o.type==item->_o.type && tmp->_o.which==item->_o.which)
		{
			tmp->_o.quantity+=item->_o.quantity;
			clear_entity(item);
		}
		else
		{
			objat(player->pos.x, player->pos.y)=item;
			item->pos=player->pos;
		}
	}
}
int drop()
{
	int status=RETURN_UNDEF;
	Entity *e=menuselect(0, "select item to drop");
	if(e)
	{
		_drop(e);
		status=RETURN_SUCCESS;
	}
	else status=RETURN_FAIL;
	return status;
}
