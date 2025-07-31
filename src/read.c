#include "diabaig.h"

static void notimplemented(char *s)
{
	msg("%s not yet implemented",s);
}

static void _scroll_identification()
{
	Entity *to_id=menuselect(0,"select an item to identify");
	if(to_id)
	{
		getinfo(to_id->_o.type)[to_id->_o.which].known=true;
		msg("you identified %s",getname(to_id));
		to_id->_o.flags|=ISIDENTIF;
	}
}

static void _scroll_teleport()
{
	coord p;
	p=player->pos;
	moat(p.x,p.y)=NULL;
	do
	{
		p.x=rng(XMAX);
		p.y=rng(YMAX);
	}while(!islegal(p));
	moat(p.x,p.y)=player;
	player->pos=p;
	msg("aargh! suddenly you are ripped through space");
}


static void _scroll_cleanse()
{
	for(_daemon *d=&db.daemons[0]; d<&db.daemons[NDAEMONS]; d++)
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
				d->type==D_BLIND   ||
				d->type==D_LSD)
				stop_daemon(d);
		}
	}
	player->_c.flags &= ~(ISCONFUSED| ISPOISON|
							ISSLEEP| ISBURN
							|ISFREEZE | ISBOUND
							|ISSLOW | ISLSD
							|ISBLIND );
	db.hunger=MAXHUNGER;
	memset(message,'\0',sizeof(message));
	msg("a pale light bursts from the scroll and shimmers around your body");
}

static void _scroll_enchantarmour()
{
	Entity *item=menuselect(ARMOUR, "select armour to enchant");
	if(item && item->_o.type==ARMOUR)
	{
		msg("your %s shimmers in silver for a moment",getname(item));
		enchant(item);
	}
	else msg("you feel a sense of loss");
}

static void _scroll_enchantweapon()
{
	Entity *item=menuselect(WEAPON,"select weapon to enchant");
	if(item && item->_o.type==WEAPON)
	{
		msg("your %s shimmers in silver for a moment",getname(item));
		enchant(item);
	}
	else msg("you feel a sense of loss");
}

static void _scroll_revealmap()
{
	for(int id=0; id<XMAX*YMAX; id++)
	{
		db.levels[db.cur_level].tile_flags[id] |= MS_EXPLORED;
	}
	msg("the scroll glows and for a moment shows a map");
}

static void _scroll_sensecreatures()
{
	wclear(win);
	int ncreatures=0; //excluding player
	Entity *e;
	for(int x=0;x<XMAX;x++)
	for(int y=0;y<YMAX;y++)
	{
		if((e=moat(x,y)) && e!=player)
		{
			mvwaddch(win,y,x,e->_c.type);
			e->flags |= ISMARKED;
			ncreatures+=1;
		}
	}
	if(ncreatures) mvwaddstr(win,1,1,"you can sense all the creatures on this floor");
	else mvwaddstr(win,1,1,"there are no creatures on this floor");
	wborder(win,0,0,0,0,0,0,0,0);
	wrefresh(win);
	getch();
	player->_c.flags &= (~ISMARKED);
}

static void _scroll_senseitems()
{
	wclear(win);
	int nitems=0;
	Entity *e;
	for(int x=0;x<XMAX;x++)
	for(int y=0;y<YMAX;y++)
	{
		if((e=objat(x,y)))
		{
			mvwaddch(win,y,x,e->_o.type);
			e->flags |= ISMARKED;
			nitems++;
		}
	}
	if(nitems) mvwaddstr(win,1,1,"you can sense all the items on this floor");
	else mvwaddstr(win,1,1,"there are no items on this floor");
	wborder(win,0,0,0,0,0,0,0,0);
	wrefresh(win);
	getch();
}

static void _scroll_alertmonster()
{
	for(Entity *e=db.creatures; e<&db.creatures[DBSIZE_CREATURES]; e++)
	{
		if(e->flags&ISACTIVE && e->pos.z==db.cur_level)
		{
			if( (e->_c.flags & ISAGRO) && !(e->_c.flags & SEENPLAYER) )
			{
				e->_c.flags |= CANTRACK;
				if(rng(2)) break;
			}
		}
	}
	msg("the scroll emits a hitch pitched whistle");
}

static void _scroll_learnspell()
{
	int i=pick_spell();
	if(learn_spell(i)==RETURN_SUCCESS)
		msg("you learn the spell \"%s\"",spell_info[i].spell_name);
}

static void _scroll_nyctophobia()
{
	db.levels[db.cur_level].flags |= L_DARK;
	for(room *rm=&db.rooms[0]; rm<&db.rooms[db.nrooms]; rm++) rm->flags|=RM_ISDARK;

	msg("you hear a whispering wind, all the lights go out!");
}

