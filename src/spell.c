#include "diabaig.h"

static void master_spell(spell *sp, int slot);
static void _do_unlearn_spell(int slot);
static void _do_learn_spell(int type, int slot);

static void _spell_bandagewounds(); //small fast regen
static void _spell_pacify();   	//Unset ISAGRO
static void _spell_polymorph(); //change type
static void _spell_disarm(); 	//reduce attak a bit
static void _spell_hypnotise(); //sleep 
static void _spell_sonicboom(); //damage and confuse
 
static void _spell_firebolt();
static void _spell_firestorm();

static void _spell_frostbite();
static void _spell_blizzard();

static void _spell_bindingvines();
static void _spell_tanglingmangrove();

static void _spell_heal(); 
static void _spell_holyheal(); //cureall + fast regen

static void _spell_repel(); 	//knockback 1
static void _spell_shockwave(); //knockback circle

static void _spell_slice(); 	  //hit 3 enemies
static void _spell_twirlstrike(); //hit all adjacent enemies
static void _spell_whirlwind(); //hit all adjacent enemies

//static void _spell_block();	  //block some attacks?
//static void _spell_fortify(); //block everything? for a bit?
static void _spell_arcanebarrier();
static void _spell_arcaneabsorption();


int pick_spell()
{
	_spell_info *info=spell_info, *end;

	for(end=&spell_info[MAXSPELL]; info<end; info++)
	{
		int i=rng(100);
		if(!info->known && i<info->prob) break;
		if(info==&spell_info[MAXSPELL-1]) info=spell_info;
	}
	return (int)(info-spell_info);
}

static void _do_unlearn_spell(int slot)
{
	spell *sp=&db.spells[slot];
	_spell_info *info;
	if(sp->type!=SP_NOTLEARNT)
	{
		info=&spell_info[ sp->type ];
		info->known=false;
	}
}
static void _do_learn_spell(int type, int slot)
{
	spell *sp=&db.spells[slot];
	_spell_info *info=&spell_info[type];
	sp->type=type;
	sp->cooldown=info->cooldown;
	sp->charge=info->cooldown;
	info->known=true;
}

int learn_spell(int type)
{
	if(type>=MAXSPELL) return RETURN_FAIL;
	int status=RETURN_UNDEF;
	int slot=-1,tmp;
	//spell *sp;
	//_spell_info *info;
	if(db.spells[0].type==SP_NOTLEARNT) slot=0;
	else if(db.spells[1].type==SP_NOTLEARNT) slot=1;
	else if(db.spells[2].type==SP_NOTLEARNT) slot=2;
	else
	{
		msg("select slot to overwrite with \"%s\" (1,2,3,x):",spell_info[type].spell_name);
		display();
		switch((tmp=getch()))
		{
			case '1':case'2':case'3': slot=tmp-'1'; break;
			default:
					msg("the scroll vanishes");
					status=RETURN_FAIL;
					break;
		}
	}
	_log("slot %d, name %s",slot, spell_info[type].spell_name);
	if(slot>-1)
	{
		_do_unlearn_spell(slot);
		_do_learn_spell(type,slot);
		status=RETURN_SUCCESS;
	}
	return status;
}

