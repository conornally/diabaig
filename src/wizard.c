#include "diabaig.h"
static int _name(int argc, char *argv[]);
static int _give(int argc, char *argv[]);
static int _goto(int argc, char *argv[]);
static int _levelup(int argc, char *argv[]);
static int _learn(int argc, char *argv[]);
static int _reveal(int argc, char *argv[]);
static int _identify(int argc, char *argv[]);
static int _help(int argc, char *argv[]);
static int _spawn(int argc, char *argv[]);
static int _profile(int argc, char *argv[]);

static int _stresstest1(int argc, char *argv[]);
static int _show(int argc, char *argv[]);

struct _func
{
	char *name; 
	int(*func)();
} funcs[]={
	{"name",_name},
	{"give",_give},
	{"goto",_goto},
	{"levelup",_levelup},
	{"learn",_learn},
	{"reveal",_reveal},
	{"identify",_identify},
	{"help",_help},
	{"spawn",_spawn},
	{"profile",_profile},
	{"stress", _stresstest1},
	{"show",_show},
	{NULL, NULL}
};

int _do_wizard(char *buf)
{
	char *bufcpy=malloc(64);
	int argc=0;
	char *argv[8];
	int status=0;

	if(strlen(buf))
	{
		strcpy(bufcpy,buf);

		//while(argc<8 && (argv[argc]=strsep(&bufcpy," "))) argc++;
		argv[argc++]=strtok(bufcpy," ");
		while(argc<8 && (argv[argc]=strtok(NULL," "))) argc++;
		

		if(argc>0)
		{
			struct _func *f=funcs;
			while(f->name)
			{
				if(!strcmp(argv[0],f->name))
				{
					status=f->func((int)argc,(char**)argv);
					break;
				}
				f++;
			}
		}
	}
	free(bufcpy);
	return status;
}

int wizard_console()
{
	if(!wizardmode)	
	{
		return 0;
	}

	char *buf=malloc(64);
	int status=0;

  	wmove(win,MSGY+2,MSGX);
	wclrtoeol(win);
	wborder(win,0,0,0,0,0,0,0,0);

  	waddstr(win,"$~");
	echo();
	wgetnstr(win,buf,64);
	_do_wizard(buf);
	noecho();
	free(buf);

	return status;
}


static int _name(int argc, char *argv[])
{
	if(argc==1 && argv) msg("player name: \"%s\"",getname(player));
	return 0;
}

