#include "diabaig.h"

static void display_entity(Entity *e)
{
	char c;
	if(e)
	{
		//wmove(win,e->pos.y+1, e->pos.x+1);
		if(e->flags & ISOBJ) waddch(win,e->_o.type);
		else if(e->flags & ISCREATURE)
		{

			int status_list[10]={0,0,0,0,0,0,0,0,0,0};
			int n_statuses=0;

			/*
			if(e->_c.flags & ISBURN) 	wattron(win,COLOR_PAIR(STATUS_BURN));
			if(e->_c.flags & ISPOISON) 	wattron(win,COLOR_PAIR(STATUS_POISON));
			if(e->_c.flags & ISFREEZE) 	wattron(win,COLOR_PAIR(STATUS_FREEZE));
			if(e->_c.flags & ISREGEN) 	wattron(win,COLOR_PAIR(STATUS_REGEN));
			if(e->_c.flags & ISBOUND) 	wattron(win,COLOR_PAIR(STATUS_BIND));
			if(e->_c.flags & ISINKED) 	wattron(win,COLOR_PAIR(STATUS_INK));
			*/
			//if(e->_c.flags & ISSPEED) wattron(win,A_ITALIC);
			if(e->_c.flags & ISBURN) status_list[n_statuses++]	=A_BOLD|STATUS_BURN;
			if(e->_c.flags & ISPOISON) status_list[n_statuses++]=STATUS_POISON;
			if(e->_c.flags & ISFREEZE) status_list[n_statuses++]=STATUS_FREEZE;
			if(e->_c.flags & ISREGEN) status_list[n_statuses++]	=STATUS_REGEN;
			if(e->_c.flags & ISBOUND) status_list[n_statuses++]	=STATUS_BIND;
			if(e->_c.flags & ISINKED) status_list[n_statuses++]	=STATUS_INK;
			if(e->_c.flags & ISARCANE) status_list[n_statuses++]=STATUS_ARC;

			if(n_statuses) wattron(win,COLOR_PAIR(status_list[rng(n_statuses)]));

			if(tileat(e->pos.x,e->pos.y)->air_pressure>0.05)
			{
				int air=tileat(e->pos.x,e->pos.y)->air;
				if(air==COMBUST) wattron(win, COLOR_PAIR(STATUS_BURN));
				if(air==MIST)    wattron(win, COLOR_PAIR(STATUS_FREEZE));
				if(air==MIASMA)  wattron(win, A_REVERSE|COLOR_PAIR(STATUS_POISON));
			}

			switch(e->_c.type)
			{
				case 'm': 
					if(e->_c.form) c=e->_c.form;
					else c='m';
					break;
				case 't':
					if(e->_c.form==tFIRE) wattron(win,COLOR_PAIR(STATUS_BURN));
					if(e->_c.form==tFROST) wattron(win,COLOR_PAIR(STATUS_FREEZE));
					if(e->_c.form==tROT) wattron(win,COLOR_PAIR(STATUS_POISON));
					c='t';
					break;
				case 'v':
					if(e->_c.form==vBAT) c='b';
					else c='v';
					break;
				case 'V':
					if(e->_c.form==vBAT) c='b';
					else c='V';
					break;
				case 'x':
					if(e->_c.form==xPHYLACTERY) c='0';
					else c='x';
					break;
				case 'I':
					c=e->_c.form;
					break;

				default:
					c=e->_c.type;
					break;
			}

			if(player->_c.flags&ISLSD) 
			{
				if(e->_c.type>'a')	c=rng(26)+'a';
				else c=rng(26)+'A';
			}
			if(e==player && player->_c.flags & ISINVIS ) wattron(win,A_REVERSE);//c='_';

			if(!(e->_c.flags&ISINVIS) || e==player) waddch(win,c);

			wattroff(win,	COLOR_PAIR(STATUS_BURN)|
							COLOR_PAIR(STATUS_FREEZE)|
							COLOR_PAIR(STATUS_POISON)|
							COLOR_PAIR(STATUS_REGEN) |
							COLOR_PAIR(STATUS_BIND) |
							COLOR_PAIR(STATUS_INK) |
							A_ITALIC|A_BOLD|A_DIM|A_REVERSE);
		}
	}
}

