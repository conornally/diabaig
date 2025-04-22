#include "diabaig.h"

int dragon_breath(Entity* e, Entity* target)
{
	if(e && target)
	{
		int colour=C_WHITE;
		int dtype=D_NONE;

		switch(e->_c.form)
		{ //INITIALISE ALL EFFECTS
			case DRG_FIRE: 	colour=STATUS_BURN; 	dtype=D_BURN; break;
			case DRG_ICE:  	colour=STATUS_FREEZE; dtype=D_FREEZE; break;
			case DRG_POISON:colour=STATUS_POISON; dtype=D_POISON; break;
			default: break;
		}

		if(lineofsight(e,target))
		{//LOS to inflict the damage
			int air=AIR;
			switch(e->_c.form)
			{
				case DRG_FIRE:   msg("%s breaths fire",getname(e)); air=COMBUST; break;
				case DRG_ICE:    msg("%s breaths icy mist",getname(e)); air=MIST;break;
				case DRG_POISON: msg("%s spits poison",getname(e)); air=MIASMA; break;
				default: break;
			}

			// EITHER EXTEND OR ADD DAEMON
			_daemon *d=search_daemon(e,dtype);
			if(d) d->time+=rng(2);
			else add_daemon(target,dtype,2);

			tileat(target->pos.x,target->pos.y)->air=air;
			tileat(target->pos.x,target->pos.y)->air_pressure+=2;

		}

		
		//I dont need LOS to animate it
		//But i do need them to be in the same room
		if(e->_c._inroom==target->_c._inroom)
		{
			light_room(e->_c._inroom); //Annoying bodge
			animation a=(animation){A_LASER, '*', COLOR_PAIR(colour)|A_BOLD, e->pos, target->pos,0};
			a.pos=(coord){e->pos.x,e->pos.y,3};
			//a.target_pos=target->pos;
			//add_animation(a);
			animate(&a);
		}
	}
	return 0;
}

int dragon_hypnoticwords(Entity *e, Entity *target)
{
	if(e && target && lineofsight(e,target))
	{
		light_room(e->_c._inroom); //Annoying bodge
		add_daemon(target, D_SLEEP, 3+rng(3));
		msg("%s whispers hypnotic words",dragonname);
		animation a=(animation){A_AOE, '*', COLOR_PAIR(C_WHITE), e->pos, {0,0,0},0};
		a.pos=(coord){e->pos.x, e->pos.y, XMAX};
		//add_animation(a);
		animate(&a);
	}
	return 0;
}

void _ai_dragon(Entity* e)
{
	/* If dragon is not breathing fire, 
	 * chance to start
	 * continue breathing
	 * do not move during this
	 *
	 * If dragon is breathing fire,
	 * chance to stop
	 */
	int *stam=&e->_c.stamina;

	if(e->_c._inroom != player->_c._inroom)
	{
		e->_c.last_seen_player+=MAX_PATHMEM;
		e->_c.flags &= ~CANTRACK;
		e->_c.flags |= ISWANDER;
	}
	else
	{
		//e->_c.flags |= CANTRACK;
		e->_c.flags &= ~ISWANDER;
	}

	// START BREATHE ATTACK
	if(!(e->_c.flags & ISBREATH) && !rng(10) && lineofsight(e,player))
	{
		add_daemon(e, D_DRAGONBREATH, 3+rng(10));
		msg("%s prepares for a large attack",dragonname);
		*stam-=1;
	}

	// DO BREATHE ATTACK
	if(*stam)
	{
		if(e->_c.flags&ISBREATH)
		{
			dragon_breath(e,player);
			//e->_c.stamina--;
			*stam-=1;
		}
	}

	// DO HYPNOSIS
	//if(*stam && !rng(50))
	//{
	//	dragon_hypnoticwords(e, player);
	//	*stam-=1;
	//}

	// SPAWN ADDS ELSEWHERE
	if(!rng(80))
	{
		int available[]={'l','Y','W','H','F'};
		int weights[]=  {15, 10, 5, 1, 2};
		int type=available[ weighted_pick(weights,5) ];

		int attempts=0;
		room *r;
		do
		{
			r=&db.rooms[ rng(db.nrooms-1) ];
			attempts++;
		}while(r==player->_c._inroom && attempts<20);

		if(attempts<20)
		{
			Entity *e=_new_monster(type);
			if(!rng(2)) e->_c.flags|=(CANTRACK);
			placeinroom(r, e);
		}
	}

	// SPAWN POTIONS ELSEWHERE
	if(!rng(50))
	{
		room *r;
		int attempts=0;
		do
		{
			r=&db.rooms[ rng(db.nrooms-1) ];
			attempts++;
		}while(r==player->_c._inroom && attempts<20);

		if(attempts<20)
		{
			Entity *e=_new_obj(POTION);
			placeinroom(r,e);
		}
	}

	if(!rng(30))
	{
		tileat(e->pos.x,e->pos.y)->air_pressure+=3;
		tileat(e->pos.x,e->pos.y)->air=e->_c.form+1; //might need to be explicit
	}
}