int cast_spell(int slot)
{
	int status=RETURN_SUCCESS;
	spell *sp;
	if(slot>2 || db.spells[slot].type==SP_NOTLEARNT) return RETURN_FAIL;

	sp=&db.spells[slot];
	if(sp->charge!=sp->cooldown) 
	{
		msg("%s not ready",spell_info[sp->type].spell_name);
		return RETURN_FAIL;
	}

	switch(sp->type)
	{
		case SP_HEAL: _spell_heal(); break;
		case SP_BANDAGEWOUNDS: _spell_bandagewounds(); break;
		case SP_FIREBOLT: _spell_firebolt(); break;
		case SP_FROSTBITE: _spell_frostbite(); break;
		case SP_CALM: _spell_pacify(); break;
		case SP_POLYMORPH: _spell_polymorph(); break;
		case SP_DISARMING: _spell_disarm(); break;
		case SP_BINDINGVINES: _spell_bindingvines(); break;
		case SP_HYPNOTISE: _spell_hypnotise(); break;
		case SP_SONICBOOM: _spell_sonicboom(); break;

		case SP_FIRESTORM: _spell_firestorm(); break;
		case SP_BLIZZARD: _spell_blizzard(); break;
		case SP_TANGLINGMANGROVE: _spell_tanglingmangrove(); break;
		case SP_MAXHEAL: _spell_holyheal(); break;

		case SP_REPEL: _spell_repel(); break;
		case SP_SHOCKWAVE: _spell_shockwave(); break;

		case SP_SLICE:  	 _spell_slice(); break;
		case SP_TWIRLSTRIKE: _spell_twirlstrike(); break;
		case SP_WHIRLWIND: 	 _spell_whirlwind(); break;

		case SP_ARCANEBARRIER: _spell_arcanebarrier(); break;
		case SP_ARCANEABSORPTION: _spell_arcaneabsorption(); break;
		default: msg("spell has no effect");break;
	}
	sp->charge=0;
	spell_info[sp->type].nuses++;

	if( spell_info[sp->type].nuses>= spell_info[sp->type].mastery )
	{
		master_spell(sp, slot);
	}
	return status;
}

static void master_spell(spell *sp, int slot)
{
	// certain spells can have "mastery", e.g. firebolt becomes firestorm
	// not every sell will have an advancement
	int newsp;
	switch(sp->type)
	{
		case SP_FIREBOLT: newsp=SP_FIRESTORM; break;
		case SP_FROSTBITE: newsp=SP_BLIZZARD; break;
		case SP_BINDINGVINES: newsp=SP_TANGLINGMANGROVE; break;
		case SP_HEAL: newsp=SP_MAXHEAL; break;

		case SP_REPEL: newsp=SP_SHOCKWAVE; break;
		case SP_SLICE: newsp=SP_TWIRLSTRIKE; break;
		case SP_TWIRLSTRIKE: newsp=SP_WHIRLWIND; break;
        case SP_ARCANEBARRIER: newsp=SP_ARCANEABSORPTION; break;
		default :newsp=-1; break;
	}
	if(newsp!=-1)
	{
		msg("%s masters \"%s\", and can now cast \"%s\"",getname(player), spell_info[sp->type].spell_name, spell_info[newsp].spell_name);
		_do_learn_spell(newsp,slot);
	}
}


static void _spell_heal()
{
	int dhp=(int)MAX(5,(player->_c.stat.maxhp/2));
	dhp+= (int)(0.5*spell_info[SP_HEAL].nuses);
	player->_c.stat.hp= MIN( player->_c.stat.hp+dhp, player->_c.stat.maxhp);
	//player->_c.stat.hp=player->_c.stat.maxhp;
	if(dhp<player->_c.stat.maxhp) msg("some of your wounds fade before your eyes");
	else msg("your wounds completely disappear");
}

static void _spell_bandagewounds()
{
	add_daemon(player,D_FASTREGEN,10+ spell_info[SP_BANDAGEWOUNDS].nuses);
	msg("you apply bandages to your wounds");
}

static void _spell_firebolt()
{
	int dir=pick_direction();
	Entity *target;//=get_target_direct(player,dir);
	tile *t=get_tile_direct(player,dir);
	target=t->creature;

	animation a=(animation){A_SHOOTDIR,'"',COLOR_PAIR(STATUS_BURN), player->pos, {0,0,0}, 0};
	a.pos=(coord){player->pos.x,player->pos.y,dir};
	//add_animation(a);
	animate(&a);

	if(target)
	{
		if(target->_c.res_flags & IMMUNE_FIRE) msg("the spell has no effect on %s",getname(target));
		else
		{
			int base_damage[2]={10,2};
			int damage=modify_damage(target,diceroll(base_damage), DMG_FIRE);
			//if(target->_c.res_flags & RESIST_FIRE) damage/=2;
			//if(target->_c.res_flags & WEAKTO_FIRE) damage*=2;

			target->_c.stat.hp-=damage;
			target->_c.flags |= ISBURN; //just cause daemon wont run till next update
			add_daemon(target,D_BURN,5);
			msg("fire erupts from your hands and %s bursts into flames",getname(target));
		}
	}
	t->air=COMBUST;
	t->air_pressure+=1;

}