static void display_spells()
{
	wmove(win,MSGY+3,MSGX);
	wclrtoeol(win);
	int slot=1;
	for(spell *sp=db.spells; sp<&db.spells[NSPELLS]; sp++)
	{
		if(sp->type==SP_NOTLEARNT) ;//waddstr(win,"[slot empty] ");
		else
		{
			//_log("disSPELL type %d slot %d %s",sp->type,slot,spell_info[sp->type].spell_name);
			if(spell_info[sp->type].mastery==-1) wattron(win,A_BOLD);
			wprintw(win,"[%d]%s %d%% ",slot++, spell_info[sp->type].spell_name, (int)(100*sp->charge/sp->cooldown));
			wattroff(win,A_BOLD);
		}
	}

}
static void display_hud()
{
	wmove(win,MSGY+4,MSGX);
	wclrtoeol(win);

	float total_str=dicemean(player->_c.stat.str);
	int total_def=player->_c.stat.def;
	int total_res=player->_c.stat.res;


	if(db.cur_mainhand!=-1) total_str+=dicemean(db.objects[db.cur_mainhand]._o.mod_melee);
	if(db.cur_offhand!=-1) 
	{
		total_str+=dicemean(db.objects[db.cur_offhand]._o.mod_melee);
		total_def+=db.objects[db.cur_offhand]._o.mod_def;
	}

	if(db.cur_armour!=-1) 
	{
		total_def+=db.objects[db.cur_armour]._o.mod_def;
		total_res+=db.objects[db.cur_armour]._o.mod_res;
	}
	/*
	if(player->_c.flags & ISVULN) 
	{
		total_def=0;
		total_res=0;
	}
	*/

	//if(db.cur_level==LICHLEVEL) wprintw(win,"lich's lair ");
	if(db.cur_level==SECONDARYBOSS) wprintw(win,"dragon's lair ");
	else wprintw(win,"floor:%d ",db.cur_level+1);

	wprintw(win,"gold:%d ",db.gold);

	wprintw(win,"hp:");
	if(((float)player->_c.stat.hp/player->_c.stat.maxhp)<=0.2) wattron(win,A_BOLD|COLOR_PAIR(C_RED));
	wprintw(win,"%d",player->_c.stat.hp);
	wattroff(win,A_BOLD|COLOR_PAIR(C_RED));
	wprintw(win,"/%d ",player->_c.stat.maxhp);

	//lol inefficient much
	if(db.cur_mainhand!=-1 && (	db.objects[db.cur_mainhand]._o.which==SHORTBOW ||
								db.objects[db.cur_mainhand]._o.which==LONGBOW ||
								db.objects[db.cur_mainhand]._o.which==RECURVEBOW) )
	{
		for(int i=0;i<26;i++)
		{
			if(db.inventory[i]!=-1 && (db.objects[db.inventory[i]]._o.type==WEAPON) && (db.objects[db.inventory[i]]._o.which==ARROW))
			{
				wprintw(win,"arr:%d ",db.objects[db.inventory[i]]._o.quantity);
				break;
			}
		}
	}
	else wprintw(win,"str:%d ",(int)ceil(total_str));

	wprintw(win,"def:%d ",total_def);
	wprintw(win,"res:%d ",total_res);
	if(player->_c.res_flags )
	{
		waddch(win,'[');
		if(player->_c.res_flags & IMMUNE_FIRE) wprintw(win,"BI");
		else if(player->_c.res_flags & RESIST_FIRE) wprintw(win,"B+");
		else if(player->_c.res_flags & WEAKTO_FIRE) wprintw(win,"B-");
		if(player->_c.res_flags & IMMUNE_FROST) wprintw(win,"FI");
		else if(player->_c.res_flags & RESIST_FROST) wprintw(win,"F+");
		else if(player->_c.res_flags & WEAKTO_FROST) wprintw(win,"F-");
		if(player->_c.res_flags & IMMUNE_POISON) wprintw(win,"PI");
		else if(player->_c.res_flags & RESIST_POISON) wprintw(win,"P+");
		else if(player->_c.res_flags & WEAKTO_POISON) wprintw(win,"P-");
		waddstr(win,"] ");
	}
	//waddch(win,ACS_UARROW);
	//waddch(win,ACS_DARROW);

	if(db.cur_level!=SECONDARYBOSS) wprintw(win,"xp:%d/%d ",db.xp,db.xplvl+1);

	wprintw(win,"status:");


	/*
	wprintw(win,"floor:%d gold:%d hp:%d/%d str:%d def:%d res:%d xp:%d/%d status:",
			db.cur_level+1,
			db.gold,
			player->_c.stat.hp,
			player->_c.stat.maxhp,
			total_str,
			total_def,
			total_res,
			db.xp,db.xplvl+1
			//status
		   );
		   */
	int flag=player->_c.flags;

	if(flag&ISPOISON)
	{
		wattron(win,COLOR_PAIR(STATUS_POISON));
		wprintw(win,"POI ");
		wattroff(win,COLOR_PAIR(STATUS_POISON));
	}
	if(flag&ISBURN)
	{
		wattron(win,COLOR_PAIR(STATUS_BURN));
		wprintw(win,"BRN");
		wattroff(win,COLOR_PAIR(STATUS_BURN));
		wprintw(win," ");
	}
	if(flag&ISFREEZE)
	{
		wattron(win,COLOR_PAIR(STATUS_FREEZE));
		wprintw(win,"FRZ");
		wattroff(win,COLOR_PAIR(STATUS_FREEZE));
		wprintw(win," ");
	}
	if(flag&ISREGEN)
	{
		wattron(win,COLOR_PAIR(STATUS_REGEN));
		wprintw(win,"RGN ");
		wattroff(win,COLOR_PAIR(STATUS_REGEN));
	}
	if(flag&ISBOUND)
	{
		wattron(win,COLOR_PAIR(STATUS_BIND));
		wprintw(win,"BND");
		wattroff(win,COLOR_PAIR(STATUS_BIND));
		wprintw(win," ");
	}
	if(flag&ISSPEED)
	{
		wattron(win,A_ITALIC);
		wprintw(win,"SPD ");
		wattroff(win,A_ITALIC);
	}
	if(flag&ISINKED)
	{
		wattron(win,COLOR_PAIR(STATUS_INK));
		wprintw(win,"INK ");
		wattroff(win,COLOR_PAIR(STATUS_INK));
	}
	if(flag&ISARCANE)
	{
		wattron(win,COLOR_PAIR(STATUS_ARC));
		wprintw(win,"ARC");
		wattroff(win,COLOR_PAIR(STATUS_ARC));
		wprintw(win," ");
	}

	if(db.hunger<150) 
	{
		wattron(win,A_BLINK);
		if(db.hunger<FAINT_LIMIT) wprintw(win,"FNT ");
		else wprintw(win,"STV ");
		wattroff(win,A_BLINK);
	}
	else if(db.hunger<500) wprintw(win,"HNG ");

	char status[32]={'\0'};
	if(player->_c.flags&ISCONFUSED) strcat(status,"CON ");
	if(player->_c.flags&ISINVIS) strcat(status,"INV ");
	if(player->_c.flags&ISSLOW) strcat(status,"SLO ");
	if(player->_c.flags&ISSLEEP) strcat(status,"SLP ");
	if(player->_c.flags&ISBLIND) strcat(status,"BLD ");
	if(player->_c.flags&ISVULN) strcat(status,"VLN ");
	if(player->_c.flags&ISLSD) strcat(status,"LSD");
	if(player->_c.flags&ISREBIRTH) strcat(status,"CUR");

	//for(_daemon *d=db.daemons; (d-db.daemons<NDAEMONS); d++)
	//{
	//	if(d->c_id==player->id && (d->type==D_ARCANEBARRIER || d->type==D_ABSORPTION))
	//	{
	//		strcat(status,"ARC");
	//		break;
	//	}
	//}
	//if(player->_c.flags&ISAGRO) strcat(status,"AGR ");
	//if(player->_c.flags&CANTRACK) strcat(status,"CNT ");

	wprintw(win,"%s",status);
	//wprintw(win,"%d",db.hunger);
}

