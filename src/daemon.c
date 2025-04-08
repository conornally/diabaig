#include "diabaig.h"

#define POISON_CHANCE 2

/* resistance
 * RESIST_STATUS causes 50% chance of effect not occuring
 * IMMUNE_STATUS causes complete immunity from the status
 * RESIST_FIRE against dragon fire has 1/4 resistence
 */

static void _daemon_end_regen(_daemon *d)
{
	Entity *e=cid(d->c_id);
	stats *s=&e->_c.stat;
	s->hp++;
	s->hp= MIN(s->hp,s->maxhp);
	add_daemon(e, D_REGEN, REGENDURATION-db.xplvl); //youll slowly start regenning faster
}

static void _daemon_confuse(_daemon *d)
{
	//if stacked confusion daemons, this will cancel..
	Entity *e=cid(d->c_id);
	e->_c.flags |= ISCONFUSED;

	if(d->time==1)
	{
		e->_c.flags &= ~(ISCONFUSED);
		if(e==player) msg("you are now less disorientated");
	}
}

static void _daemon_invisibilty(_daemon *d)
{
	Entity *e=cid(d->c_id);
	e->_c.flags |= ISINVIS;

	if(d->time==1)
	{
		e->_c.flags &= (~ISINVIS);
		if(e==player) msg("you are no longer invisible");
		else msg("%s reappears",getname(e));
	}

}
//static void _daemon_poison(_daemon *d)
//{
//	Entity *e=cid(d->c_id);
//
//	if(e->_c.res_flags & IMMUNE_POISON) 
//	{
//		e->_c.flags &= ~ISPOISON;
//		stop_daemon(d);
//	}
//	else e->_c.flags|= ISPOISON;
//
//	if(d->time==1)
//	{
//		e->_c.flags &= ~ISPOISON;
//		if(e==player) msg("the poison wears off");
//	}
//
//	//int flag=e->_c.res_flags;
//	//int resist_chance= (flag&RESIST_POISON)?2:1;
//
//	//if(flag & IMMUNE_POISON) //poison immunity
//	//{
//	//	//d->time=1;
//	//	player_msg(e,"the poison has no effect");
//	//	stop_daemon(d);
//	//}
//	//else
//	//{
//	//	e->_c.flags |= ISPOISON;
//	//	if(!rng(POISON_CHANCE))
//	//	{
//	//		if(!rng(resist_chance))
//	//		{
//	//			int damage= (flag&WEAKTO_POISON)?2:1;
//	//			e->_c.stat.hp-=damage;
//	//			if(e==player)
//	//			{
//	//				msg("you feel the poison running through your body");
//	//				set_ripdata(RIP_STATUS,"poison");
//	//			}
//	//		}
//	//		else
//	//		{
//	//			player_msg(e,"you resist the poison");
//	//		}
//	//	}
//	//}
//
//	// END DAEMON
//	//if(d->time==1)
//	//{
//	//	e->_c.flags &= ~ISPOISON;
//	//	if(e==player) msg("the poison wears off");
//	//}
//}
//
//static void _daemon_burn(_daemon *d) //dragon fire can be more powerful than this
//{
//	Entity *e=cid(d->c_id);
//	if(e->_c.res_flags & IMMUNE_FIRE)
//	{
//		e->_c.flags&=~ISBURN;
//		stop_daemon(d);
//	}
//	else e->_c.flags|=ISBURN;
//
//	//END DAEMON
//	if(d->time==1) 
//	{
//		e->_c.flags &= ~ISBURN;
//		if(e==player) msg("you stopped, dropped and rolled");
//	}
//}
//
//static void _daemon_freeze(_daemon *d) 
//{
//	/* freeze can be broken free of at any point
//	 * but you are exponentially more likely to 
//	 * closer to the end time
//	 *
//	 * resisitance makes it more likely to be broken
//	 *
//	 * the "maths"
//	 * inverse exponential
//	 */
//	Entity *e=cid(d->c_id);
//	int flag=e->_c.res_flags;
//
//	if(flag & IMMUNE_FROST)
//	{
//		e->_c.flags&=~ISBURN;
//		stop_daemon(d);
//	}
//	else e->_c.flags|=ISFREEZE;
//	//{
//
//	//	float exponent= d->time;
//	//	if(flag&RESIST_FROST) exponent/=2.0;
//	//	if(flag&WEAKTO_FROST) exponent*=2.0;
//	//	float threshold=exp(-exponent);
//	//	float break_attempt= (float)rand()/RAND_MAX;
//
//	//	if(break_attempt <= threshold) //break freeze
//	//	{
//	//		e->_c.flags &= ~ISFREEZE;
//	//		player_msg(e,"you break free of the ice");
//	//		stop_daemon(d); //that might be dangerous
//	//	}
//	//	else
//	//	{
//	//		e->_c.flags |= ISFREEZE;
//	//		//player_msg(e,"you are frozen");
//	//	}
//	//}
//
//	if(d->time==1) 
//	{
//		e->_c.flags &= ~ISFREEZE;
//		if(e==player) msg("you have thawed");
//	}
//}

