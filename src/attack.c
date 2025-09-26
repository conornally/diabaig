#include "diabaig.h"

static void do_hitmsg(Entity *e, Entity *target);
static void do_missmsg(Entity *e, Entity *target);
static void do_critmsg(Entity *e, Entity *target);


static int do_special_hit(Entity *e, Entity *target);
static int _special_hit_bat(Entity *e, Entity *target);
static int _special_hit_harpy(Entity *e, Entity *target);
static int _special_hit_player(Entity *e, Entity *target);
static int _special_hit_spider(Entity *e, Entity *target);
static int _special_hit_phantom(Entity *e, Entity *target);
static int _special_hit_vampire(Entity *e, Entity *target);
static int _special_hit_Acidblob(Entity* e, Entity* target);
static int _special_hit_FellBeast(Entity* e, Entity* target);
static int _special_hit_GateKeeper(Entity *e, Entity *target);
static int _special_hit_Dragon(Entity *e, Entity *target);
static int _special_hit_Lindworm(Entity *e, Entity *target);
static int _special_hit_xolo(Entity *e, Entity *target);
static int _special_hit_xolo(Entity *e, Entity *target);
static int _special_hit_Quetzalcoatl(Entity *e, Entity* target);

static int do_special_def(Entity *e, Entity *target);
static int _special_def_player(Entity *e, Entity *target);
static int _special_def_mimic(Entity *e, Entity *target);
static int _special_def_phantom(Entity *e, Entity *target);
static int _special_def_vampire(Entity *e, Entity *target);
static int _special_def_yeti(Entity *e, Entity *target);
static int _special_def_Hydra(Entity* e, Entity* target);
static int _special_def_Dragon(Entity* e, Entity* target);
static int _special_def_Acidblob(Entity* e, Entity* target);

static int do_special_miss(Entity *e, Entity *target);
static int _special_miss_player(Entity *e, Entity *target);
static int _special_miss_bat(Entity *e, Entity *target);
static int _special_miss_vampire(Entity *e, Entity *target);
static int _special_miss_Tezcatlipoca(Entity *e, Entity *target);


#define HITMSGSZ 6
#define MISSMSGSZ 6
#define DEFEATMSGSZ 2

static char *_hitmsg[]={//HITMSGSZ]={
	"%s lands a powerful blow on %s",
	"%s scored an excellent hit on %s",
	"%s hits %s",
	"%s strikes %s",
	"%s has injured %s",
	"%s swings and hits %s",
	0
};

static char *_missmsg[]={
	"%s narrowly missed %s",
	"%s's strike was narrowly evaded by %s",
	"%s's lunge was deftly sidestepped by %s",
	"%s misses %s",
	"%s swings and misses %s",
	"%s is evaded by %s",
	0
};

static char *_critmsg[]={
	"%s lands a devastating strike",
	"%s lands a critical hit",
	"%s's attack hits a critical area",
	0
};

static char *body_parts[]={
	"arm","leg","forearm","thigh","calf","hand","foot",
	0
};

static void _hit(Entity *e, Entity *target)
{
	int total_str;
	int dmg;

	//total_def=target->_c.stat.def;
	total_str= diceroll(e->_c.stat.str);

	_daemon *d=search_daemon(target,D_SLEEP);
	if(d)
	{
		target->_c.flags &= (~ISSLEEP);
		d->time=1;
	}
	
	if((d=search_daemon(target,D_PACIFY))) d->time=1;

	if(e==player)
	{
		if(db.cur_mainhand!=-1) _do_weapon_effect(&db.objects[db.cur_mainhand],target, 1);
		if(db.cur_offhand!=-1) _do_weapon_effect(&db.objects[db.cur_offhand],target, 1);
	}
	dmg= modify_damage(target,total_str, DMG_PHYSICAL);
	target->_c.stat.hp-=dmg;
}