static int _give(int argc, char *argv[])
{
	int which=0, status=1, quantity=1;
	Entity *item=NULL;

//	if(argc==2)
//	{
//		int types[]={SCROLL,POTION,WEAPON,ARMOUR,RING,FOOD,TRINKET,GOLD};
//		obj_info *info=NULL;
//		for(int i=0; i<8; i++)
//		{
//			info=getinfo(types[i]);
//			for(int j=0; j<getmax(types[i]); j++)
//			{
//				msg("%s %s %d",info[j].obj_name, argv[1],strcmp(argv[1],info[j].obj_name));
//				if(!strcmp(argv[1],info[j].obj_name))
//				{
//					item=_new_obj(types[i]);
//					set_item(item,j);
//				}
//
//			}
//		}
//
//		if(item)
//		{
//			item->_o.quantity=quantity;
//			objat(player->pos.x,player->pos.y)=item;
//			pickup();
//			status=0;
//		}
//	}
	if(argc>=2)
	{
		if(argc>2) which=atoi(argv[2]);

		if(!strcmp(argv[1],"scroll") || !strcmp(argv[1],"?"))
		{
			if(which>=0 && which<MAXSCROLL)
			{
				item=_new_obj(SCROLL);
				_set_scroll(item,which);
			}
		}
		else if(!strcmp(argv[1],"potion") || !strcmp(argv[1],"!"))
		{
			if(which>=0 && which<MAXPOTION)
			{
				item=_new_obj(POTION);
				_set_potion(item,which);
			}
		}
		else if(!strcmp(argv[1],"weapon") || !strcmp(argv[1],"/"))
		{
			if(which>=0 && which<MAXWEAPON)
			{
				item=_new_obj(WEAPON);
				_set_weapon(item,which);
			}
		}
		else if(!strcmp(argv[1],"armour") || !strcmp(argv[1],"]"))
		{
			if(which>=0 && which<MAXARMOUR)
			{
				item=_new_obj(ARMOUR);
				_set_armour(item,which);
			}
		}
		else if(!strcmp(argv[1],"ring") || !strcmp(argv[1],"="))
		{
			if(which>=0 && which<MAXRINGS)
			{
				item=_new_obj(RING);
				_set_ring(item,which);
			}
		}
		else if(!strcmp(argv[1],"food") || !strcmp(argv[1],":"))
		{
			if(which>=0 && which<MAXFOOD)
			{
				item=_new_obj(FOOD);
				item->_o.which=which;
				//_set_food(item,which);
			}
		}
		else if(!strcmp(argv[1],"trinket") || !strcmp(argv[1],"^"))
		{
			if(which>=0 && which<MAXTRINKET)
			{
				item=_new_obj(TRINKET);
				_set_trinket(item,which);
			}
		}
		else
		{
			int types[]={SCROLL,POTION,WEAPON,ARMOUR,RING,FOOD,TRINKET,GOLD};
			obj_info *info=NULL;
			for(int i=0; i<8; i++)
			{
				info=getinfo(types[i]);
				for(int j=0; j<getmax(types[i]); j++)
				{
					if(!strcmp(argv[1],info[j].obj_name))
					{
						item=_new_obj(types[i]);
						set_item(item,j);
					}

				}

			if(argc==3) quantity=atoi(argv[2]);
			}
		}
		if(argc==4)
		{
			quantity=atoi(argv[3]);
		}

		//else status=1;
		if(item)
		{
			item->_o.quantity=quantity;
			objat(player->pos.x,player->pos.y)=item;
			pickup();
			status=0;
		}
	}
	return status;
}

static int _goto(int argc, char *argv[])
{
	int floor=-1,status=1;
	if(argc==2)
	{
		floor=atoi(argv[1]);
		if(floor>=0 && floor<(NLEVELS-1))
		{
			db.cur_level=floor;
			construct_level(&db.levels[floor]);
			moat(player->pos.x,player->pos.y)=NULL;
			if(db.levels[db.cur_level].upstair)
			{
				int ti= db.levels[db.cur_level].upstair ;
				db.tiles[ti].creature=player;

				player->pos.x=ti%XMAX;
				player->pos.y=ti/XMAX;
				player->pos.z=floor;
			}
			else placeinroom(&db.rooms[0],player);
			status=0;
		}
	}
	return status;
}

static int _levelup(int argc, char *argv[])
{
	int status=1;
	if(argc==1 && argv)
	{
		if(db.xplvl<(MAX_XPLEVELS-1))
		{
			db.xp=level_values[db.xplvl];
			levelup();
			status=0;
		}
	}
	return status;
}

static int _learn(int argc, char *argv[])
{
	int status=1, which=-1;
	if(argc==2)
	{
		which=atoi(argv[1]);
		if(which>=0 && which<MAXSPELL)
		{
			learn_spell(which);
			status=0;
		}
	}
	return status;
}

static int _reveal(int argc, char *argv[])
{
	if(argc==1 && argv)
	{
		for(int i=0;i<XMAX*YMAX;i++)db.levels[db.cur_level].tile_flags[i] |= MS_EXPLORED;
	}
	return 0;
}

static int _identify(int argc, char *argv[])
{
	if(argc==1 && argv)
	{
		for(int i=0;i<MAXPOTION;i++)potion_info[i].known=true;
		for(int i=0;i<MAXSCROLL;i++)scroll_info[i].known=true;
		for(int i=0;i<MAXRINGS;i++)ring_info[i].known=true;
	}
	return 0;
}