static void _daemon_poison(_daemon *d)
{
	Entity *e=cid(d->c_id);
	int flag=e->_c.res_flags;
	int resist_chance= (flag&RESIST_POISON)?2:1;

	if(flag & IMMUNE_POISON) //poison immunity
	{
		//d->time=1;
		player_msg(e,"the poison has no effect");
		stop_daemon(d);
	}
	else
	{
		e->_c.flags |= ISPOISON;
		if(!rng(POISON_CHANCE))
		{
			if(!rng(resist_chance))
			{
				int damage= modify_damage(e,1,DMG_POISON);//(flag&WEAKTO_POISON)?2:1;
				e->_c.stat.hp-=damage;
				if(e==player)
				{
					msg("you feel the poison running through your body %d",damage);
					set_ripdata(RIP_STATUS,"poison");
				}
			}
			else
			{
				player_msg(e,"you resist the poison");
			}
		}
	}

	// END DAEMON
	if(d->time==1)
	{
		e->_c.flags &= ~ISPOISON;
		if(e==player) msg("the poison wears off");
	}
}

static void _daemon_burn(_daemon *d) //dragon fire can be more powerful than this
{
	Entity *e=cid(d->c_id);
	int flag=e->_c.res_flags;
	int resist_chance= (flag&RESIST_FIRE)?2:1;

	if(flag & IMMUNE_FIRE) //FIRE immunity
	{
		player_msg(e,"the fire has no effect");
		stop_daemon(d);
	}
	else
	{
		e->_c.flags |= ISBURN;
		if(!rng(resist_chance))
		{
			int damage=modify_damage(e,1,DMG_FIRE);//(flag&WEAKTO_FIRE)?2:1;
			e->_c.stat.hp-=damage;
			if(e==player) 
			{
				msg("you are burning");
				set_ripdata(RIP_BURN,"to a crisp"); //dragon fire can say BURN,in dragon fire
			}
		}
		else player_msg(e,"you resist the fire");
	}

	//END DAEMON
	if(d->time==1) 
	{
		e->_c.flags &= ~ISBURN;
		if(e==player) msg("you stopped, dropped and rolled");
	}
}

static void _daemon_freeze(_daemon *d) 
{
	/* freeze can be broken free of at any point
	 * but you are exponentially more likely to 
	 * closer to the end time
	 *
	 * resisitance makes it more likely to be broken
	 *
	 * the "maths"
	 * inverse exponential
	 */
	Entity *e=cid(d->c_id);
	int flag=e->_c.res_flags;


	if(flag & IMMUNE_FROST)
	{
		player_msg(e,"the ice has no effect");
		stop_daemon(d);
	}
	else if(e==player && has_wep(TORCH))
	{
		msg("the warmth from your torch prevents the ice freezing your body");
		//stop_daemon(d);
	}
	else
	{
		float exponent= d->time;
		if(flag&RESIST_FROST) exponent/=2.0;
		if(flag&WEAKTO_FROST) exponent*=2.0;
		float threshold=exp(-exponent);
		float break_attempt= (float)rand()/(float)RAND_MAX;

		if(break_attempt <= threshold) //break freeze
		{
			e->_c.flags &= ~ISFREEZE;
			player_msg(e,"you break free of the ice");
			stop_daemon(d); //that might be dangerous
		}
		else
		{
			e->_c.flags |= ISFREEZE;
			//player_msg(e,"you are frozen");
		}
	}

	if(d->time==1 || (e->_c.flags&ISBURN) )
	{
		e->_c.flags &= ~ISFREEZE;
		if(e==player) msg("you have thawed");
		stop_daemon(d);
	}
}

static void _daemon_fastregen(_daemon *d)
{
	Entity *e=cid(d->c_id);
	e->_c.flags |= ISREGEN;
	e->_c.stat.hp=MIN( e->_c.stat.hp+1, e->_c.stat.maxhp);
	if(d->time==1) 
	{
		e->_c.flags &= ~ISREGEN;
	}
}

