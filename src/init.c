#include "diabaig.h"

void init_guesses()
{
	obj_info *end, *info;
	int id;

	for(id=0;id<NCOLOURS;id++) potion_colours[id].used=false;
	for(id=0;id<NMETALS;id++) metal_types[id].used=false;
	for(id=0;id<MAXSPELL;id++) spell_info[id].nuses=0;


	//POTIONS
	assert(MAXPOTION<=NCOLOURS);
	info=potion_info;
	for(end=&potion_info[MAXPOTION]; info<end; info++)
	{
		do
		{
			id=rng(NCOLOURS);
		}while(potion_colours[id].used);

		//info->guess=potion_colours[id].guess;
		strncpy(info->guess,potion_colours[id].guess,OBJNAMESIZE);
		potion_colours[id].used=true;
	}

	//SCROLLS initial
	info=scroll_info;
	for(id=0, end=&scroll_info[MAXSCROLL]; info<end; id++,info++)
	{
		for(int i=0; i<15; i++)
		{
			switch(rng(6))
			{
				case 0: case 1:
					scroll_names[id][i]=rng(26)+'A';
					break;
				case 2: case 3: case 4:
					scroll_names[id][i]=rng(26)+'a';
					break;
				case 5:
					scroll_names[id][i]=' ';
					break;
			}
		}
		scroll_names[id][15]='\0';
		//info->guess=scroll_names[id];
		strncpy(info->guess,scroll_names[id],OBJNAMESIZE);
	}

	//RINGS
	assert(MAXRINGS<=NMETALS);
	info=ring_info;
	for(end=&ring_info[MAXRINGS]; info<end; info++)
	{
		do
		{
			id=rng(NMETALS);
		}while(metal_types[id].used);

		//info->guess=metal_types[id].guess;
		strncpy(info->guess, metal_types[id].guess, OBJNAMESIZE);
		metal_types[id].used=true;
	}
	_log("initialised guesses");
}

void init_playername()
{
	//display_dathead(docs_intro_txt, docs_intro_txt_len);

	char *env=NULL, tmp[12];
	echo();
	curs_set(1);

	char *names[]={"delver","explorer","misguided","lost",
		"sacrifice", "victim","misplaced","naive","brave",
		"lucky","unlucky","hapless", "hopeful", "helpless",
		0};


	env=getenv("USER");
	if(env) strncpy(playername,env,PLAYERNAMESZ);
	else strcpy(playername,names[rng(clen(names))]);

	wmove(win,6,4);
	wprintw(win,"Choose your name: ");
	wgetnstr(win,tmp,PLAYERNAMESZ);
	if(strlen(tmp)>0) strncpy(playername,tmp,PLAYERNAMESZ);
	curs_set(0);


	// THIS DOESNT NEED TO BE HERE?
	for(struct _profiler *p=_profiles; p->name; p++)
	{
		if(!strcmp(playername,p->name)) _wizard_profiles(p);
	}

	noecho(); //just final input initialisation
}

static char *class_info[CLASSMAX]={
"+--------------------------------------------------------------------------+\
  |  Rogue (classic)                                                         |\
  |                                                                          |\
  |  The original mode. You are a rounded character with the tools for       |\
  |  close and ranged combat, you also start with the ability to heal        |\
  |  yourself from minor damage taken in the dungeon.                        |\
  |                                                                          |\
  |                                                                          |\
  +--------------------------------------------------------------------------+",
"+--------------------------------------------------------------------------+\
  |  Warrior                                                                 |\
  |                                                                          |\
  |  You start with a stronger weapon, the ability to repel creatures and    |\
  |  have a higher base strength. However your strength comes at the cost    |\
  |  of your dexterity, you might get hit more and miss more often.          |\
  |                                                                          |\
  |                                                                          |\
  +--------------------------------------------------------------------------+",
"+--------------------------------------------------------------------------+\
  |  Monk                                                                    |\
  |                                                                          |\
  |  An acolyte of your diety, you are somewhat blessed. You can heal        |\
  |  yourself effctively from damage taken and begin with more resistance    |\
  |  to magic and elemental effects. You are however, a little bit flimsy.   |\
  |                                                                          |\
  |                                                                          |\
  +--------------------------------------------------------------------------+",
"+--------------------------------------------------------------------------+\
  |  Stark Raving Mad                                                        |\
  |                                                                          |\
  |  You've gone stark raving mad! You enter the dungeon with nothing but    |\
  |  what you find inside to protect you.                                    |\
  |                                                                          |\
  |                                                                          |\
  |                                                                          |\
  +--------------------------------------------------------------------------+",
"+--------------------------------------------------------------------------+\
  |  Wizard                                                                  |\
  |                                                                          |\
  |  Wizard(mode) allows you test all the items and experience all the       |\
  |  creature AI and try all the spells. It is really designed for           |\
  |  development and testing, you will not get entered into the highscores   |\
  |  with this run.          Try typing ':give potion 1'                     |\
  |                                                                          |\
  +--------------------------------------------------------------------------+"
};







