#include "diabaig.h"

void do_addxp(Entity *e)
{
	//int dxp=0;
	if(e==player) return;
	monster_info info=monsters[e->_c.type-'A'];

	//if(e->flags&ISILLUSION) dxp=1;
	//else dxp=info.xp;

	db.xp+=info.xp;
	// need to do reanimations
	if(e->_c.type=='r') db.xp+= monsters[e->_c.form-'A'].xp;

	if(db.xp>level_values[db.xplvl])
	{
		levelup();
	}
}

void levelup()
{
	stats *s=&player->_c.stat;
	msg("you level up");
	db.xplvl+=1;
	
	int dhp=5+rng(5);
	s->maxhp+=dhp;
	s->hp+=dhp;
	s->str[1]++; //this might be too powerful?
	s->def++;
	s->res++;
}
