#include "diabaig.h"

void spawn_monster_inroom(room *r)
{
	Entity *e;
	int x,y;
	if(r)
	{
		e=new_monster();
		if(e)
		{
			placeinroom(r,e);
			x=e->pos.x;
			y=e->pos.y;
			switch(e->_c.type)
			{
				case 'f': (objat(x,y)=_new_obj(FOOD))->_o.which=F_MUSHROOM; break;
			}
			if(e->_c.flags&ISAGRO)
			{
				if(!rng(25)) e->_c.flags&=(~ISAGRO); //become not agro
				else if(!rng(15)) e->_c.flags|=CANTRACK; //chance that it can begins to track player 
			}
			// APEX creatures more likely to target player
			if(e->_c.flags&ISAPEX && !rng(5))
			{
				e->_c.flags|=(ISAGRO|CANTRACK);
			}
		}
		if(db.defeated_dragon)
		{
			if(!rng(5)) spawn_monster_inroom(r);
		}
		else if(!rng(20)) spawn_monster_inroom(r); //spawn another monster in room
	}
}