static void _daemon_vulnerability(_daemon *d)
{
	Entity *e=cid(d->c_id);
	e->_c.flags |= ISVULN;
	if(d->time==1)
	{
		e->_c.flags &= ~ISVULN;
		player_msg(e,"you come back to your senses, you feel more alert");
	}
}

static void _daemon_blindness(_daemon *d)
{
	Entity *e=cid(d->c_id);
	e->_c.flags|=ISBLIND;
	if(d->time==1)
	{
		e->_c.flags &= ~ISBLIND;
		player_msg(e,"you can see again");
	}
}
static void _daemon_sleep(_daemon *d)
{
	Entity *e=cid(d->c_id);
	if(e->_c.res_flags & IMMUNE_SLEEP)
	{
		e->_c.flags&=~ISSLEEP;
		d->time=0;
	}
	else
	{
		e->_c.flags|=ISSLEEP;
		e->_c.flags &= ~ISFOLLOW;
	}

	if(d->time==1)
	{
		e->_c.flags&=~ISSLEEP;
		if(e==player) msg("you regain conciousness, some time has passed");
		else msg("%s wakes up",getname(e));
	}
}

static void _daemon_end_str(_daemon *d)
{
	Entity *e=cid(d->c_id);
	e->_c.stat.str[1]-=1;
	if(e==player) msg("your increased strength wears off");

}

static void _daemon_bind(_daemon *d)
{
	Entity *e=cid(d->c_id);
	e->_c.flags |= ISBOUND;
	if(d->time==1) 
	{
		e->_c.flags &= ~ISBOUND;
		if(e==player) msg("you struggle free from the binding vines");
	}
}

static void _daemon_haste(_daemon *d)
{
	Entity *e=cid(d->c_id);
	e->_c.flags |= ISSPEED;
	e->_c.flags &= ~ISSLOW;
	if(d->time==1) 
	{
		e->_c.flags &= ~ISSPEED;
		if(e==player) msg("your movement slows to a normal speed");
	}
}

static void _daemon_slow(_daemon *d)
{
	Entity *e=cid(d->c_id);
	e->_c.flags |= ISSLOW;
	e->_c.flags &= ~ISSPEED;
	if(d->time==1) 
	{
		e->_c.flags &= ~ISSLOW;
		if(e==player) msg("you feel less sluggish");
	}
}

static void _daemon_lsd(_daemon *d)
{
	Entity *e=cid(d->c_id);
	e->_c.flags |= ISLSD;
	if(d->time==1) 
	{
		e->_c.flags &= ~ISLSD;
		if(e==player) msg("that was a bad trip");
	}
}

static void _daemon_ink(_daemon *d)
{
	Entity *e=cid(d->c_id);
	e->_c.flags |= ISINKED;
	if(d->time==1) 
	{
		e->_c.flags &= ~ISINKED;
		if(e==player) msg("the ink has faded");
	}
}

//static void _daemon_lichrespawn(_daemon *d)
//{
//	Entity *lich;
//	Entity *phylactery =get_target_type('x')[0];
//
//	if(phylactery)
//	{
//		lich=_new_monster('L');
//		lich->_c.form=LVULNERABLE;
//		placeat(lich,phylactery->pos.x, phylactery->pos.y);
//		clear_entity(phylactery);
//		msg("[TEMP] lich respawns");
//	}
//}

static void _daemon_pacify(_daemon* d)
{
	Entity* e=cid(d->c_id);
	e->_c.flags &= ~ (ISAGRO|ISFOLLOW);
	if(d->time==1)
	{
		if(monsters[e->_c.type-'A'].flags&ISAGRO) e->_c.flags|=ISAGRO;//|ISFOLLOW);
	}

}

static void _daemon_slowdeath(_daemon* d)
{
	Entity* e=cid(d->c_id);
	if(d->time==1)
	{
		e->_c.stat.hp=0;
	}
}

/*
static void _daemon_wyvernpoison(_daemon* d)
{
	//TBD
	Entity* e=cid(d->c_id);
	if(d->time==1)
	{
	}
}
*/

static void _daemon_dragonbreath(_daemon* d)
{
	Entity* e=cid(d->c_id);
	e->_c.flags |= ISBREATH;
	if(d->time==1)
	{
		e->_c.flags &= ~ISBREATH;
	}

}