static void _spell_frostbite()
{
	int dir=pick_direction();
	Entity *target;//=get_target_direct(player,dir);
	tile *t=get_tile_direct(player,dir);
	target=t->creature;

	animation a=(animation){A_SHOOTDIR,'"',COLOR_PAIR(STATUS_FREEZE)|A_BOLD,player->pos,{0,0,0},0};
	a.pos=(coord){player->pos.x,player->pos.y,dir};
	//add_animation(a);
	animate(&a);

	if(target)
	{
		if(target->_c.res_flags & IMMUNE_FROST) msg("the spell has no effect on %s",getname(target));
		else
		{
			int base_damage[2]={10,2};
			int damage=modify_damage(target,diceroll(base_damage), DMG_ICE);
			//if(target->_c.res_flags & RESIST_FROST) damage/=2;
			//if(target->_c.res_flags & WEAKTO_FROST) damage*=2;

			target->_c.stat.hp-=damage;
			target->_c.flags |= ISFREEZE; //just cause daemon wont run till next update
			add_daemon(target,D_FREEZE,5); 
			msg("your icy breath freezes %s",getname(target));
		}
	}
	t->air=MIST;
	t->air_pressure+=1;
}
static void _spell_pacify()
{
	int dir=pick_direction();
	Entity *target=get_target_direct(player,dir);

	animation a=(animation){A_SHOOTDIR,'"',COLOR_PAIR(C_WHITE),player->pos,{0,0,0},0};
	a.pos=(coord){player->pos.x,player->pos.y,dir};
	//add_animation(a);
	animate(&a);

	if(target && !(target->_c.flags&ISAPEX))
	{
		target->_c.flags &= ~(ISAGRO|ISFOLLOW); //just cause daemon wont run till next update
		add_daemon(target, D_PACIFY, 100+10*spell_info[SP_CALM].nuses);
		msg("%s ceases its hostility towards you",getname(target));
	}
}

static void _spell_polymorph()
{
	int dir=pick_direction();
	Entity *target=get_target_direct(player,dir);
	Entity *polymorph;
	int polymorph_type;

	animation a=(animation){A_SHOOTDIR,'"',COLOR_PAIR(C_WHITE),player->pos,{0,0,0},0};
	a.pos=(coord){player->pos.x,player->pos.y,dir};
	//add_animation(a);
	animate(&a);

	if(target)
	{
		if( target->_c.flags&ISAPEX )
		{
			msg("the spell is ineffective");
		}
		else
		{
			do
			{
				polymorph_type='A'+rng(MAXMONSTERS);
			} while((monsters[polymorph_type-'A'].level>get_efflevel()) || polymorph_type==target->_c.type);

			polymorph=_new_monster(polymorph_type);
			if(polymorph)
			{
				polymorph->_c.stat.hp= MIN(target->_c.stat.hp, polymorph->_c.stat.maxhp);
				polymorph->pos=target->pos;
				moat(polymorph->pos.x,polymorph->pos.y)=polymorph;

				char *targetname=strdup(getname(target));
				msg("%s morphs into %s",targetname, getname(polymorph));
				free(targetname);
				clear_entity(target);
			}
		}
	}
}

static void _spell_disarm()
{
	int dir=pick_direction();
	Entity *target=get_target_direct(player,dir);

	animation a=(animation){A_SHOOTDIR,'"',COLOR_PAIR(C_WHITE),player->pos,{0,0,0},0};
	a.pos=(coord){player->pos.x,player->pos.y,dir};
	//add_animation(a);
	animate(&a);

	if(target)
	{
		target->_c.stat.str[0]/=2;
		target->_c.stat.str[1]/=2;
		msg("you disarm %s",getname(target));
	}
}