void display()
{
	int x,y,c;
	Entity *e;
	tile *t;
	for(x=0;x<XMAX;x++)
	for(y=0;y<YMAX;y++)
	{
		wmove(win,y,x);
		t=tileat(x,y);
		//e=moat(x,y);
		//_log("%d %d %ld %ld",x,y,&db.tiles[0],t);
		//if(e) _log("%d %d %ld %c %d",x,y,db.tiles[0].creature,e->_c.type,e->id);
		//_log("%d %d %ld ",x,y,moat(x,y));
		if(moat(x,y) &&  moat(x,y)->flags  & ISMARKED) display_entity(moat(x,y));
		if(objat(x,y) && objat(x,y)->flags & ISMARKED) display_entity(objat(x,y));
		c=t->c;
		if(t->flags & ML_VISIBLE)
		{

			if(t->air_pressure>=0.05)
			{
				if(t->air==COMBUST) wattron(win,COLOR_PAIR(FLUID_COL_COMBUST));
				if(t->air==MIST)    wattron(win,COLOR_PAIR(FLUID_COL_MIST));
				if(t->air==MIASMA)  wattron(win,COLOR_PAIR(FLUID_COL_MIASMA));
				if(t->air==SMOKE)  	wattron(win,COLOR_PAIR(FLUID_COL_SMOKE));
			}
			if(t->air_pressure>1.0) 	  c=ACS_CKBOARD;
			//else if(t->air_pressure>0.5)  c='=';
			else if(t->air_pressure>0.2)  c=ACS_CKBOARD;
			else if(t->air_pressure>0.15) c=ACS_CKBOARD;
			else if(t->air_pressure>0.1)  c='~';
			else if(t->air_pressure>0.05) c='.';

			if((e=moat(x,y)) && (!(e->_c.flags &ISINVIS)||e==player)) display_entity(e);
			else if((e=objat(x,y))) display_entity(e);
			else waddch(win,c);

			wattroff(win,COLOR_PAIR(FLUID_COL_COMBUST)|COLOR_PAIR(FLUID_COL_MIST)|COLOR_PAIR(FLUID_COL_MIASMA)|COLOR_PAIR(FLUID_COL_SMOKE));
		}
		else if(tflags(x,y) & MS_EXPLORED)
		{
			wattron(win,A_DIM);
			waddch(win,c); //for now
			//waddch(win,t->c); //for now
		}
		else waddch(win,' ');
		wattroff(win,A_DIM|A_BOLD);
	}

	display_spells();
	display_hud();

	wmove(win,MSGY,MSGX);
	wclrtoeol(win);
	waddstr(win,message);

	wmove(win,MSGY+1,MSGX);
	wclrtoeol(win);
	waddstr(win,message_queue[0]);

	wmove(win,MSGY+2,MSGX);
	wclrtoeol(win);
	waddstr(win,message_queue[1]);


	if(db.cur_level==SECONDARYBOSS) boss_bar(getdragon());

	display_tutorial();

	wborder(win,0,0,0,0,0,0,0,0);
	//refresh();
	wrefresh(win);
}



