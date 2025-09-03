#include "diabaig.h"

int nwin,nlose;
struct _score scores[11];

static int _readscores(const char *fname);
static int _writescores(const char *fname);
static int _sortscores(const void *a, const void *b);
static void _display_score(struct _score s);
static void _print_gravestone();
static int _strcentre(char *s);

//static char *rip[] = {
//"                   __________              ",
//"                  /          \\            ",
//"                 /    HERE    \\           ",
//"                /     LIES     \\          ",
//"               /                \\         ",
//"              /                  \\        ",
//"              |                  |         ",
//"              |                  |         ",
//"              |                  |         ",
//"              |                  |         ",
//"              |                  |         ",
//"             *|     *  *  *      | *       ",
//"     ________)/\\\\_//(\\/(/\\)/\\//\\/|_)_______\n",
//0
//};

void set_ripdata(int by, char *name)
{
	static char *_killedby[]=
	{
		"killed by",
		"bested by",
		"beaten by",
		0
	};

	if(game_won) by=RIP_WIN; //a hack
	rip_data.by=by;

	switch(by)
	{
		case RIP_KILLED: strncpy(rip_data.cause,  _killedby[rng(clen(_killedby))],24);break;
		case RIP_DRANK:  strncpy(rip_data.cause,  "drank",24);break;
		case RIP_STARVE: strncpy(rip_data.cause,  "starved",24);break;
		case RIP_STATUS: strncpy(rip_data.cause,  "succumbed to",24);break;
		case RIP_BURN:	 strncpy(rip_data.cause,  "burnt", 24);break;
		case RIP_SUFFOCATE: strncpy(rip_data.cause,  "suffocated", 24);break;
		case RIP_QUIT: 	    strncpy(rip_data.cause,  "quit",24);break;
		case RIP_SAVE: 	    strncpy(rip_data.cause,  "saved",24);break;
		//case RIP_WIN: 	 strncpy(rip_data.cause,  "escaped with a tooth",24);break;
		case RIP_UNKOWN:
		default: strncpy(rip_data.cause,"died for",24);break;
	}
	switch(by)
	{
		case RIP_KILLED:
			snprintf(rip_data.name,24,"%s",name);
			break;
		case RIP_DRANK:
			snprintf(rip_data.name,24,"%s %s",get_prefix(name), name);
			break;
		case RIP_WIN:
			snprintf(rip_data.cause, 24, "defeated ");//,dragonname);
			strncat(rip_data.cause,dragonname, 24-strlen(rip_data.cause));
			snprintf(rip_data.name,24,"the %s",dragon_mod);

			break;
		default:
			strncpy(rip_data.name,name,24);
			break;
	}
}

//char *class_msgs[CLASSMAX]={"a rogue","a warrior","a monk","went mad and","a wizard"};

void player_die()
{
	// THIS function is awfully complex

	if(player->_c.flags&ISREBIRTH) 
		return player_rebirth();



	nwin=0; 
	nlose=0;

	running=false;
	delete_autosave();

	if(rip_data.by==RIP_SAVE) return;
	//if(wizardmode|testarena) return;

	wclear(win);
	memset(&scores,'\0',sizeof(struct _score)*11);

	struct _score s;
	s.gold=db.gold;

	if(rip_data.by==RIP_WIN)
	{
		snprintf(s.message,128, "%s: a %s %s %s",getname(player), classnames[player->_c.form], 
			rip_data.cause, rip_data.name);
		display_dathead(res_win_txt,res_win_txt_len);
		if(!wizardmode) nwin=1;

	}
	else
	{
		snprintf(s.message,128, "%s: a %s %s %s on floor %d",getname(player), classnames[player->_c.form], 
			rip_data.cause, rip_data.name, db.cur_level+1);
		display_dathead(res_gravestone_txt,res_gravestone_txt_len);
		_print_gravestone();
		if(!wizardmode) nlose=1;
	}
	

	if( !(wizardmode|testarena) && s.gold>0) scores[10]=s;

	_readscores(scorefile);
	qsort(scores,11,sizeof(struct _score),_sortscores);

	wmove(win,19,0);
	int tt=0,hs=0;
	for(int i=0;i<10;i++)
	{
		if(strlen(scores[i].message)>0 && (scores[i].gold>0)) //when less than 10 scores, it prints the nulls
		{
			if(!memcmp(&scores[i],&s,sizeof(struct _score))) //got onto the highscore list
			{
				if(i==0) tt=1;//wprintw(win," <-- High Score!");
				else     hs=1;//wprintw(win," <-- Top Ten!");
				wattron(win,A_BOLD|A_REVERSE);
			}
			wprintw(win," %2d)",i+1);
			_display_score(scores[i]);
			wprintw(win,"\n");
			wattroff(win,A_BOLD|A_REVERSE);
		}
	}
	wmove(win,17,16);
	if(tt) waddstr(win,"New High Score!");
	else if(hs) waddstr(win,"New Top Ten!");

	if(!game_won)
	{
		wmove(win, 8,56);
		wprintw(win,"%d",nlose);
		wmove(win, 10,57);
		wprintw(win,"%d",nwin);

		wmove(win,NROWS-2,2);
		wprintw(win,"Press SPACE to leave the graveyard.. ");
	}
	else
	{
		char tmp[64];
		sprintf(tmp,"%s %s",dragon_mod,dragonname);
		wmove(win,6,61-_strcentre(tmp));
		wattron(win,A_BOLD);
		waddstr(win,tmp);
		wattroff(win,A_BOLD);

		wmove(win, 15,57);
		wprintw(win,"%d",nlose);
		wmove(win, 17,58);
		wprintw(win,"%d",nwin);

		wmove(win,NROWS-2,2);
		wprintw(win,"Press SPACE to go home.. ");

	}

	_writescores(scorefile);
	wborder(win,0,0,0,0,0,0,0,0);
	if(game_won) display_frameheader("YOU BEAT DIABAIG!");
	else display_frameheader("YOU FELL IN DIABAIG");
	wrefresh(win);
	while((getch()!=' '));
}