static void _spell_bindingvines()
{
	int dir=pick_direction();
	Entity *target=get_target_direct(player,dir);

	animation a=(animation){A_SHOOTDIR,'"',COLOR_PAIR(C_GREEN)|A_BOLD,player->pos,{0,0,0},0};
	a.pos=(coord){player->pos.x,player->pos.y,dir};
	//add_animation(a);
	animate(&a);

	if(target)
	{
		add_daemon(target,D_BIND,10);
		msg("vines grow from the floor and trap %s",getname(target));
	}
}

static void _spell_hypnotise()
{
	Entity ** lst;
	int i=1;
	if(player->_c._inroom) lst=get_target_room(player);
	else lst=get_target_radius(player,3+spell_info[SP_HYPNOTISE].nuses);

	animation a=(animation){A_AOE,'~',COLOR_PAIR(C_WHITE),player->pos,{0,0,0},0};
	a.pos=(coord){player->pos.x,player->pos.y,XMAX}; //LARGE radius because it wont be seen outside a room
	//add_animation(a);
	animate(&a);

	while(lst[i])
	{
		lst[i]->_c.flags |= ISSLEEP;
		add_daemon(lst[i], D_SLEEP, 10+ spell_info[SP_HYPNOTISE].nuses);
		i++;
	}
	if(i==1) msg("the spell lands of deaf ears..");
	else msg("all those that hear your hypnotic words, fall asleep");
	free(lst);
}

static void _spell_sonicboom()
{
	int nuses=spell_info[SP_SONICBOOM].nuses;
	int radius=10;
	Entity **lst=get_target_radius(player,radius);
	int i=1;

	animation a=(animation){A_AOE,'~',COLOR_PAIR(C_WHITE),player->pos,{0,0,0},0};
	a.pos=(coord){player->pos.x,player->pos.y,radius};
	//add_animation(a);
	animate(&a);

	while(lst[i])
	{
		lst[i]->_c.stat.hp-=modify_damage(lst[i],rng(5+nuses),DMG_MAGIC);
		lst[i]->_c.flags |= ISCONFUSED;
		add_daemon(lst[i],D_CONFUSE, 20+(2*nuses));
		i++;
	}
	msg("you unleash a deafening shriek, disorientating all nearby creatures"); 
	free(lst);
}

static void _spell_firestorm()
{
	int radius=7+spell_info[SP_FIRESTORM].nuses;
	int i=1, dmg[2]={20,4};
	Entity **lst=get_target_radius(player,radius);

	animation a=(animation){A_AOE,'~',COLOR_PAIR(STATUS_BURN),player->pos,{0,0,0},0};
	a.pos=(coord){player->pos.x,player->pos.y,radius};
	//add_animation(a);
	animate(&a);

	while(lst[i])
	{
		if(!(lst[i]->_c.res_flags & IMMUNE_FIRE))
		{
			int damage=modify_damage(lst[i],diceroll(dmg),DMG_FIRE);
			//if(lst[i]->_c.res_flags & RESIST_FIRE) damage/=2;
			//if(lst[i]->_c.res_flags & WEAKTO_FIRE) damage*=2;

			lst[i]->_c.stat.hp-=damage;
			lst[i]->_c.flags |= ISBURN;
			add_daemon(lst[i],D_BURN,10);
			tileat(lst[i]->pos.x,lst[i]->pos.y)->air=COMBUST;
			tileat(lst[i]->pos.x,lst[i]->pos.y)->air_pressure+=0.5;;
			tileat(player->pos.x,player->pos.y)->air=SMOKE; //protect the player a tiny bit
			//tileat(player->pos.x,player->pos.y)->air_pressure+=0.1;
		}
		i++;
	}
	msg("fire explodes from your body and engulfs all nearby creatures");
	free(lst);
}

