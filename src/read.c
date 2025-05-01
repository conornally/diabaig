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
	//i should end all timers too
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
	if(db.cur_armour!=-1)
	{
		/*
		db.objects[db.cur_armour]._o.mod_def++;
		db.objects[db.cur_armour]._o.mod_res++;
		db.objects[db.cur_armour]._o.enchant_level++;
		*/
		msg("your %s shimmers in silver for a moment",getname(&db.objects[db.cur_armour]));
		enchant(&db.objects[db.cur_armour]);
	}
	else msg("you feel a sense of loss");
}

static void _scroll_enchantweapon()
{
	int id;
	Entity *item;
	if(db.cur_mainhand!=-1 && db.cur_offhand==-1) id=db.cur_mainhand;
	else if(db.cur_mainhand==-1 && db.cur_offhand!=-1) id=db.cur_offhand;
	else if(db.cur_mainhand==db.cur_offhand) id=db.cur_mainhand; //two handed weapon //or none
	else
	{
		char *_mainhand=strdup(getname(&db.objects[db.cur_mainhand]));
		wprintw(win,"\nenchant %s (mainhand) or %s (offhand) (m/o):",_mainhand, getname(&db.objects[db.cur_offhand]));
		wrefresh(win);
		free(_mainhand);
		switch(getch())
		{
			case 'm':case'M':
				id=db.cur_mainhand;
				break;
			case 'o':case'O':
				id=db.cur_offhand;
				break;
			default: id=-1; break;
		}
	}
	if(id!=-1)
	{
		item=&db.objects[id];
		msg("your %s shimmers in silver for a moment",getname(&db.objects[id]));
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
	int ncreatures=-1; //excluding player
	Entity *e;
	for(int x=0;x<XMAX;x++)
	for(int y=0;y<YMAX;y++)
	{
		if((e=moat(x,y)))
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
				break;
			}
		}
	}
	msg("the scoll emits a hitch pitched whistle");
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
		"vampires can permenantly reduce your health", //vampires
		"you may be frozen solid if you get close to a yeti", //yeti
		"zombies might move slowly but if they hit you, it will hurt", //zombies
																		  //
																		  
	  	//spells
		//..............................................this is the max length --->|
		"many spells become stronger as you use them, some can even be \"mastered\"",
	  	//potions
		"you can throw potions at creatures and also coat your weapons with them",
		"be careful drinking unknown potions",
		//scrolls
		"beware of reading scrolls you don't understand",
		//weapons
		//..............................................this is the max length --->|
		"disenchanted weapons are not as strong as regular ones",
		"enchanted weapons are much stronger than regular ones",
		"two handed weapons can hit hard but are less accurate",
		//"one handed weapons are weaker but hit more reliably than two handed weapons",
		//armour
		"disenchanted weapons are not as strong as regular ones",
		"resistance (res) is the measure of defense against magic attacks",
		"a previous adventurer dropped some mithril armour in the dungeon",
		"the ring of earendil lights your way in this dark dungeon",
		//..............................................this is the max length --->|
		// mechanics
		// lore
		"it is believed that dwarves dug the great halls of diabaig",
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