int init_player()
{
	int select=0;
	int input=0;
	int status=0;
	int XX=7,YY=10;

	display_dathead(res_player_txt, res_player_txt_len);
	char *menu[]={  "Rogue (classic)",
					"Warrior",
					"Monk",
					"Stark Raving Mad",
   					"Wizard"	};

	init_playername();

	do
	{
		display_dathead(res_player_txt, res_player_txt_len);
		wmove(win,6,22);
		waddstr(win,playername);

		wmove(win,YY+select,XX-2);
		waddch(win,' ');
		switch(input)
		{
			case KEY_UP: case KEY_LEFT: case 'k': 	select--; break;
			case KEY_DOWN: case KEY_RIGHT: case 'j': select++; break;
			//case 'w': wizardmode=!wizardmode; break;
			//case 't': testarena=!testarena; break;
			case 'q': 
			case KEY_BACKSPACE: 
			case 127:
			case '\b':
				return 1; 
				break;
										  /*
			case 'r': 
				seed=time(NULL);
				srand(seed);
				init_world();
				break;
				*/
		}
		select=(select+CLASSMAX)%CLASSMAX;
		wmove(win,YY+select,XX-2);
		waddch(win,'>');

		for(int i=0; i<CLASSMAX; i++)
		{
			wmove(win,YY+i,XX);
			if(i==(select)) wattron(win,A_BOLD);
			waddstr(win,menu[i]);
			wattroff(win,A_BOLD);
		}

		wmove(win,22,1);
		waddstr(win,class_info[select]);

		int armx=46,army=11;
		switch(select)
		{
			case ROGUE: 
				//wmove(win,army-1,armx-0);waddstr(win,"\\"); 
				wmove(win,army+0,armx+0);waddstr(win,"\\"); 
				wmove(win,army+1,armx+0);waddstr(win,"/"); 
				break;
			case WARRIOR: 
				wmove(win,army-1,armx-0);waddstr(win,"|"); 
				wmove(win,army+0,armx+0);waddstr(win,"-"); 
				wmove(win,army+1,armx+0);waddstr(win,"'"); 
				wmove(win,army+1,armx-4);waddstr(win,"U"); 
				break;
			case MONK: 
				//wmove(win,army-1,armx-0);waddstr(win,"\\"); 
				wmove(win,army+0,armx+0);waddstr(win,"!"); 
				//wmove(win,army+1,armx+0);waddstr(win,"/"); 
				break;
			case STK_RAV_MAD: 
				//wmove(win,army-1,armx-0);waddstr(win,"\\"); 
				wmove(win,army-1,armx+11);waddstr(win,"-"); 
				wmove(win,army+2,armx-3);waddstr(win,"'"); 
				break;
			case WIZARD: 
				wmove(win,army-1,armx+0);waddstr(win,","); 
				wmove(win,army+0,armx+0);waddstr(win,"|"); 
				wmove(win,army+1,armx+0);waddstr(win,"|"); 
				wmove(win,army+2,armx+0);waddstr(win,"|"); 
				break;
		}

		wmove(win,YMAX+3,2);
		//wprintw(win,"Wizardmode: %c | ", wizardmode ? 'Y':'N');
		//wprintw(win,"Testarena: %c | ", testarena ? 'Y':'N');
		wprintw(win,"Seed: %d | ", seed);
		wprintw(win,"%s the %s | ",dragonname, dragon_mod);
		//waddstr(win,"(r) regenerate ");

		wborder(win,0,0,0,0,0,0,0,0);
		wrefresh(win);
		input=wgetch(win);

	}while(input!='\n');

	set_class( select );

	return status;
}