void _ai_youngdragon(Entity* e)
{
	int *stam=&e->_c.stamina;
	if(!(e->_c.flags & ISBREATH) && !rng(10) && lineofsight(e,player) && (pow(e->pos.x-player->pos.x,2)+pow(e->pos.y-player->pos.y,2)<pow(10,2)))
	{
		add_daemon(e, D_DRAGONBREATH, 3+rng(4));
		msg("%s prepares for a large attack",getname(e));
		*stam-=1;
	}

	// DO BREATHE ATTACK
	if(*stam)
	{
		if(e->_c.flags&ISBREATH)
		{
			dragon_breath(e,player);
			*stam-=1;
		}
	}

	if(!rng(30))
	{
		tile *t=tileat(e->pos.x,e->pos.y);
		if(t && ((t->flags&ML_VISIBLE) || (e->_c._inroom==player->_c._inroom)))
		{
			t->air_pressure+=1;
			t->air=COMBUST; //might need to be explicit
			msg("%s hiccups",getname(e));
		}
	}
}

void _ai_ElderDragon(Entity *e)
{
	int *stam=&e->_c.stamina;
	if(!(e->_c.flags & ISBREATH) && !rng(10) && lineofsight(e,player))
	{
		add_daemon(e, D_DRAGONBREATH, 10+rng(10));
		msg("%s prepares for a large attack",getname(e));
		*stam-=1;
	}

	// DO BREATHE ATTACK
	if(*stam)
	{
		if(e->_c.flags&ISBREATH)
		{
			dragon_breath(e,player);
			*stam-=1;
		}

		if(!rng(10))
		{
			_spawn_adds(e,'Y',2+rng(2));
		}
	}

	if(!rng(30))
	{
		tileat(e->pos.x,e->pos.y)->air_pressure+=5;
		tileat(e->pos.x,e->pos.y)->air=COMBUST; //might need to be explicit
	}
}

void _ai_dragon_offscreen(Entity* e)
{
	if(e && !rng(1000))
	{
		msg("you hear a loud roar from somewhere far beneath you");
	}
}

void gen_dragon()
{
	int form, forms[NDRAG_TYPE]={DRG_FIRE,DRG_ICE,DRG_POISON};
	Entity* e=_new_monster('D');

	form=forms[rng(NDRAG_TYPE)];
	e->_c.form= form;
	gen_dragonname(dragonname,form);
	e->pos.z=SECONDARYBOSS;

	struct mod { int dhp, dstr, ddef, ddex, res;};
	struct mod mods[NDRAG_TYPE] = {
		{0,   2, -10,0, IMMUNE_FIRE},
		{100, -2,10, 0, IMMUNE_FROST},
		{-100,0, -10,8, IMMUNE_POISON}
	};

	struct mod _mod=mods[form];
	e->_c.stat.maxhp+=_mod.dhp;
	e->_c.stat.hp+=_mod.dhp;
	e->_c.stat.str[0]+=_mod.dstr;
	e->_c.stat.def+=_mod.ddef;
	e->_c.stat.dex+=_mod.ddex;
	e->_c.res_flags|=_mod.res;


}

Entity* getdragon()
{
	Entity *e=NULL, *tmp=NULL;
	for(tmp=&db.creatures[0]; tmp<&db.creatures[DBSIZE_CREATURES]; tmp++)
	{
		if( (tmp->flags&ISACTIVE) && (tmp->_c.type=='D') )
		{
			e=tmp;
			break;
		}
	}
	return e;
}

int checkfortooth()
{
	int id,success=false;
	Entity e;
	for(int i=0; i<26; i++)
	{
		id=db.inventory[i];
		if(id>=0)
		{
			e=db.objects[id];
			if(e._o.type==TRINKET && e._o.which==TK_DRAGONTOOTH) success=1;
		}
	}
	return success;
}