static void _scroll_lore()
{
	char *lst[]={
		//..............................................this is the max length --->|
		"alchemists are skilled brewers of status inflicting potions",    //alchemist
		"druids can draw natural energy from around the dungeon to heal themselves", //druid
		"members of the druid guild excel in communing with natural energy sources", //druid
		"no body knows who introduced emus into diabaig", //emu
		"faes are peaceful in nature but are very protective of their toadstools", //fae
		"goblins like to hoard gold", //goblin
		"under close inspection, creatures may turn out to be illusions", //illusion
		"there are a race of lizardfolk that guard the dungeon above the dragons", //lizardfolk
		"be careful of mimics! they can disguise themselves as objects", //mimics
		"necromancers are physically weak, they use strong reanimations to fight", //necro
		"beware of phantoms, they disappear and attack when you least expect it", //phantoms
		"creatures are much tougher and more resistant after they are reanimated", //reanims
		"vampires can permanently reduce your health", //vampires
		"you may be frozen solid if you get close to a yeti", //yeti
		"zombies might move slowly but if they hit you, it will hurt", //zombies
																		  //
		"the acidic blobs will corrode your gear if you get too close", //Acidic blob
		"the presence of the dragon keeps the dungeon ecologically stable",//Dragon
		"fell beasts are twisted reptiles that ooze filth, don't get too close", //fell beast
		"gate keepers are remnants from a former time, great walking statues", //gatekeepers
		"destroy all the heads of a hydra and it will fall", //hydra
		"rumour has it, the mother of dragons dwells in diabaig somewhere", //motherofdragons
		"raggle the haggler is a cunning character who may sell you powerful items",//raggle
		"sidhe are vile faes that stop at nothing to pursue you", //sidhe

	  	//spells
		//..............................................this is the max length --->|
		"many spells become stronger as you use them, some can even be \"mastered\"",
		"elemental spells are powerful but are dangerous to use",
		"don't let your food get too close to clouds of miasma",
		"the art of the arcane is to absorb physical damage with magic",
		"hypnosis can be utilised in busy rooms for an effective escape",
		"control large numbers of creatures with the slice attack",

	  	//potions
		"you can throw potions at creatures and also coat your weapons with them",
		"be careful drinking unknown potions",
		"fire beats poison, poison beats ice, ice beats fire, nothing beats smoke",
		"smoke bombs offer a great opportunity to escape or ambush creatures",
		"marking creatures with ink will allow you to see them even when invisible",
		"potions may shatter if they get frozen",

		//food
		"beware the effects of mushrooms, they may satiate you at a price",
		"slime mould is safe to eat, but doesn't contain much nutrient",

		//scrolls
		"beware of reading scrolls you don't understand",
		"scrolls are highly flammable",
		//weapons
		//..............................................this is the max length --->|
		"disenchanted weapons are not as strong as regular ones",
		"enchanted weapons are much stronger than regular ones",
		"two handed weapons can hit hard but are less accurate",
		"attacking with a dagger is super effective when the target is asleep",
		"attacking with a dagger is super effective when you are invisible",
		//"one handed weapons are weaker but hit more reliably than two handed weapons",
		//armour
		"disenchanted weapons are not as strong as regular ones",
		"resistance (res) is the measure of defense against magic attacks",
		"a previous adventurer dropped some mithril armour in the dungeon",
		"the ring of earendil lights your way in this dark dungeon",
		"double up on elemental resistance for immunity",
		"armour made of cloth or leather will not be corroded by acidic blobs",

		//..............................................this is the max length --->|
		// mechanics
		"break line of sight and creatures might struggle to follow after a while",
		// lore
		"nobody knows who constructed Diabaig, maybe it was always here",
		"dragon scales are extremely valuable",

		0
	};
	msg( "you open the scroll, the words are only visible for a moment, they read:");
	msg( lst[ rng(clen(lst))]);
}

static void _scroll_amnesia()
{
	for(int i=0; i<XMAX*YMAX; i++) 
	{
		db.levels[db.cur_level].tile_flags[i] &= ~MS_EXPLORED;
	}
	msg("the words disorientate you, you forget where you have just been!");
}

static void _scroll_dexterity()
{
	player->_c.stat.dex+=2;
	char *options[2]={"dodging speed","melee accuracy"};
	msg("copying the martial forms on the scroll, you improve your %s",options[rng(2)]);
}

int read_scroll()
{
	int status=RETURN_UNDEF;
	Entity *e=menuselect(SCROLL,"select scroll to read");
	if(e && e->_o.type==SCROLL)
	{
		status=_doread(player,e);
	}
	else
	{
		msg("you can't read that");
		status=RETURN_FAIL;
	}

	return status;
}

int _doread(Entity *target, Entity *item)
{
	int status=RETURN_SUCCESS;
	if(!target || ! item) return RETURN_FAIL;
	switch( item->_o.which)
	{
		case S_ID: _scroll_identification(); break;
		case S_ENCHANTWEAPON: _scroll_enchantweapon(); break;
		case S_ENCHANTARMOUR: _scroll_enchantarmour(); break;
		case S_TELEPORT: _scroll_teleport(); break;
		case S_CLEANSE: _scroll_cleanse(); break;
		case S_MAP: _scroll_revealmap(); break;
		case S_SENSECREATURES: _scroll_sensecreatures(); break;
		case S_SENSEITEMS: _scroll_senseitems(); break;
		case S_ALERTMONSTER: _scroll_alertmonster(); break;
		case S_LEARNSPELL: _scroll_learnspell(); break;
		case S_NYCTOPHOBIA: _scroll_nyctophobia(); break;
		case S_LORE: _scroll_lore(); break;
		case S_AMNESIA: _scroll_amnesia(); break;
		case S_DEXTERITY: _scroll_dexterity(); break;
		default :
		{
			obj_info info=scroll_info[item->_o.which];
			notimplemented( info.known ? info.obj_name:info.guess);
			status=RETURN_FAIL;
			break;
		}
	}
	use(item);
	return status;
}