void display_file(const char *fname)
{
	FILE *fp;
	char buf;
	if((fp=fopen(fname,"r")))
	{
		wclear(win);
		wrefresh(win);
		while(fread(&buf,1,1,fp)) wprintw(win,"%c",buf);
		wborder(win,0,0,0,0,0,0,0,0);
		wrefresh(win);

	}
	else
	{
		perror(fname);
		_log("failed to open file \"%s\" errorno:%s",fname,strerror(errno));
		msg("failed to open file \"%s\" errorno:%s",fname,strerror(errno));
	}
	fclose(fp);
	wrefresh(win);
}

void display_dathead(const char dat[], const int size)
{
	wclear(win);
	wrefresh(win);
	waddnstr(win,dat,size);
	wborder(win,0,0,0,0,0,0,0,0);
	wrefresh(win);
	noecho();//?
}

void display_scores()
{
	FILE *fp;
	struct _score s;

	display_dathead(res_halloffame_txt, res_halloffame_txt_len);

	if(!(fp=fopen(scorefile,"rb")))
	{
		perror(scorefile);
		return;
	}

	int yy=YMAX-7;
	//wmove(win,yy-1,1);
	//wprintw(win,"High Scores");

	for(int i=0; i<10;i++) 
	{
		fread(&s,sizeof(struct _score),1,fp);
		if(s.gold)
		{
			wmove(win,yy+i,1);
			wprintw(win,"%2d) %4d %s\n",i+1,s.gold,s.message);
		}
	}
	fclose(fp);

	//waddstr(win,"  press any key to return");
	wborder(win,0,0,0,0,0,0,0,0);
	//display_frameheader("HALL OF FAME");
	wrefresh(win);
	wgetch(win);
}