static void _daemon_stun(_daemon* d)
{
	Entity* e=cid(d->c_id);
	e->_c.flags |= ISSTUN;
	if(d->time==1)
	{
		e->_c.flags &= ~ISSTUN;
	}
}

static void _daemon_tutorial(_daemon* d)
{
	//Entity* e=cid(d->c_id);
	if(wizardmode) d->time=1;
	if(d->time==1)
	{
	}
}

static void _daemon_phoenix(_daemon *d)
{

	if(d->time==1)
	{
		int placed=0;
		int attempts=0;
		while(!placed && attempts<100)
		{
			int id=rng(XMAX*YMAX);
			if( db.tiles[id].flags&(ML_VISIBLE|~ML_OBSTRUCTS)) 
			{
				Entity *p=_new_monster('P');
				p->_c.flags|=CANTRACK;
				p->pos=(coord){id%XMAX, id/XMAX, db.cur_level};
				db.tiles[id].creature=p;
				placed=1;
			}
			attempts++;
		}

	}
}


static void _daemon_arcanebarrier(_daemon *d)
{
	//int def = 5+ (int)round( (float)(spell_info[ARCANEBARRIER].nuses)/5.0);
	int def=10;
	Entity* e=cid(d->c_id);
	e->_c.flags|=ISARCANE;
	if(d->time==1)
	{
		e->_c.stat.def-=def;
		e->_c.stat.res-=def;
		e->_c.flags&=~ISARCANE;
		if(e==player) msg("your arcane barrier fades away");
	}
}

static void _daemon_absorption(_daemon *d)
{
	Entity* e=cid(d->c_id);
	e->_c.flags|=ISARCANE;
	if(d->time==1)
	{
		e->_c.flags&=~ISARCANE;
		if(e==player)msg("the arcane absorption protective layer shatters");
	}
}


int add_daemon(Entity *e, int type, int duration)
{
	int id=0;
	_daemon *d, *end=&db.daemons[NDAEMONS];
	for(d=&db.daemons[0]; d<end; d++,id++)
	{
		if(d->type==D_NONE)
		{

			d->c_id= e?e->id:-1; //no deference check
			d->type=type;
			d->time=duration;
			break;
		}
	}
	update_daemon(d); // might not want to do this, well see
	return id;
}
void update_daemon(_daemon *d)
{
	if(d)
	{
		switch(d->type)
		{
			case D_POISON: _daemon_poison(d); break;
			case D_BURN: _daemon_burn(d); break;
			case D_FASTREGEN: _daemon_fastregen(d); break;
			case D_VULNERABILITY: _daemon_vulnerability(d); break;
			case D_FREEZE: _daemon_freeze(d); break;
			case D_BIND: _daemon_bind(d); break;
			case D_BLIND: _daemon_blindness(d); break;
			case D_HASTE: _daemon_haste(d); break;
			case D_SLOW: _daemon_slow(d); break;
			case D_LSD: _daemon_lsd(d); break;
			case D_INK: _daemon_ink(d); break;
			case D_PACIFY: _daemon_pacify(d); break;
			case D_SLOWDEATH: _daemon_slowdeath(d); break;
			case D_DRAGONBREATH: _daemon_dragonbreath(d); break;

			case D_CONFUSE: _daemon_confuse(d); break;
			case D_INVIS: _daemon_invisibilty(d); break;
			case D_SLEEP: _daemon_sleep(d);break;
			case D_STUN: _daemon_stun(d); break;
			case D_TUTORIAL: _daemon_tutorial(d); break;
			case D_SPAWNPHOENIX: _daemon_phoenix(d); break;

			case D_ARCANEBARRIER: _daemon_arcanebarrier(d); break;
			case D_ABSORPTION: _daemon_absorption(d); break;
		}
		if((--d->time)<=0) end_daemon(d);
	}
}
void end_daemon(_daemon *d)
{
	if(d && (d->c_id>=0 && d->c_id<DBSIZE_CREATURES))
	{
		switch(d->type)
		{
			case D_REGEN: _daemon_end_regen(d); break;
			case D_STRENGTH: _daemon_end_str(d); break;
			default: break;
		}
		stop_daemon(d);
	}
}

void stop_daemon(_daemon *d)
{
	//force quit early
	d->c_id=-1;
	d->type=0;
	d->time=0;
}

_daemon *search_daemon(Entity *e, int type)
{
	_daemon *d;
	for(d=&db.daemons[0]; d<&db.daemons[NDAEMONS]; d++)
	{
		if(d->c_id==e->id && d->type==type) return d;
	}
	return NULL;
}