static void _spell_blizzard()
{
	int radius=7+spell_info[SP_FIRESTORM].nuses;
	int i=1, dmg[2]={20,4};
	Entity **lst=get_target_radius(player,radius);

	animation a=(animation){A_AOE,'~',COLOR_PAIR(STATUS_FREEZE),player->pos,{0,0,0},0};
	a.pos=(coord){player->pos.x,player->pos.y,radius};
	//add_animation(a);
	animate(&a);

	while(lst[i])
	{
		if(!(lst[i]->_c.res_flags & IMMUNE_FROST))
		{
			int damage=modify_damage(lst[i],diceroll(dmg), DMG_ICE);
			//if(lst[i]->_c.res_flags & RESIST_FROST) damage/=2;
			//if(lst[i]->_c.res_flags & WEAKTO_FROST) damage*=2;

			lst[i]->_c.stat.hp-=damage;
			lst[i]->_c.flags |= ISFREEZE;
			add_daemon(lst[i],D_FREEZE,10);
			tileat(lst[i]->pos.x,lst[i]->pos.y)->air=MIST;
			tileat(lst[i]->pos.x,lst[i]->pos.y)->air_pressure+=0.25;;
			tileat(player->pos.x,player->pos.y)->air=SMOKE; //protect the player a tiny bit
			//tileat(player->pos.x,player->pos.y)->air_pressure+=0.1;
		}
		i++;
	}
	msg("you conjure a bitter blizzard that freezes your foes");
	free(lst);
}

static void _spell_tanglingmangrove()
{
	int radius=7+spell_info[SP_FIRESTORM].nuses;
	int i=1;//, dmg[2]={20,4};
	Entity **lst=get_target_radius(player,radius);

	animation a=(animation){A_AOE,'~',COLOR_PAIR(C_GREEN)|A_BOLD,player->pos,{0,0,0},0};
	a.pos=(coord){player->pos.x,player->pos.y,radius};
	//add_animation(a);
	animate(&a);

	while(lst[i])
	{
		lst[i]->_c.flags |= ISBOUND;
		add_daemon(lst[i],D_BIND,100);

		msg("the room fills with twisting roots that bind all creatures");
		i++;
	}
	free(lst);
}

static void _spell_holyheal()
{
	player->_c.stat.hp=player->_c.stat.maxhp;
	for(_daemon *d=&db.daemons[0];d<&db.daemons[NDAEMONS];d++)
	{
		if(d->c_id==0)
		{
			if(	d->type==D_CONFUSE ||
				d->type==D_POISON  ||
				d->type==D_SLEEP   ||
				d->type==D_BURN	   ||
				d->type==D_FREEZE  ||
				d->type==D_BIND    ||
				d->type==D_SLOW    ||
				d->type==D_LSD)
				stop_daemon(d);
		}
	}
	player->_c.flags &= ~(ISCONFUSED| ISPOISON| ISSLEEP| ISBURN
						|ISFREEZE | ISBOUND |ISSLOW | ISLSD);
	add_daemon(player,D_FASTREGEN,20+ spell_info[SP_MAXHEAL].nuses);
	msg("your wounds are healed and you feel an aura of protection");
}

static void _spell_repel()
{	//knockback 1
	
	int direction=pick_direction();
	Entity *e=get_target_direct(player, direction);
	float strength=3 + (spell_info[SP_REPEL].nuses / 5.0);

	knockback(player,e,strength,direction);
}

static void _spell_shockwave()
{	//knockback circle
	Entity **lst=get_target_adjacent(player), *target;
	int i=0,dx,dy,direction;
	float strength=3+spell_info[SP_SHOCKWAVE].nuses/2;
	if(lst)
	{
		while(lst[i])
		{
			target=lst[i];
			dx=target->pos.x-player->pos.x;
			dy=target->pos.y-player->pos.y;
			if( dx==0 && dy==0) direction=nodir;
			else if( dx>=1 && dy==0 ) direction=east;
			else if( dx<=-1&& dy==0 ) direction=west;
			else if( dx==0 && dy>=1 ) direction=south;
			else if( dx==0 && dy<=-1) direction=north;

			else if( dx>=1 && dy<=-1) direction=northeast;
			else if( dx>=-1&& dy<=-1) direction=northwest;
			else if( dx>=1 && dy>=1 ) direction=southeast;
			else if( dx<=-1&& dy>=1 ) direction=southwest;

			knockback(player, target, strength, direction);
			i++;
		}
	}
	free(lst);
}

