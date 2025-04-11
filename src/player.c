#include "diabaig.h"

static int _show_inv(int type)
{
	int id;
	Entity *e;
	int x,y;
	int count=0;
	

	if(!type)
	{
		int types[]={WEAPON,ARMOUR,RING,TRINKET,FOOD,POTION,SCROLL,0};

		for(int i=0; types[i]!=0; i++)
		{
			_show_inv(types[i]);
		}
		return 0;
	}

	getyx(win,y,x);

	for(int i=0; i<26; i++)
	{
		if(db.inventory[i]>=0 && db.objects[db.inventory[i]]._o.type==type) count+=1;
	}
	if(!count) return 0;

	switch(type)
	{
		case WEAPON: 	wprintw(win," weapons:"); 	break;
		case ARMOUR: 	wprintw(win," armour:"); 	break;
		case RING: 		wprintw(win," rings:"); 		break;
		case TRINKET:	wprintw(win," trinkets:");	break;
		case FOOD: 		wprintw(win," food:"); 		break;
		case POTION: 	wprintw(win," potions:"); 	break;
		case SCROLL: 	wprintw(win," scrolls:"); 	break;
		default:		break;//wprintw(win,"other:\n"); 	break;
	}

	for(int i=0; i<26; i++)
	{
		if((id=db.inventory[i])!=-1)
		{
			e=&db.objects[id];

			if(!type || e->_o.type==type)
			{
				wmove(win,y,x+9);
				wprintw(win," %c) %s ", i+'a', getname(e));
				y++;
				//count++;
				//if(e->_o.enchant_level>0) wprintw(win,"+%d enchanted ",e->_o.enchant_level);
				//if(e->_o.enchant_level<0) wprintw(win,"%d disenchanted ",e->_o.enchant_level);
				if(e->_o.potion_effect[1]>0)
				{
					wprintw(win,"coated with a potion of %s ",potion_info[e->_o.potion_effect[0]].obj_name);
				}
				
				if(e->id==db.cur_mainhand) wprintw(win,"(mainhand) ");
				if(e->id==db.cur_offhand) wprintw(win,"(offhand) ");
				if(e->id==db.cur_armour) wprintw(win,"(being worn) ");
				if(e->id==db.cur_ringR) wprintw(win,"(right hand) ");
				if(e->id==db.cur_ringL) wprintw(win,"(left hand) ");
				//if(e->id==db.quiver) wprintw(win,"(inside quiver) ");
				//wprintw(win,"\n");
			}
		}
	}
	wprintw(win,"\n");
	wborder(win,0,0,0,0,0,0,0,0);
	return count;
}
void show_inventory()
{
	wclear(win);
	wrefresh(win);

	char tmp[128];
	int count=0;
	//for(int i=0; i<26; i++)
	//{
	//	if(db.inventory[i]!=-1) count++; 
	//}

	int types[]={WEAPON,ARMOUR,RING,TRINKET,FOOD,POTION,SCROLL,0};
	int id, *t=&types[0];
	wmove(win,1,0);
	while(*t)
	{
		for(int i=0;i<26;i++)
		{
			if((id=db.inventory[i])!=-1 && db.objects[id]._o.type==*t)
			{
				count+=_show_inv(*t);
				break;
			}
		}
		t++;
	}

	wmove(win,NROWS-2,1);
	wprintw(win,"Press any key to continue or use item (e:eat, t:throw, d:drink etc)");
	sprintf(tmp,"Inventory (%d/26)",count);
	display_frameheader(tmp);
	wrefresh(win);
	switch(getch())
	{
		case 'a': apply_potion(); break;
		case 'e': eat(); break;
		case 'd': drink(); break;
		case 'r': read_scroll(); break;
		case 'w': equip(); break;
		case 't': throw_item(); break;
		case 'D': drop(); break;

		default:
			wclear(win);
			wrefresh(win);
			break;
	}
}