int melee(Entity *e, Entity *target)
{
	int status=RETURN_UNDEF;
	int e_agile, target_agile;
	if(e==target) return RETURN_UNDEF;

	bool hit, iscritical=false;

	if(e && target)
	{
		if((target->_c.flags & ISFRIEND) && e==player) return interact(e,target);
		if((e->_c.flags&ISFRIEND) && target==player) return interact(e,target);

		e_agile=rng(e->_c.stat.dex);
		target_agile=rng(target->_c.stat.dex);

		if(target->_c.flags&ISSLEEP || e->_c.flags&ISINVIS) target_agile=0;
		//if(e_agile==(e->_c.stat.dex-1)) iscritical=true; 
		if( !rng(CRITMAX - e->_c.stat.dex + target->_c.stat.dex ) ) iscritical=true;

		if(e_agile==0) hit=false;//automatically miss
		else if(target_agile==0)
		{
			hit=true;//automatically hit
			iscritical=true;
		}
		else if(target_agile<=e_agile) hit=true;
		else hit=false;

		e->_c.stamina--;
		char *name1=strdup(getname(e));
		//char *name2=getname(target);
		if(hit) 
		{
			if(!do_special_hit(e,target)) //if special fails
			{
				_hit(e,target);
				do_hitmsg(e,target);
			}
			if(iscritical)
			{
				_hit(e,target);
				//msg("%s crit",getname(e));
				//player_msg(e,"a critical hit!");
				do_critmsg(e,target);
			}

			do_special_def(e,target); //most wont have one
									  //DOES THIS PRINT MSG?
		}
		else
		{
			if(!do_special_miss(e,target)) do_missmsg(e,target);
		}
		if(target->_c.stat.hp<=0) murder(e,target);
		status=RETURN_SUCCESS;
		free(name1);
	}

	return status;
}

int knockback(Entity *e, Entity *target, float strength, int direction)
{
	int status=RETURN_FAIL;
	int walldamage=0;
	coord p,safe;
	Entity *chain=NULL;

	if(e && target)
	{
		animation a={A_PROJ,target->_c.type, 0, {0,0,0},{0,0,0},0};
		a.pos=target->pos;
		p=target->pos;
		safe=p;
		moat(target->pos.x, target->pos.y)=NULL;

		if(target->_c.flags&ISFLY) strength*=1.5;

		while( (strength--)>0 && islegal(p) )
		{
			safe=p;
			switch(direction)
			{
				case north: p.y-- ; break;
				case south: p.y++ ; break;
				case west: p.x-- ; break;
				case east: p.x++ ; break;
				case northeast: p.y-- ;  p.x++; break;
				case northwest: p.y-- ;  p.x--; break;
				case southeast: p.y++ ;  p.x++; break;
				case southwest: p.y++ ;  p.x--; break;
			}

			// CHAIN KNOCKBACK
			chain=moat(p.x,p.y);
			if( chain && chain!=target) 
			{
				target->pos=safe; //first place the target
				moat(safe.x, safe.y)=target;
				knockback(target, chain, strength/2, direction);
				break;
			}
			chain=NULL;

			if(obstructs(p.x,p.y)) walldamage=1;
		}

		if(walldamage)
		{
			// This should maybe go through dmgmod
			target->_c.stat.hp-=fabs(strength);
			add_daemon(target, D_STUN, 4);
			if(e==player) msg("%s hits the wall, the shock stuns it momentarily",getname(target));
		}
		else msg("a powerful strike sends %s flying",getname(target));

		//place the target
		target->pos=safe; 
		moat(safe.x, safe.y)=target;

		//Maybe i want this to occur elsewhere
		a.target_pos=safe;
		//add_animation(a);
		animate(&a);

		status=RETURN_SUCCESS;
	}
	return status;
}




static int do_special_hit(Entity *e, Entity *target)
{
	bool success=false;
	switch(e->_c.type)
	{
		case '@': success=_special_hit_player(e,target); break;
		case 'b': success=_special_hit_bat(e,target); break;
		case 'h': success=_special_hit_harpy(e,target); break;
		case 's': success=_special_hit_spider(e,target); break;
		case 'p': success=_special_hit_phantom(e,target); break;
		case 'v': success=_special_hit_vampire(e,target); break;
		case 'x': success=_special_hit_xolo(e,target); break;

		case 'A': success=_special_hit_Acidblob(e,target); break;
		case 'F': success=_special_hit_FellBeast(e,target); break;
		case 'G': success=_special_hit_GateKeeper(e,target); break;
		case 'D': success=_special_hit_Dragon(e,target); break;
		case 'L': success=_special_hit_Lindworm(e,target); break;

		case 'Q': success=_special_hit_Quetzalcoatl(e,target); break;
		//case 'T': success=_special_hit_Tezcatlipoca(e,target); break;

		default: break;
	}
	return success;
}

static int _special_hit_bat(Entity *e, Entity *target)
{
	int success=false;
	if(e&&target->_c.type=='b')
	{
		//if(!rng(2))if(!rng(2))msg("there is a flurry of bats");
		success=true;
	}
	return success;
}

