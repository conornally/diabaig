#include "diabaig.h"

struct save_header {
	char pname[PLAYERNAMESZ];
	int floor, gold;
	int date[3];
	char version[16];
};

//static int fexists(char *fname);
//static int get_saveslot();
static char *slot_name(int i);
int save_screen();
/*
 * I kind of messed this up from the start by making the 
 * architecture really awful. Rather than fixing it and 
 * potentially breaking things, i am just going to try and
 * deal with it in the save/load stuff here.
 * Realistically though, it wont be perfect
 */
int quit()
{
	int status=RETURN_UNDEF;
	wclear(win);
	wborder(win,0,0,0,0,0,0,0,0);
	display_frameheader("Quit?");
	wmove(win,2,2);
	wprintw(win,"are you sure you want to quit? [y/N]");
	wmove(win,3,2);
	wprintw(win,"this will not save the game.");

	int input=wgetch(win);
	if(input=='y' || input=='Y')
	{
		running=false;
		set_ripdata(RIP_QUIT,"the quest early");
		status=RETURN_SUCCESS;
	}
	return status;
}

int _save(const char *fname)
{
	int status=0;
	FILE *fp=NULL;


	MKDIR("save",0755);

	if(!(fp=fopen(fname,"wb")))
	{
		perror(fname);
		status=1;
	}
	else
	{
		// Do I actually still need to do this?
		for(int id=0; id<(XMAX*YMAX);id++) 
		{
			db.tiles[id].creature=NULL;
			db.tiles[id].obj=NULL;
		}

		struct save_header head;
		time_t now=time(NULL);
		struct tm *_tm=gmtime(&now);

		strcpy(head.pname, playername);
		strcpy(head.version, VERSION);
		head.floor=db.cur_level;
		head.gold=db.gold;
		head.date[0]=_tm->tm_mday;
		head.date[1]=_tm->tm_mon+1;
		head.date[2]=_tm->tm_year+1900;

		fwrite(&head,sizeof(struct save_header),1,fp);

		fwrite(&db,sizeof(struct _database),1,fp);
		fwrite(&verbose,sizeof(verbose),1,fp);
		fwrite(&message,sizeof(message),1,fp);
		fwrite(&message_queue,sizeof(message_queue),1,fp);
		fwrite(&message_history,sizeof(message_history),1,fp);
		fwrite(&playername,sizeof(playername),1,fp);
		
		fwrite(&wizardmode,sizeof(wizardmode),1,fp);
		fwrite(&testarena,sizeof(testarena),1,fp);
		fwrite(&bfs_nodes,sizeof(bfs_nodes),1,fp);
		fwrite(&path_memory,sizeof(bfs_nodes),1,fp);


		fwrite(&scroll_info,sizeof(scroll_info),1,fp);
		fwrite(&potion_info,sizeof(potion_info),1,fp);
		fwrite(&ring_info,sizeof(ring_info),1,fp);
		fwrite(&spell_info,sizeof(spell_info),1,fp);
		
		fwrite(&potion_colours,sizeof(potion_colours),1,fp);
		fwrite(&metal_types,sizeof(potion_colours),1,fp);

		fclose(fp);
	}
	return status;
}


void autosave()
{
	char *fname=slot_name(0);
	_save(fname);
	free(fname);
}

void delete_autosave()
{
	char *fname=slot_name(0);
	remove(fname);
	free(fname);
}


int _load(const char *fname)
{
	int status=0;
	FILE* fp=NULL;
	if(!(fp=fopen(fname,"rb")))
	{
		perror(fname);
		status=1;
	}
	else
	{
		struct save_header head;
		fread(&head,sizeof(struct save_header),1,fp);

		fread(&db,sizeof(struct _database),1,fp);
		fread(&verbose,sizeof(verbose),1,fp);
		fread(&message,sizeof(message),1,fp);
		fread(&message_queue,sizeof(message_queue),1,fp);
		fread(&message_history,sizeof(message_history),1,fp);
		fread(&playername,sizeof(playername),1,fp);

		fread(&wizardmode,sizeof(wizardmode),1,fp);
		fread(&testarena,sizeof(testarena),1,fp);
		fread(&bfs_nodes,sizeof(bfs_nodes),1,fp);
		fread(&path_memory,sizeof(bfs_nodes),1,fp);

		fread(&scroll_info,sizeof(scroll_info),1,fp);
		fread(&potion_info,sizeof(potion_info),1,fp);
		fread(&ring_info,sizeof(ring_info),1,fp);
		fread(&spell_info,sizeof(spell_info),1,fp);

		fread(&potion_colours,sizeof(potion_colours),1,fp);
		fread(&metal_types,sizeof(potion_colours),1,fp);


		// HACK the map back into shape
		Entity* e;
		int id;
		for(id=0; id<DBSIZE_CREATURES; id++)
		{
			e=&db.creatures[id];
			if(e && (e->flags&ISACTIVE) && (db.cur_level==e->pos.z))
			{
				db.tiles[e->pos.y*XMAX+e->pos.x].creature=e;
				inroom(e);
			}
		}
		for(id=0; id<DBSIZE_OBJECTS; id++)
		{
			e=&db.objects[id];
			if(e && (e->flags&ISACTIVE) && (db.cur_level==e->pos.z))
			{
				int ininv=0;
				for(int iid=0;iid<26;iid++)
				{
					if(db.inventory[iid]==id) ininv=1;
				}
				if(!ininv) objat(e->pos.x, e->pos.y)=e;
			}
		}
		sprintf(message_queue[1], "successfully loaded: ");
		strncat(message_queue[1], loadfile, MSGSZ-strlen(message_queue[1])-1);
		//snprintf(message_queue[1], MSGSZ, "successfully loaded from %s",loadfile);
		fclose(fp);
	}


	return status;
}