void set_class(classes cls)
{
	Entity *e=getdragon();
	int wmain=-1, woff=-1, tmain=-1,arrows=0;
	int armour=-1, ring=-1;
	int food=1;
	int spell=-1;
	int potion=-1;

	int dhp=0, dstr1=0, dstr2=0, ddef=0, ddex=0, dres=0;

	switch(cls)
	{
		case ROGUE:
			wmain=SPEAR;
			tmain=SHORTBOW;
			arrows=10;
			spell=SP_BANDAGEWOUNDS;
			break;

		case WARRIOR:
			wmain=SWORD;
			woff=SHIELD; //?
			spell=SP_REPEL;
			dstr1=1;
			ddex=-2;
			break;
		
		case MONK:
			wmain=DAGGER;
			armour=CLOTH;
			spell=SP_HEAL;
			if(e)
			{
				switch(e->_c.form)
				{
					case DRG_FIRE: ring=R_FIRERESIST; break;
					case DRG_ICE:  ring=R_FROSTRESIST; break;
					case DRG_POISON:ring=R_POISONRESIST; break;
					default: ring=R_ELENDIL; break;
				}
			}

			dres=5;
			dhp=-5;
			break;
		
		case STK_RAV_MAD:
			food=0;
			break;

		//case ASSASSIN:
		//	wmain=DAGGER;
		//	ring=R_CRITICALEYE;
		//	potion=P_SMOKEBOMB;
		//	ddex=2;
		//	break;

		case WIZARD:
			wizardmode=1;
			food=0;
			break;
		default: break;
	}

	int inv=0;
	Entity *wm=NULL,*wo=NULL,*tm=NULL,*aro=NULL;
	Entity *arm=NULL,*rn=NULL;
	if(wmain>=0)  //SET MAINHAND 
	{
		wm=_new_obj(WEAPON);	
		_set_weapon(wm, wmain);
		wm->_o.enchant_level=0;
		wm->_o.quantity=1;
		db.inventory[inv++]=wm->id;
		db.cur_mainhand=wm->id;

		if(wm->_o.flags&W_TWOHAND) db.cur_offhand=wm->id;
	}
	if(woff>=0)  //SET OFFHAND 
	{
		wo=_new_obj(WEAPON);	
		_set_weapon(wo, woff);
		wo->_o.enchant_level=0;
		wo->_o.quantity=1;
		db.inventory[inv++]=wo->id;
		db.cur_offhand=wo->id;
	}
	if(tmain>=0) //SET TOGGLE OFF
	{
		tm=_new_obj(WEAPON);	
		_set_weapon(tm, tmain);
		tm->_o.enchant_level=0;
		tm->_o.quantity=1;
		db.inventory[inv++]=tm->id;
		db.toggle_mainhand=tm->id;
		if(tm->_o.flags&W_TWOHAND) db.toggle_offhand=tm->id;
	}
	if(arrows>0) //HAVE ARROWS?
	{
		aro=_new_obj(WEAPON);	
		_set_weapon(aro, ARROW);
		aro->_o.enchant_level=0;
		aro->_o.quantity=arrows;
		db.inventory[inv++]=aro->id;
		db.quiver=aro->id;
	}
	if(armour>=0) 
	{
		arm=_new_obj(WEAPON);	
		_set_armour(arm, armour);
		arm->_o.quantity=1;
		arm->_o.enchant_level=0;
		db.cur_armour=arm->id;
		db.inventory[inv++]=arm->id;
	}
	if(ring>=0)
	{
		rn=_new_obj(RING);
		_set_ring(rn,ring);
		rn->_o.quantity=1;
		db.cur_ringR=rn->id;
		ring_info[ring].known=1;
		db.inventory[inv++]=rn->id;

	}
	if(food>0)
	{
		Entity *f=_new_obj(FOOD);
		f->_o.which=F_RATION;
		f->_o.quantity=1;
		db.inventory[inv++]=f->id;
	}
	if(potion>0)
	{
		Entity *p=_new_obj(POTION);
		_set_potion(p, potion);
		p->_o.quantity=1;
		db.inventory[inv++]=p->id;
		potion_info[potion].known=true;
	}

	if(spell>0) learn_spell(spell);

	if(dhp)
	{
		player->_c.stat.hp+=dhp;
		player->_c.stat.maxhp+=dhp;
	}

	if(dstr1) player->_c.stat.str[0]+=dstr1;
	if(dstr2) player->_c.stat.str[1]+=dstr2;
	if(ddef) player->_c.stat.def+=ddef;
	if(ddex) player->_c.stat.dex+=ddex;
	if(dres) player->_c.stat.res+=dres;
	player->_c.form=cls;
}