static int _special_hit_harpy(Entity *e, Entity *target)
{
	int success=false;
	Entity *item;
	if(e && target==player && !rng(10))
	{
		for(int id=0; id<26; id++)
		{
			if(db.inventory[id]!=-1)
			{
				item=&db.objects[db.inventory[id]];
				if(item->_o.type==FOOD)
				{
					item=split_stack(item);
					msg("a harpy stole %s",getname(item));
					success=true;
				}
			}
		}
	}
	return success;
}


static int _special_hit_spider(Entity *e, Entity *target)
{
	int success=false;
	if(e && target && rng(5))
	{
		if(!(target->_c.flags&ISPOISON))
		add_daemon(target,D_POISON,10);
		success=true;
		if(target==player) msg("%s bites %s",getname(e),getname(target));
	}
	return success;
}
static int _special_hit_player(Entity *e, Entity *target)
{
	if(target) target->_c.flags|=(ISAGRO|SEENPLAYER|ISFOLLOW);

	if(e && e->_c.flags & ISINVIS)
	{
		_daemon *d=search_daemon(player, D_INVIS);
		if(d) end_daemon(d);
		e->_c.flags &= (~ISINVIS);
	}
	//knockback( e, target, 10, getdirection( e->pos, target->pos) );
	return false;
}


static int _special_hit_phantom(Entity *e, Entity *target)
{
	if(e && target)
	{
		e->_c.flags &= ~ISINVIS;
		if(target==player) msg("%s suddenly reappears",getname(e));
	}
	return false;
}

static int _special_hit_vampire(Entity *e, Entity *target)
{
	int success=false;
	if(e && target && !rng(VAMPBITE))
	{
		int dhp=2+rng(3);
		target->_c.stat.maxhp-=dhp;
		target->_c.stat.hp= MIN( target->_c.stat.hp, target->_c.stat.maxhp);
		add_daemon(e,D_FASTREGEN, dhp);

		success=true;
		if(target==player) msg("%s bites you! you feel your life draining",getname(e));
	}
	return success;
}

static int _special_hit_xolo(Entity *e, Entity *target)
{
	int success=false;
	if(e && target && !rng(3))
	{
		target->_c.stat.hp= MIN( target->_c.stat.hp, --target->_c.stat.maxhp);
		success=true;
		if(target==player) msg("%s bites you! you feel an emptiness",getname(e));
	}
	return success;
}

static int _special_hit_Acidblob(Entity* e, Entity* target)
{
	int id=db.cur_armour;
	Entity *item;
	int success=false;
	if(e && target==player && id>=0)
	{
		item=&db.objects[id];
		if(item && (item->flags&ISACTIVE) && !rng(RUSTRATE))
		{
			switch(item->_o.which)
			{
				case CLOTH: case LEATHER: case MITHRIL:
					msg("the acidic body of %s has no effect on your %s",getname(target), armour_info[item->_o.which].obj_name);
					break;
				default:
					disenchant(item);
					msg("oh no! the acidic body of %s degrades your %s!",getname(target),armour_info[item->_o.which].obj_name);

					break;
			}
			success=true;
		}
	}
	return success;
}


static int _special_hit_FellBeast(Entity *e, Entity *target)
{
	int success=false;
	if(e && target && !rng(FELLCHANCE))
	{
		target->_c.stat.str[1] = MAX( target->_c.stat.str[1]-1,0 );
		if(target==player)
		{
			msg("%s bites you! you feel weak from the lingering pain",getname(e));
		}
		success=true;
	}
	return success;
}

static int _special_hit_GateKeeper(Entity *e, Entity *target)
{
	int success=false;
	if(e && target && !rng(5))
	{
		knockback(e,target,3,getdirection(e->pos,target->pos));
		success=true;
	}
	return success;
}

static int _special_hit_Dragon(Entity *e, Entity *target)
{
	int success=false;
	if(e && target && !rng(10))
	{
		knockback(e,target,3,getdirection(e->pos,target->pos));
		success=true;
	}
	return success;
}

static int _special_hit_Lindworm(Entity *e, Entity *target)
{
	if(e && target && !rng(5))
	{
		add_daemon(target, D_POISON, 5+rng(5));
		if(target==player) msg("%s sinks its fangs into your %s",getname(e),body_parts[ rng(clen(body_parts))]);
	}
	return 0;
}

static int _special_hit_Quetzalcoatl(Entity *e, Entity* target)
{
	int success=false;
	if(e && target && !rng(5))
	{
		knockback(e,target,3+rng(3), getdirection(e->pos, target->pos));
		success=true;
	}
	return success;
}