Entity *menuselect(int type, const char *header)
{
	wclear(win);
	wrefresh(win);
	wmove(win,1,0);
	_show_inv(type);
	display_frameheader((char*)header);
	wrefresh(win);

	Entity *e=NULL;
	int sel=getch()-'a', id;
	bool valid=true;
	if(sel<0||sel>=26) valid=false;
	else
	{
		id=db.inventory[sel];
		if(id<0 || id>DBSIZE_OBJECTS) valid=false;
		else
		{
			e=&db.objects[id];
			if(!(e->flags & ISACTIVE)) valid=false;
		}
	}
	if(!valid)
	{
		e=NULL;
		msg("invalid selection");
	}
	wclear(win);
	wrefresh(win);

	return e;
}

void show_discovered()
{
	wclear(win);
	wrefresh(win);
	//wprintw(win,"discovered items:\n\n");
	obj_info *info, *start;

	wmove(win,2,0);
	wprintw(win,"  potions:\n");
	start=getinfo(POTION);
	for(info=start; info<&start[MAXPOTION]; info++)
	{
		if(info->known)
			wprintw(win,"  > %s potion -> potion of %s\n",info->guess, info->obj_name);
	}

	wprintw(win,"  scrolls:\n");
	start=getinfo(SCROLL);
	for(info=start; info<&start[MAXSCROLL]; info++)
	{
		if(info->known)
			wprintw(win,"  > scroll marked \"%s\" -> scroll marked \"%s\" \n",info->guess, info->obj_name);
	}

	wborder(win,0,0,0,0,0,0,0,0);
	wrefresh(win);
	//getch();
	//wclear(win);
	//wrefresh(win);

}

void _status_overlay()
{
	struct statusinfo{ char name[3]; int y,x; long long unsigned int c;};
	struct statusinfo arr[]={
		{"BRN", 2, 3,COLOR_PAIR(STATUS_BURN)},
		{"POI", 6, 3,COLOR_PAIR(STATUS_POISON)},
		{"RGN", 10, 3,COLOR_PAIR(STATUS_REGEN)},
		{"CON",11, 3,0},
		{"BND",12, 3,COLOR_PAIR(STATUS_BIND)},
		{"SPD",13, 3,A_ITALIC},
		{"SLO",14, 3,0},
		{"FRZ",15, 3,COLOR_PAIR(STATUS_FREEZE)},
		{"INK",21, 3,COLOR_PAIR(STATUS_INK)},
		{"FNT",26, 3,A_BLINK},
		{"ARC",29, 3,COLOR_PAIR(STATUS_ARC)},
		{"\0",0,0,0}
	};

	int i=0;
	while(arr[i].name[0])
	{
		wmove(win,arr[i].y,arr[i].x);
		wattron(win,arr[i].c);
		waddnstr(win,arr[i].name,3);

		wrefresh(win);
		wattroff(win,arr[i].c);
		i++;
	}
}

void show_help()
{

	int inloop=1;
	int input=0;

	display_dathead(res_help_txt, res_help_txt_len);
	display_frameheader("HELP MENU");

	while( inloop )
	{
		//inloop=0;
		wrefresh(win);
		input=getch();

		switch(input)
		{
			case '1':
				display_dathead(res_help_overview_txt,res_help_overview_txt_len);
				display_frameheader("HELP OVERVIEW");
				break;

			case '2':
				display_dathead(res_help_overview_txt,res_help_overview_txt_len);
				display_frameheader("HELP CONTROLS");
				break;

			case '3': 
				display_dathead(res_help_status_txt, res_help_status_txt_len);
				_status_overlay();
				display_frameheader("STATUS EFFECTS");
				break;

			case '4':
				display_dathead(res_help_tips_txt,res_help_tips_txt_len);
				display_frameheader("TIPS FOR SUCCESS");
				break;

			case '5':
				show_discovered();
				display_frameheader("DISCOVERED ITEMS");
			   	break;

			case '6':
				show_message_history();
				display_frameheader("ALERT HISTORY");
				break;

			case '7':
				identify_screen();
				break;

			case '8':
				conf_set();
				inloop=0;
				break;

			case '9':
				colour_check();
				break;

			default:
				inloop=0;
				//display_dathead(res_help_txt, res_help_txt_len);
				//display_frameheader("HELP OVERVIEW");
				break;
		}
	}
	wclear(win);
	wrefresh(win);
}