static int _writescores(const char *fname)
{
	FILE *fp;
	if(!(fp=fopen(fname,"wb")))
	{
		perror(fname);
		return -1;
	}
	for(int i=0;i<10;i++)
	{
		fwrite(&scores[i],sizeof(struct _score),1,fp);
	}
	fwrite(&nwin,sizeof(int),1,fp);
	fwrite(&nlose,sizeof(int),1,fp);
	fclose(fp);
	return 0;
}

static int _readscores(const char *fname)
{
	int _win=0,_lose=0;
	FILE *fp;
	if(!(fp=fopen(fname,"rb")))
	{
		//perror(fname);
		return -1;
	}
	for(int i=0;i<10;i++)
	{
		fread(&scores[i],sizeof(struct _score),1,fp);
	}
	fread(&_win,sizeof(int),1,fp);
	fread(&_lose,sizeof(int),1,fp);
	nwin+=_win;
	nlose+=_lose;
	fclose(fp);
	return 0;
}

static int _sortscores(const void *a, const void *b)
{
	return ((struct _score*)b)->gold - ((struct _score*)a)->gold;
}

static void _display_score(struct _score s)
{
	int len=strlen(s.message);
	int i=0, space=0;
	char line[NCOLS-2];
	if(len >= (NCOLS-2))
	{
		while(i<NCOLS)
		{
			if(s.message[i]==' ') space=i;
			i++;
		}
		s.message[space]='\n';
	}
	snprintf(line, NCOLS-2, "%4d ",s.gold);//,s.message);
	strncat(line,s.message, NCOLS-2-strlen(line));
	waddstr(win,line);
}
static void _print_gravestone()
{
	int x=25;
	char scoreline[12];
	sprintf(scoreline,"score: %d",db.gold);

	wmove(win,6,x-_strcentre(playername));
	waddstr(win,playername);

	wmove(win,8,x-_strcentre(rip_data.cause));
	waddstr(win,rip_data.cause);

	wmove(win,9,x-_strcentre(rip_data.name));
	waddstr(win,rip_data.name);

	wmove(win,11,x-_strcentre(scoreline));
	waddstr(win,scoreline);

}
static int _strcentre(char *s)
{
	return (strlen(s)/2);
}