static int do_special_def(Entity *e, Entity *target)
{
	int success=false;
	switch(target->_c.type)
	{
		case '@': success=_special_def_player(e,target); break;
		case 'm': success=_special_def_mimic(e,target);break;
		case 'p': success=_special_def_phantom(e,target);break;
		case 'v': success=_special_def_vampire(e,target);break;
		case 'y': success=_special_def_yeti(e,target);break;

		case 'H': success=_special_def_Hydra(e,target);break;
		case 'D': success=_special_def_Dragon(e,target);break;
		case 'A': success=_special_def_Acidblob(e,target);break;
		default: break;
	}
	return success;
}

static int _special_def_player(Entity *e, Entity *target)
{
	//char* name1=strdup(getname(e));
	//msg(_hitmsg[rng(HITMSGSZ)],name1,getname(target));
	//free(name1);
	
	if(e && target) set_ripdata(RIP_KILLED,getname(e));
	return false;
}

static int _special_def_mimic(Entity *e, Entity *target)
{
	int success=false;
	if(e==player) //if attacker is the player
	{
		if(target && target->_c.form)
		{
			target->_c.form=0; //undisguise sort of
			memset(message,'\0',sizeof(message));
			msg("oh no! it's a mimic");
			_hit(target,e); //garuanteed hit (for now)
		}
	}
	return success;
}

static int _special_def_phantom(Entity *e, Entity *target)
{
	int success=false;
	if(e&&target)
	{
		if(target->_c.flags & ISINVIS)
		{
			target->_c.flags &= ~ISINVIS;
		}
		else
		{
			if(e==player) msg("%s vanishes",getname(target));
			moat(target->pos.x,target->pos.y)=NULL;
			target->_c.flags |= ISINVIS;
			if(target->_c._inroom)
			{
				placeinroom(target->_c._inroom, target);
				success=true;
			}
			else
			{
				int placed=0;
				for(int x=e->pos.x-5; x<e->pos.x+5;x++)
					for(int y=e->pos.y-5; y<e->pos.y+5;y++)
					{
						coord p={x,y,e->pos.z};
						if(!placed && islegal(p) && !moat(x,y))
						{
							moat(x,y)=target;
							placed=1;
							success=true;
						}
					}
			}
		}
		if(target->_c.stat.hp<=0) target->_c.flags &=~ISINVIS;
	}
	return success;
}

static int _special_def_vampire(Entity *e, Entity *target)
{
	int success=false;
	if(e&&target)
	{
		if(target->_c.form==vBAT)
		{
			player_msg(e,"a bat transforms into a vampire");//,getname(target));
			target->_c.flags &= ~ISFLY;
			target->_c.form=DEFAULTFORM;
			success=true;
		}
	}
	return success;
}

static int _special_def_yeti(Entity *e, Entity *target)
{
	int success=false;
	if(e&&target && !rng(5))
	{
		char* name=strdup(getname(target));
		if(e==player) msg("you can't withstand the cold eminating from %s",name);
		free(name);

		add_daemon(e,D_FREEZE,2+rng(3));
		success=true;
	}
	return success;
}

static int _special_def_Hydra(Entity* e, Entity* target)
{
	int success=false;
	int nheads=0;
	char* name;
	if(e&&target && !rng(2))
	{
		name=strdup(getname(target));
		nheads=--target->_c.stat.str[0];
		if(nheads>0) 
		{
			if(e==player)
				msg("you sever one of the Hydras heads, it has %d left",nheads);
		}
		else
		{
			msg("you destroy the final head of the Hydra");
			target->_c.stat.hp=0;
		}
		free(name);
		success=true;
	}
	return success;
}

static int _special_def_Dragon(Entity* e, Entity* target)
{
	int success=false;
	if(e&&target && !rng(30))
	{
		Entity *item=_new_obj(GOLD);
		if(item)
		{
			item->_o.which=1;
			item->_o.quantity=20+rng(50); //a little lower?
			item->pos=e->pos;
			objat(e->pos.x, e->pos.y)=item;
			msg("the strike chips a scale from %s's body",dragonname);
			success=true;
		}
	}
	return success;
}