int load(const char *fname)
{
	int status=0;
	char *backup;
	status=_load(fname);
	if(!status)
	{
		backup=slot_name(0);
		rename(fname,backup);
		remove(fname);
		free(backup);
	}
	return status;
}

int continue_screen()
{
	FILE *fp;
	struct save_header head;
	char *fname, *menu[SAVESLOTS+2];
	char pref[16];
	int selection, X=5, Y=10;
	int status=1,autosave=0,version=0;

	for(int i=0; i<SAVESLOTS+2; i++)
	{
		fname=slot_name(i);
		if(!i) sprintf(pref,"[recover]");
		else   sprintf(pref,"[save: %d]",i);

		menu[i]=malloc(64);
		memset(menu[i],'\0',64);
		if((fp=fopen(fname,"rb")))
		{
			fread(&head, sizeof(struct save_header), 1, fp);
			sprintf(menu[i], "%s: %02d/%02d/%d %s floor:%2d",pref,head.date[0], head.date[1], head.date[2], head.pname, head.floor+1);

			if(!i) autosave=1;
			if(strcmp(head.version,VERSION)) 
			{
				version=1;
				strcat(menu[i]," **");
			}
		}
		else sprintf(menu[i],"%s: -- empty --",pref);
		free(fname);
	}


	sprintf(menu[SAVESLOTS+1],"return to home");
	display_dathead(res_load_txt, res_load_txt_len);
	display_frameheader("Load Game");

	if(autosave)
	{
		wmove(win,2,4);
		waddstr(win,"RECOVERY FILE detected: if this is the result of a bug or game crash,");
	    wmove(win,3,4);
		waddstr(win,"please get in touch at https://github.com/conornally/diabaig/issues");
	}
	if(version)
	{
		wmove(win,5,4);
		waddstr(win,"** VERSION mismatch: save file version does not match current version,");
		wmove(win,6,4);
		waddstr(win,"there is a chance this will corrupt when opened");
	}

	selection=simple_menu(menu, SAVESLOTS+2, X,Y);
	for(int i=0; i<SAVESLOTS+2; i++) free(menu[i]);
	if(selection>=0 && selection<=SAVESLOTS)
	{
		fname=slot_name(selection);
		status=_load(fname);

		if(!status)
		{
			char *tmp=slot_name(0);
			rename(fname, tmp);
			remove(fname);
			free(tmp);
		}
		free(fname);
	}
	return status;
}

int save()
{
	//struct finfo fi;
	FILE *fp=NULL;
	struct save_header head;
	char *fname, *menu[SAVESLOTS+1], pref[16];
	int selection=-1;
	int X=5, Y=10;
	int status=RETURN_UNDEF;

	//menus[SAVESLOTS]=malloc(64);
	//strcpy(menus[SAVESLOTS],"return to diabaig");

	for(int i=1; i<=SAVESLOTS+1; i++)
	{
		fname=slot_name(i);
		sprintf(pref,"[save: %d]",i);
		menu[i-1]=malloc(64);
		memset(menu[i-1],'\0',64);
		if((fp=fopen(fname,"rb")))
		{
			fread(&head,sizeof(struct save_header),1,fp);
			sprintf(menu[i-1], "%s: %02d/%02d/%d %s floor:%2d",pref,head.date[0], head.date[1], head.date[2], head.pname, head.floor+1);
		}
		else sprintf(menu[i-1],"%s: -- empty --",pref);
	}

	sprintf(menu[SAVESLOTS],"return to diabaig");
	display_dathead(res_load_txt, res_load_txt_len);
	display_frameheader("Save Game");
	selection=simple_menu(menu, SAVESLOTS+1, X,Y);

	if(selection>=0 && selection<SAVESLOTS)
	{
		wmove(win,Y+SAVESLOTS+4, X+2);
		waddstr(win,"are you sure? y/N");
		wrefresh(win);
		if(wgetch(win)=='y') 
		{
			fname=slot_name(selection+1);
			if(!_save(fname)) 
			{
				running=0;
				status=RETURN_SUCCESS;
			}
			free(fname);
		}
	}
	return status;
}

int simple_menu(char *lst[], int len, int x, int y)
{
	int selection=0;
	int input=0;
	do
	{
		for(int i=0; i<len; i++)
		{
			wmove(win,y+i,x);
			if(i==selection)
			{
				wprintw(win,"> ");
				wattron(win,A_REVERSE);
			}
			else wprintw(win,"  ");
			waddstr(win,lst[i]);
			wattroff(win,A_REVERSE);
		}

		wrefresh(win);
		input=wgetch(win);
		switch(input)
		{
			case 'j': case KEY_DOWN: case KEY_RIGHT:
				selection=(selection+1)%len;
				break;
			case 'k': case KEY_UP: case KEY_LEFT:
				selection=(selection+len-1)%len;
				break;
			case KEY_BACKSPACE: case 127: case '\b':
				return -1;
		}

	}while(input!='\n');
	return selection;
}

//static int fexists(char *fname)
//{
//	struct stat buf;
//	return (STAT(fname,&buf)==0);
//}

//static int get_saveslot()
//{
//	char *fname=NULL;
//	int slot=0;
//	for(slot=1; slot<=SAVESLOTS; slot++)
//	{
//		fname=slot_name(slot);
//		if(!fexists(fname))
//		{
//			free(fname);
//			break;
//		}
//		free(fname);
//	}
//	return slot<SAVESLOTS ? slot : 0;
//}

static char *slot_name(int i)
{
	char *fname=malloc(64);
	if(!i) sprintf(fname,"save/diabaig.autosave");
	else sprintf(fname, "save/diabaig.save.%d",i);
	return fname;
}