void murder(Entity *e, Entity *target)
{
	if(target)
	{
		if(target->flags&ISILLUSION)
		{
			if(e==player)msg("%s fades way, it was an illusion!",getname(target));
			clear_entity(target);
			return;
		}

		int n=0;
		Entity *item=NULL;
		Entity *creature=NULL;
		switch(target->_c.type)
		{
			case 'a':
				if(rng(3))
				{
					item=_new_obj(POTION);
				}
				break;
			case 'd':
			case 'i':
			case 'w':
			case 'n':
				if(!rng(4) && (db.nspellscrolls++<2))
				{
				   item=_new_obj(SCROLL);
				   _set_scroll(item,S_LEARNSPELL);
				}
				if( (target->_c.type=='n')||(target->_c.type=='i'))
				{
					for(Entity *c=db.creatures; c<&db.creatures[DBSIZE_CREATURES]; c++)
					{
						if(c && (c->flags&ISACTIVE) && (c->_c.parent==target->id))
						{
							add_daemon(c,D_SLOWDEATH,rng(10));
							n++;
						}
					}
					if(n && target->_c.type=='i') player_msg(e,"the illusions begin to fade away");
					if(n && target->_c.type=='n') player_msg(e,"the thralls of the necromancer begin to fall");
				}
				break;

			case 'g':
				if(rng(2))
				{
				   item=_new_obj(GOLD);
				   item->_o.which=0;
				   item->_o.quantity=1+rng(3);
				}
				break;

			case 'l':
				if(!rng(3))
				{
					item=_new_obj(WEAPON);
					_set_weapon(item,DAGGER);
					item->_o.quantity=1+rng(3);
				}
				break;

			case 'p':
				if(!rng(3))
				{
				   item=_new_obj(POTION);
				   _set_potion(item,P_INVISIBILITY);
				}
				break;

			case 's':
				if(!rng(3))
				{
				   item=_new_obj(POTION);
				   _set_potion(item,P_POISON);
				}
				break;

			case 'D':
				db.defeated_dragon=1;
				item=_new_obj(TRINKET);
				_set_trinket(item,TK_DRAGONTOOTH);

				break;

			case 'O':
				if(!rng(2))
				{
					item=_new_obj(POTION);
					_set_potion(item,P_CONFUSION);
				}
				break;

			case 'P':
				add_daemon(NULL, D_SPAWNPHOENIX, 30+rng(30));
				if(!rng(2))
				{
					item=_new_obj(POTION);
					_set_potion(item,P_HEALING);
				}
				break;

			case 'Y':
				if(!rng(2))
				{
					item=_new_obj(POTION);
					_set_potion(item,P_BURNING);
				}
				break;

			case 'U': case 'K':
				for(Entity *c=db.creatures; c<&db.creatures[DBSIZE_CREATURES]; c++)
				{
					if(c && (c->flags&ISACTIVE) && (c->_c.parent==target->id))
					{
						n++;
						c->_c.flags|=ISSCARED;
						c->_c.flags&=~ISAGRO;
						c->_c.target=0;
					}
				}
				if(n && target->_c.type=='U') msg("%s falls, his troops lose their resolve and flee",getname(target));
				if(n && target->_c.type=='K') msg("%s falls, his guards lose their resolve and flee",getname(target));
				break;
			case 'Z':
				if(rng(3))
				{
					item=_new_obj(POTION);
					_set_potion(item,P_HEALING);
				}
				break;
				

		}

		if(item)
		{
			item->pos=target->pos;
			Entity *tmp=objat(item->pos.x, item->pos.y);
			if(tmp && tmp->_o.type==item->_o.type && tmp->_o.which==item->_o.which)
			{
				tmp->_o.quantity+=item->_o.quantity;
				clear_entity(item);
			}
			else objat(item->pos.x,item->pos.y)=item;
		}
		if(creature)
		{
			creature->pos=target->pos;
			moat(creature->pos.x,creature->pos.y)=creature;
		}

		if(e==player && (tileat(target->pos.x, target->pos.y)->flags&ML_VISIBLE))
		{
			char *_defeatmsg[]={
				"%s defeated %s",
				"%s struck down %s",
				"%s bests %s",
				"%s has slain %s",
				0
			};
			char *name=strdup(getname(target));
			do_addxp(target);
			msg( _defeatmsg[ rng(clen(_defeatmsg)) ], getname(player), name);
			free(name);
		}
	}
	
	if(target != player && (target->id!=0)) //the latter term is a hack to stop regen being terminated incorrectly
	{
		for(_daemon *d=&db.daemons[0]; d<&db.daemons[NDAEMONS]; d++)
		{
			if(d->c_id==target->id) stop_daemon(d);
		}
		clear_entity(target);
	}
}


void player_rebirth()
{

	player->_c.stat.hp=player->_c.stat.maxhp;
	add_daemon(player, D_FASTREGEN, player->_c.stat.maxhp);
	player->_c.flags &= ~ISREBIRTH;
	tileat(player->pos.x,player->pos.y)->creature=NULL;

	room *r;
	do
	{
		r=&db.rooms[rng(db.nrooms)];
	}while(r==player->_c._inroom);
	placeinroom(r,player);


	//POOF OF SMOKE
	tile *t=tileat(player->pos.x, player->pos.y);
	for(int n=0; n<8; n++)
	{
		int id=t->neighbours[n];
		if(id>=0) 
		{
			db.tiles[id].air=SMOKE;
			db.tiles[id].air_pressure+=0.5;
		}
	}

	memset(message,'\0',sizeof(message));
	memset(message_queue,'\0',sizeof(message_queue));

	display_dathead(res_rebirth_txt,res_rebirth_txt_len);
	wmove(win,19,20);
	wprintw(win,"%s %s",rip_data.cause, rip_data.name);
	wrefresh(win);
	msg("you are thrust back into your body, you feel a renewed vigour");
	while(getch()!=' ');
	running=1;
}