static int _special_def_Acidblob(Entity *e,Entity *target)
{
	int id=db.cur_mainhand;
	Entity *item;
	int success=false;
	if(e==player && id>=0 && target)
	{
		item=&db.objects[id];
		if(item && (item->flags&ISACTIVE) && !rng(RUSTRATE))
		{
			switch(item->_o.which)
			{
				case STICK: case SHORTBOW: case LONGBOW: case RECURVEBOW:
					msg("the acidic body of %s has no effect on your %s",getname(target), weapon_info[item->_o.which].obj_name);
					break;
				default:
					disenchant(item);
					msg("oh no! the acidic body of %s degrades your %s!",getname(target),weapon_info[item->_o.which].obj_name);
					animation a={A_AOE,'"',COLOR_PAIR(C_MAGENTA), e->pos, {0,0,0},0};
					a.pos=e->pos;
					a.pos.z=2;
					//add_animation(a);
					animate(&a);
					break;
			}
			success=true;
		}
	}
	return success;
}

static int do_special_miss(Entity *e, Entity *target)
{
	int success=false;
	if(e && target)
	{
		switch(e->_c.type)
		{
			case '@': success=_special_miss_player(e,target); break;
		}
		switch(target->_c.type)
		{
			case '@': success=_special_miss_player(e,target); break;
			case 'b': success=_special_miss_bat(e,target); break;
			case 'm': success=_special_def_mimic(e,target); break;
			case 'v': success=_special_miss_vampire(e,target); break;
			case 'T': success=_special_miss_Tezcatlipoca(e,target); break;
		}
	}
	return success;
}
static int _special_miss_player(Entity *e, Entity *target)
{
	//char *name1=strdup(getname(e));
	//msg(_missmsg[rng(MISSMSGSZ)],name1,getname(target));
	//free(name1);
	if(e==player && target) target->_c.flags|=(ISAGRO|SEENPLAYER|ISFOLLOW);
	return false;
}

static int _special_miss_bat(Entity *e, Entity *target)
{
	int success=false;
	if(e&&target && e->_c.type=='b')
	{
		//if(!rng(2)) msg("you hear bats fluttering around");
		success=true;
	}
	return success;
}

static int _special_miss_vampire(Entity *e, Entity *target)
{
	int success=false;
	coord pos;
	Entity *bat;
	if(e&&target)
	{
		do
		{
			pos.x= target->pos.x-5 + rng(10);
			pos.y= target->pos.y-5 + rng(10);
			pos.z= target->pos.z;
		}while( !islegal(pos) );//&& moat(pos.x,pos.y) );

		bat=_new_monster('b');
		if(bat)
		{
			msg("a %s vanishes in a flurry or bats",monsters[target->_c.type-'A'].monster_name);
			moat(target->pos.x, target->pos.y)=bat;

			bat->pos=target->pos;
			target->pos=pos;
			moat(pos.x,pos.y)=target;
			target->_c.form=vBAT;
			target->_c.flags|=ISFLY;
		}
		success=true;
	}
	return success;
}

static int _special_miss_Tezcatlipoca(Entity *e, Entity *target)
{
	coord p=target->pos;
	int success=false, attempts=0;
	if(e&&target && !rng(2))
	{
		do{
			p.x= target->pos.x +rng(6)-4;
			p.y= target->pos.y +rng(4)-2;
			attempts++;
		}while( !islegal(p) && attempts<100);
		if(attempts<100)
		{
			moat(target->pos.x,target->pos.y)=NULL;
			moat(p.x,p.y)=target;
			target->pos=p;
			msg("%s teleports around the room",getname(target));
			success=true;
		}
	}
	return success;
}


static void do_hitmsg(Entity *e, Entity *target)
{
	if(e&&target)
	{
		if(tileat(e->pos.x,e->pos.y)->flags&ML_VISIBLE || target==player)
		{
			char *name=strdup(getname(e));
			msg(_hitmsg[rng(clen(_hitmsg))],name, getname(target));
			free(name);

		}
	}
}

//static void do_defmsg(Entity *e, Entity *target)
//{
//
//}

static void do_missmsg(Entity *e, Entity *target)
{
	if(e&&target)
	{
		if(tileat(e->pos.x,e->pos.y)->flags&ML_VISIBLE)
		{
			char *name=strdup(getname(e));
			msg(_missmsg[rng(clen(_missmsg))],name, getname(target));
			free(name);
		}
	}
}
static void do_critmsg(Entity *e, Entity *target)
{
	if(e&&target)
	{
		if(tileat(e->pos.x,e->pos.y)->flags&ML_VISIBLE)
		{
			//char *name=strdup(getname(e));
			msg(_critmsg[rng(clen(_critmsg))], getname(e));
			//free(name);
		}
	}
}
