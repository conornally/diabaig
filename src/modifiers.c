#include "diabaig.h"

#define MOD_MAGIC 15.0
#define MOD_PHYSICAL 15.0

static int modify_physical_damage(Entity *target, int damage);
static int modify_magic_damage(Entity *target, int damage);
static int modify_elemental_damage(Entity *target, int damage, int type);


int modify_damage(Entity *target, int damage, int type)
{
	int dmg=0;
	switch(type)
	{

		case DMG_PHYSICAL: dmg=modify_physical_damage(target, damage); break;
		case DMG_MAGIC:    dmg=modify_magic_damage(target, damage); break;
		case DMG_FIRE: case DMG_ICE: case DMG_POISON:
			dmg=modify_elemental_damage(target, damage, type); break;

	}

	_daemon *d=search_daemon(target, D_ABSORPTION);
	if(d)
	{
		d->time=MAX(d->time-dmg,2);
		dmg=0;
		if(target==player)
		{
			animation a={A_AOE,'*',COLOR_PAIR(STATUS_ARC),player->pos, {0,0,0}, 0};
			a.pos=target->pos;
			a.pos.z=3;
			if(d->time>2) 	msg("your arcane shield absorbs the damage");
			else			msg("the arcane absorption protective layer shatters");
			//add_animation(a);
			animate(&a);
		}
	}
	return dmg;
}

static int modify_physical_damage(Entity *target, int damage)
{
	int def=target->_c.stat.def;
	int dmg=0;

	if(target==player)
	{
	   if(db.cur_armour!=-1) 	def+= db.objects[db.cur_armour]._o.mod_def;
	   if(db.cur_offhand!=-1) 	def+= db.objects[db.cur_offhand]._o.mod_def; //shield
	   if(db.cur_mainhand!=-1) 	def+= db.objects[db.cur_mainhand]._o.mod_def; //shield
	}
	if(target->_c.flags&ISVULN) def/=2;
	dmg= (int)ceil( damage* (MOD_PHYSICAL/(MOD_PHYSICAL+def)) );

	return dmg;
}

static int modify_magic_damage(Entity *target, int damage)
{
	int res=target->_c.stat.res;
	int dmg=0;
	if(target==player)
	{
	   if(db.cur_armour!=-1) 	res+= db.objects[db.cur_armour]._o.mod_res;
	   //if(db.cur_offhand!=-1) 	res+= db.objects[db.cur_offhand]._o.mod_def; //shield
	   //if(db.cur_mainhand!=-1) 	res+= db.objects[db.cur_mainhand]._o.mod_def; //shield
	}
	if(target->_c.flags&ISVULN) res/=2;
	dmg=(int)ceil( damage* (MOD_MAGIC/(MOD_MAGIC+res)) );

	return dmg;
}

static int modify_elemental_damage(Entity *target, int damage, int type)
{
	int dmg;
	int flags=target->_c.res_flags;
	int res=target->_c.stat.res; //base resistance
	int immune=0;
	int resistant=0;
	int weak=0;
	switch(type)
	{
		case DMG_FIRE: immune=(flags&IMMUNE_FIRE); resistant=(flags&RESIST_FIRE); weak=(flags&WEAKTO_FIRE); break;
		case DMG_ICE: immune=(flags&IMMUNE_FROST); resistant=(flags&RESIST_FROST); weak=(flags&WEAKTO_FROST); break;
		case DMG_POISON: immune=(flags&IMMUNE_POISON); resistant=(flags&RESIST_POISON); weak=(flags&WEAKTO_POISON); break;
	}

	dmg=(int)ceil( damage* (MOD_MAGIC/(MOD_MAGIC+res)) );
	if(immune) dmg=0;
	else if(resistant) dmg/=2;
	else if(weak) dmg*=2;

	return dmg;
}