static void _spell_slice()
{ //hit 3 enemies
	int dir =pick_direction(), ldir,rdir;
	Entity *lst[3],*target;

	if(dir!=nodir)
	{
		ldir= (dir+7)%8;
		rdir= (dir+9)%8;

		lst[0]=get_target_direct(player, ldir);
		lst[1]=get_target_direct(player,  dir);
		lst[2]=get_target_direct(player, rdir);

		for(int i=0; i<3; i++)
		{

			target=lst[i];
			if(target && (abs( player->pos.x-target->pos.x)<=1) && (abs( player->pos.y-target->pos.y)<=1) )
			{
				melee(player,target);
			}
		}
		//msg( "%d %d %d",ldir,dir,rdir);
		animation a={A_SLICE, '|', COLOR_PAIR(C_WHITE), player->pos,{0,0,0},0};
		//a.pos=player->pos;
		a.pos.z=dir;
		//add_animation(a);
		animate(&a);

		msg("you slice in a sweeping arc");

	}
}

static void _spell_twirlstrike()
{	//hit all adjacent enemies
	Entity **lst=get_target_adjacent(player), *target;
	int i=0;
	if(lst)
	{
		while(lst[i])
		{
			target=lst[i];
			melee(player,target);
			i++;
		}
		msg("you plant your feet and spin, unleashing a powerful attack");
	}
	animation a={A_TWIRL,'|',COLOR_PAIR(C_WHITE),player->pos,{0,0,0},0};
	a.pos=player->pos;
	//add_animation(a);
	animate(&a);
	free(lst);
}

static void _spell_whirlwind()
{	//hit all adjacent enemies multiple times
	Entity **lst=get_target_adjacent(player);
	int i, n=(int)(3+spell_info[SP_WHIRLWIND].nuses/2);
	if(lst)
	{
		for(int j=0; j<n; j++)
		{
			i=0;
			while(lst[i] && (lst[i]->flags&ISACTIVE))
			{
				melee(player,lst[i]);
				i++;
			}
		}
		msg("you unleash a whirlwind of slicing attacks");
	}

	for(int i=0; i<n; i++)
	{
		animation a={A_TWIRL,'|',COLOR_PAIR(C_WHITE),player->pos,{0,0,0},0};
		a.pos=player->pos;
		//add_animation(a);
		animate(&a);
	}
	free(lst);
}

static void _spell_arcanebarrier()
{
	//int def = 5+ (int)round( (float)(spell_info[ARCANEBARRIER].nuses+1)/5.0);
	int def=10;
	add_daemon(player, D_ARCANEBARRIER, 20+spell_info[SP_ARCANEBARRIER].nuses);
	msg("you conjure a protective barrier around your body");

	animation a={A_AOE,'.',A_BOLD|COLOR_PAIR(STATUS_ARC),player->pos,{0,0,0},0};
	a.pos=player->pos;
	a.pos.z=4;

	//add_animation(a);
	animate(&a);


	player->_c.stat.def+=def;
	player->_c.stat.res+=def;
}

static void _spell_arcaneabsorption()
{
	add_daemon(player, D_ABSORPTION, 40+(5*spell_info[SP_ARCANEBARRIER].nuses));
	msg("you form an arcane sheild that absorbs damage");

	animation a={A_AOE,'.',A_BOLD|COLOR_PAIR(STATUS_ARC),player->pos,{0,0,0},0};
	a.pos=player->pos;
	a.pos.z=4;

	//add_animation(a);
	animate(&a);
}