void show_message_history()
{
	wclear(win);
	wrefresh(win);
	//wprintw(win,"message history:\n");
	wmove(win,2,0);
	for(int i=0;i<26;i++)
	{
		wprintw(win,"  > %s\n",message_history[i]);
	}
	wprintw(win,"  Press any key to continue...");
	wborder(win,0,0,0,0,0,0,0,0);
	wrefresh(win);
	//getch();
	//wclear(win);
	//wrefresh(win);
}

void show_performance()
{
	//This is to maybe begin to help find that game crash bug
	wclear(win);
	wrefresh(win);
	wprintw(win,"Performance\n");
	wprintw(win,"DBSIZE:           %d\n",(int)sizeof(db));
	wprintw(win,"Floor:            %d/%d\n", db.cur_level+1, NLEVELS);
	wprintw(win,"Active Rooms:     %d/%d\n", db.nrooms, NROOMS);

	int ntil=0;
	level *l;

	for(int id=0; id<NLEVELS; id++)
	{
		l=&db.levels[id];
		for(int ti=0; ti<(XMAX*YMAX); ti++)
		{
			if(l->tile_flags[ti] & MS_EXPLORED) ntil++;
		}
	}
	wprintw(win,"Tiles explored:   %d/%d %d%%\n",ntil,(NLEVELS*XMAX*YMAX),(int)(100*((float)ntil/(NLEVELS*YMAX*XMAX))));

	Entity *e;
	int nobj=0, ncreat=0;
	for(int id=0; id<DBSIZE_OBJECTS; id++)
	{
		e=&db.objects[id];
		if(e && (e->flags&ISACTIVE)) nobj++;
	}
	for(int id=0; id<DBSIZE_CREATURES; id++)
	{
		e=&db.creatures[id];
		if(e && (e->flags&ISACTIVE)) ncreat++;
	}

	wprintw(win,"Active objects:   %d/%d\n", nobj, DBSIZE_OBJECTS);
	wprintw(win,"Active creatures: %d/%d\n", ncreat, DBSIZE_CREATURES);

	_daemon *d;
	int ndam=0;
	for(int id=0; id<NDAEMONS; id++)
	{
		d=&db.daemons[id];
		if(d && (d->time>0 && d->c_id!=-1))
			ndam++;
	}

	wprintw(win,"Active daemons:   %d/%d\n", ndam, NDAEMONS);

	wprintw(win,"Player level:     %d/%d\n", db.xplvl+1, MAX_XPLEVELS);
	int ninv=0;
	for(int id=0; id<26; id++)
	{
		if(db.inventory[id]!=-1)
			ninv++;
	}
	wprintw(win,"Inventory:        %d/%d\n",ninv,26);

	wprintw(win,"Press any key to continue\n");
	wrefresh(win);
	getch();
	wclear(win);
	wrefresh(win);

}


