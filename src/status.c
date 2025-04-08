#include "diabaig.h"
/*This is a late addition to the code
 * converting the main status effects from a daemon system 
 * to a component system. So that the gas effects are easier to apply
 */

/*
static void _burn(Entity *e)
{
	int resist_chance= (e->_c.res_flags&RESIST_FIRE)?2:1;
	if(e->_c.res_flags & IMMUNE_FIRE) player_msg(e,"the fire has no effect");
	else
	{
		if(!rng(resist_chance))
		{
			int damage=(e->_c.res_flags&WEAKTO_FIRE)?2:1;
			e->_c.stat.hp-=damage;
			if(e==player) 
			{
				msg("you are burning");
				set_ripdata(RIP_BURN,"to a crisp"); 
			}
		}
		else player_msg(e,"you resist the fire");
	}
	if(tileat(e->pos.x,e->pos.y)->air!=COMBUST && !search_daemon(e,D_BURN)) e->_c.flags&=~ISBURN;
}

static void _poison(Entity *e)
{
	int resist_chance= (e->_c.res_flags&RESIST_POISON)?2:1;
	if(e->_c.res_flags & IMMUNE_POISON) player_msg(e,"the poison has no effect");
	else
	{
		//if(!rng(2) && !rng(resist_chance))
		if(!rng(resist_chance))
		{
			int damage= (e->_c.res_flags&WEAKTO_POISON)?2:1;
			e->_c.stat.hp-=damage;
			if(e==player)
			{
				msg("you feel the poison running through your body");
				set_ripdata(RIP_STATUS,"poison");
			}
		}
		else player_msg(e,"you resist the poison");
	}
	if(tileat(e->pos.x,e->pos.y)->air!=MIASMA && !search_daemon(e,D_POISON)) e->_c.flags&=~ISPOISON;
}


static void _freeze(Entity *e)
{
	/// freeze can be broken free of at any point
	 // but you are exponentially more likely to 
	 // closer to the end time
	 //
	 // resisitance makes it more likely to be broken
	 //
	 // the "maths"
	 // inverse exponential
	 ///
	_daemon *d = search_daemon(e,D_FREEZE);
	if(e->_c.res_flags & IMMUNE_FROST) player_msg(e,"the ice has no effect");
	else
	{
		float exponent= 3;//d->time;
		if(d) exponent=d->time;
		if(e->_c.res_flags&RESIST_FROST) exponent/=2.0;
		if(e->_c.res_flags&WEAKTO_FROST) exponent*=2.0;
		float threshold=exp(-exponent);
		float break_attempt= (float)rand()/RAND_MAX;

		if(break_attempt <= threshold) //break freeze
		{
			e->_c.flags &= ~ISFREEZE;
			player_msg(e,"you break free of the ice");

			// this is kinda annoying
			if(d) stop_daemon(d); 
		}
		else e->_c.flags |= ISFREEZE;
	}

	//if(tileat(e->pos.x,e->pos.y)->air!=MIST && !d) e->
}
*/

int status_system(Entity *e)
{
	int air;
	int dtype=D_NONE;
	float pres;
	_daemon *d=NULL;
	if(e && (e->flags&ISACTIVE))
	{
		air=tileat(e->pos.x,e->pos.y)->air;
		pres=tileat(e->pos.x,e->pos.y)->air_pressure;

		if(pres>=0.05)
		{
		if(air==COMBUST) dtype=D_BURN;
		if(air==MIST)    dtype=D_FREEZE;
		if(air==MIASMA)  dtype=D_POISON;
		if(air==SMOKE && e!=player) dtype=D_BLIND;

		d=search_daemon(e,dtype);
		if(!d) add_daemon(e,dtype,2);
		else d->time++;
		}


		//flag=e->_c.flags;
		//if(flag&ISBURN) _burn(e);
		//if(flag&ISPOISON) _poison(e);
		//if(flag&ISFREEZE) _freeze(e);
	}
	return 0;
}