static int _help(int argc, char *argv[])
{
	if(argc==1 && argv)
	{
		wclear(win);
		wprintw(win,"WIZARDMODE commands:\n");
		wprintw(win,"$~ name                  : print playername\n");
		wprintw(win,"$~ give type which [num] : give player item 'type' 'which' and optional 'num'\n");
		wprintw(win,"$~ goto level            : goto dungeon floor 'level'\n");
		wprintw(win,"$~ levelup               : level player up one level\n");
		wprintw(win,"$~ learn which           : learn spell 'which'\n");
		wprintw(win,"$~ reveal                : reveal map\n");
		wprintw(win,"$~ identify              : identify all items\n");
		wprintw(win,"$~ help                  : show help screen\n");
		wprintw(win,"$~ spawn type            : spawn creature of 'type'\n");
		wprintw(win,"$~ profile profilename   : load profile\n");
		wprintw(win,"$~ stress                : stress test\n");
		wprintw(win,"$~ show type             : show object 'type' information\n");
		wrefresh(win);
		wgetch(win);
	}
	return 0;
}

static int _spawn(int argc, char *argv[])
{
	int which;
	Entity* e;
	room* r=player->_c._inroom;
	if(argc==2)
	{
		which=argv[1][0];//atoi(argv[1]);
		if(which>='A' && which<='z')
		{
			if(r)
			{
				e=_new_monster(which);
				placeinroom(r,e);
			}
		}
		else
		{
			msg("bad char '%c'",(char)which);
		}
	}
	return 0;
}


////////////////// profiles /////////////////

void _wizard_profiles(struct _profiler *p)
{
	_log("loading profile: %s",p->name);
	int i=0;
	while(p->cmds[i])
	{
		_do_wizard(p->cmds[i]);
		i++;
	}
	wizardmode=1;
	msg("loaded profile: %s",p->name);
}






static int _profile(int argc, char *argv[])
{
	struct _profiler* p;
	if(argc==2)
	{
		for(p=&_profiles[0]; p->name; p++)
		{
			if(!strcmp(p->name,argv[1])) 
			{
				_wizard_profiles(p);
			}
		}
	}
	return 0;
}


static int _stresstest1(int argc, char *argv[])
{
	//int max=db.cur_level+1;
	for(int floor=0; floor<NLEVELS; floor++)
	{
			db.cur_level=floor;
			player->pos.z=floor;
			construct_level(&db.levels[floor]);
			moat(player->pos.x,player->pos.y)=NULL;
			placeinroom(&db.rooms[0],player);

			for(int i=0; i<=NROOMS; i++)
			{
				room* r=&db.rooms[i];
				if(i<db.nrooms)
				{
					player->pos.x=r->x+1;
					player->pos.y=r->y+1;
					inroom(player);
					light_local_area();
				}

				light_room(r);
				unlight_room(r);
				light_room(r);
				light_local_area();

			}

			display();
			_reveal(argc,argv);
			update();//getch();
					
	}
	return 0;
}

static int _show(int argc, char *argv[])
{
	int which,n;
	obj_info *info=NULL,*_inf;
	if(argc==2)
	{
		which=argv[1][0];
		info=getinfo(which);
		if(info)
		{
			n=getmax(which);
			_inf=&info[0];
			wclear(win);
			wprintw(win,"Info for %c\n",which);
			int i=0;
			while(i<n)
			{
				wprintw(win,"%2d %-24s  known:%d prob:%-3d guess: '%s'\n", i, _inf->obj_name, _inf->known,
						_inf->prob, _inf->guess);
				_inf++;
				i++;
			}
			wrefresh(win);
			wgetch(win);
		}
		else
		{
			msg("BAD %c",which);
		}
	}
	return 0;
}