void display_tutorial()
{
	char *s="+-------------------------------------------+\n\
  | Basic Controls:                           |\n\
  | > move/attack (arrows/numpad/hjkl/MOUSE)  |\n\
  | > eat (e), read (r), drink (d), throw (t) |\n\
  | > show inventory (i)                      |\n\
  | > show full controls (?)                  |\n\
  +-------------------------------------------+";
	int x=2,y=1;


	if(search_daemon(player,D_TUTORIAL))
	{
		if(player->pos.y<(YMAX/2)) y=YMAX-8;
		wmove(win,y,x);
		waddstr(win,s);
	}
}

void display_credits()
{
	display_dathead(res_credits_txt, res_credits_txt_len);
	wgetch(win);
}

void boss_bar(Entity *e)
{
	int hp,mhp;
	char name[3*DRAGONNAMESIZE];
	float prc;
	int n,N;

	sprintf(name,"%s the %s ",dragonname, dragon_mod);
	if(e && (e->flags&ISACTIVE))
	{
		hp=e->_c.stat.hp;
		mhp=e->_c.stat.maxhp;
		prc=(float)hp/(float)mhp;

		N=XMAX-4-strlen(name);

		wmove(win,1,2);
		waddstr(win,name);
		waddch(win,'|');
		for(n=(int)round(N*prc);n>0;n--) waddch(win,'=');
		wmove(win,1,XMAX-2);
		waddch(win,'|');
	}
}

void display_frameheader(char *s)
{
	wmove(win,0,1);
	wprintw(win,"| %s |",s);
	wmove(win,1,0);
}

void display_dijkstra()
{
	for(int x=0; x<XMAX; x++)
	for(int y=0; y<YMAX; y++)
	{
		wmove(win,y,x);
		if(dijk_nodes[y*XMAX+x].weight!=DIJK_MAX) waddch(win, '0'+((int)dijk_nodes[y*XMAX+x].weight)%10);
		//if(dijk_nodes[y*XMAX+x].weight!=DIJK_MAX) waddch(win,'0'+ (int)round(10+dijk_nodes[y*XMAX+x].weight)%10 );
	}
	wrefresh(win);

}