void identify_screen()
{
	int types[100];
	int ii=0;
	int seenbefore=0;
	Entity e;

	wclear(win);
	wrefresh(win);

	wmove(win,2,2);
	waddstr(win, "objects:\n");

	for(int i=0; i<DBSIZE_OBJECTS; i++)
	{
		e=db.objects[i];
		if(e.flags&ISACTIVE && e.pos.z==db.cur_level && db.tiles[ e.pos.y*XMAX+e.pos.x].flags&ML_VISIBLE)
		{
			seenbefore=0;
			for(int ti=0; ti<ii; ti++)
			{
				if(e._o.type==types[ti]) seenbefore=1;
			}
			if(!seenbefore)
			{
				types[ii++]=e._o.type;
				char name[128];
				switch(e._o.type)
				{
					case POTION: sprintf(name,"a potion, it may have positive or negative effects"); break;
					case SCROLL: sprintf(name,"a magic scroll, the words can only be read once"); break;
					case WEAPON: sprintf(name,"a weapon, this will make survival in Diabaig more likely"); 	break;
					case ARMOUR: sprintf(name,"some armour, this will provide you with much needed protection"); break;
					case RING:   sprintf(name,"a ring, it is sure to have some magical properties"); 	break;
					case GOLD: 	 sprintf(name,"some gold, a valuable shiny coin"); break;
					case TRINKET:sprintf(name,"a dragon tooth, it must have been knocked out in the battle"); break;
					case FOOD:   sprintf(name,"some food, to fuel your trip into the depths of Diabaig"); break;
					default: sprintf(name,"unkown"); break;
				}
				wprintw(win,"  %c : %s \n",e._o.type, name);//type_info[e._o.type].obj_name);
			}
		}
	}

	waddstr(win, "\n\n  creatures:\n");
	for(int i=0; i<DBSIZE_CREATURES; i++)
	{
		e=db.creatures[i];
		if(e.flags&ISACTIVE && e.pos.z==db.cur_level && db.tiles[ e.pos.y*XMAX+e.pos.x].flags&ML_VISIBLE)
		{
			seenbefore=0;
			for(int ti=0; ti<ii; ti++)
			{
				if(e._c.type==types[ti]) seenbefore=1;
			}
			if(!seenbefore)
			{
				types[ii++]=e._c.type;
				if(e._c.type=='@') wprintw(win,"  @ : you, the player \n");
				else wprintw(win,"  %c : %s, %s\n",e._c.type, monsters[e._c.type-'A'].monster_name, monsters[e._c.type-'A'].description);
			}
		}
	}
	wborder(win,0,0,0,0,0,0,0,0);
	display_frameheader("Everything you can see");
}



void colour_check()
{
	int cols[8]={C_BLACK,C_RED,C_YELLOW,C_GREEN,C_CYAN,C_BLUE,C_MAGENTA,C_WHITE};
	
	wclear(win);

	wmove(win,3,2);
	waddstr(win, "The following effects are all potential effects presented by the game.");
	wmove(win,4,2);
	waddstr(win, "If some do not work correctly, it may affect the visual quality but");
	wmove(win,5,2);
	waddstr(win, "should not affect the gameplay.");

	wmove(win,8,2);
	waddstr(win,"Colours:");
	for(int i=0; i<8; i++) 
	{
		wmove(win,8,11+i);
		wattron(win,COLOR_PAIR(cols[i]));
		waddch(win,'#');

		wmove(win,8,19+i);
		wattron(win,A_REVERSE);
		waddch(win,'#');

		wattroff(win,A_REVERSE|COLOR_PAIR(cols[i]));
	}

	wmove(win,10,2);
	waddstr(win,"Visual Effects: ");

	wattron(win,A_BOLD);
	waddstr(win,"BOLD");
	wattroff(win,A_BOLD);
	waddch(win,' ');

	wattron(win,A_DIM);
	waddstr(win,"DIM");
	wattroff(win,A_DIM);
	waddch(win,' ');

	wattron(win,A_ITALIC);
	waddstr(win,"ITALIC");
	wattroff(win,A_ITALIC);
	waddch(win,' ');

	wattron(win,A_BLINK);
	waddstr(win,"BLINK");
	wattroff(win,A_BLINK);
	waddch(win,' ');

	wattron(win,A_UNDERLINE);
	waddstr(win,"UNDERLINE");
	wattroff(win,A_UNDERLINE);
	waddch(win,' ');

	wattron(win,A_STANDOUT);
	waddstr(win,"STANDOUT");
	wattroff(win,A_STANDOUT);
	waddch(win,' ');

	wmove(win,12,2);
	waddstr(win,"Alt character set (box corners and block): ");
	wattron(win,A_ALTCHARSET);

	waddch(win,ACS_ULCORNER);
	waddch(win,ACS_HLINE);
	waddch(win,ACS_URCORNER);
	waddch(win,ACS_VLINE);
	waddch(win,ACS_LLCORNER);
	waddch(win,ACS_HLINE);
	waddch(win,ACS_LRCORNER);
	waddch(win,ACS_CKBOARD);
	//waddch(win,ACS_BLOCK);

	wattroff(win,A_ALTCHARSET);

	wborder(win,0,0,0,0,0,0,0,0);
	display_frameheader("Visual Check");
	wrefresh(win);


}
