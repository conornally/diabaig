#include "diabaig.h"

void init()
{
	if(verbose) fclose(fopen("/tmp/diabaig.log","w"));

	initscr();

	int winx=0,winy=0;
	getmaxyx(stdscr,winy,winx);

	while( (winx<=NCOLS) || (winy<=NROWS))
	{
		mvprintw(0,0,"Please resize terminal to at least: (%dx%d)",NCOLS+1,NROWS+1);
		mvprintw(1,0,"Current terminal size: (%dx%d)",winx,winy);
		refresh();
		getmaxyx(stdscr,winy,winx);

#ifdef WINDOWS
		resize_term(NROWS+1,NCOLS+1);
#endif
	}

	if(conf_load(".diabaigrc")) conf_diabaig=conf_default();

	win=newwin(NROWS,NCOLS,0,0);

	cbreak();
	keypad(stdscr,TRUE);
	keypad(win,TRUE);
	mousemask(BUTTON1_PRESSED|BUTTON1_CLICKED,NULL);
	mouseinterval(0);

	if(has_colors())
	{
		start_color();
		use_default_colors();
		init_pair(STATUS_BURN,	COLOR_BLACK, 	COLOR_YELLOW); 
		init_pair(STATUS_POISON,COLOR_MAGENTA, 	-1); 
		init_pair(STATUS_FREEZE,COLOR_BLACK, 	COLOR_CYAN); 
		init_pair(STATUS_REGEN, COLOR_GREEN, 	-1); 
		init_pair(STATUS_BIND, 	COLOR_BLACK, 	COLOR_GREEN); 
		init_pair(STATUS_INK,  	COLOR_YELLOW, 	-1); 
		init_pair(STATUS_ARC,  	COLOR_BLUE, 	-1); 

		init_pair(FLUID_COL_COMBUST, COLOR_RED,COLOR_YELLOW);
		init_pair(FLUID_COL_MIST,    COLOR_BLUE,COLOR_CYAN);
		init_pair(FLUID_COL_MIASMA,  COLOR_BLUE,COLOR_MAGENTA);
		init_pair(FLUID_COL_SMOKE,   COLOR_BLACK, COLOR_WHITE);

		init_pair(C_WHITE, 	COLOR_WHITE, 	-1); 
		init_pair(C_RED, 	COLOR_RED, 		-1); 
		init_pair(C_BLUE, 	COLOR_BLUE, 	-1); 
		init_pair(C_GREEN, 	COLOR_GREEN, 	-1); 
		init_pair(C_YELLOW,	COLOR_YELLOW, 	-1); 
		init_pair(C_CYAN,	COLOR_CYAN, 	-1); 
		init_pair(C_MAGENTA,COLOR_MAGENTA, 	-1); 
		init_pair(C_BLACK,	COLOR_BLACK, 	-1); 
	}
	noecho();
	curs_set(0);

	init_world();
}

void init_world()
{
	init_db();
	init_guesses();

	new_player();
	gen_dragon(); //Maybe this always is generated here?
	game_won=0;

	if(!rng(CATCHANCE)) monsters['c'-'A'].prob=1;

	construct_level(&db.levels[0]);
	spawnroom();
	init_pathfinding();
}

void reset_world()
{
	obj_info* info;
	for(int id=0; id<MAXPOTION; id++)
	{
		info=&potion_info[id];
		info->known=false;
	}
	for(int id=0; id<MAXSCROLL; id++)
	{
		info=&scroll_info[id];
		info->known=false;
	}
	for(int id=0; id<MAXRINGS; id++)
	{
		info=&ring_info[id];
		info->known=false;
	}

	//GRIM
	memset(message_history,'\0',sizeof(message_history));
	memset(message_queue,'\0',sizeof(message_queue));
	memset(message,'\0',sizeof(message));
	seed=time(NULL);
	wizardmode=0;
}

void revert()
{
	endwin();
}

void _intro()
{
	display_dathead(res_intro_txt, res_intro_txt_len);
	wmove(win, 28,34);
	wattron(win,A_BOLD);
	wprintw(win,"%s the %s.",dragonname, dragon_mod);
	wattroff(win,A_BOLD);
	wmove(win,YMAX+3,1);
	wprintw(win," seed: %d | version: diabaig-v%s | dev: conornally | redraw [r]",seed,VERSION);


}
int home()
{
	//wclrtoeol(win);
	wrefresh(win);
	_intro();

	char* menu[]=  {"          ",
					" New Game ",
					" Continue ",
					"High Score",
					" Settings ",
					" Credits  ",
					"   Quit   "};

	int select=0;
	int input=0;
	cbreak();
	keypad(win,TRUE);
	do
	{
		wmove(win,YMAX-15+select,XMAX/2-5);
		waddch(win,' ');
		wmove(win,YMAX-15+select,XMAX/2+6);
		waddch(win,' ');

		switch(input)
		{
			case KEY_UP:   case 'k': case KEY_LEFT:  select--; break;
			case KEY_DOWN: case 'j': case KEY_RIGHT: select++; break;
			case 'n': return MENU_NEWGAME;
			case 'c': return MENU_CONTINUE;
			case 'h': return MENU_HIGHSCORES;
			case 'd': return MENU_CREDITS;
			case 's': return MENU_SETTINGS;
			case 'q': return MENU_QUIT;
			case 'r': return 6;
			//case 's': return 5;
			//case 'w': return 6;
			//case 't': return 7;
		}
		select= (select+6)%6;

		wmove(win,YMAX-15+select,XMAX/2-5);
		waddch(win,'>');
		wmove(win,YMAX-15+select,XMAX/2+6);
		waddch(win,'<');

		for(int i=0; i<7; i++)
		{
			wmove(win,YMAX-16+i,XMAX/2-4);
			if(i==(select+1)) wattron(win,A_BOLD);
			waddstr(win,menu[i]);
			wattroff(win,A_BOLD);
		}

		wmove(win,0,0);
		wrefresh(win);
	}while((input=wgetch(win))!='\n');


	return select;
}

void mainloop()
{
	wclear(win);
	refresh();
	display();

	wrefresh(win);
	while(running)
	{
		update();
		display();
		if(player->_c.stat.hp<=0 || game_won) player_die(); 
	}
  	reset_world();
}
