#include "diabaig.h"

struct conf conf_default()
{
	struct conf c;
	c.move_north='k';
	c.move_south='j';
	c.move_east='l';
	c.move_west='h';
	c.move_northeast='u';
	c.move_northwest='y';
	c.move_southeast='n';
	c.move_southwest='b';
	//c.descend='>';
	//c.ascend='<';

	c.drink='d';
	c.read='r';
	c.equip='w';
	c.throw='t';
	c.eat='e';
	c.apply='a';
	c.inventory='i';
	c.search='s';
	c.fire='f';
	c.rest='z';
	return c;
}

int conf_save(const char *fname)
{
	FILE *fp;
	int status;
	if((fp=fopen(fname,"wb")))
	{
		fwrite(&conf_diabaig, sizeof(struct conf), 1, fp);
		status=0;
	}
	else
	{
		status=1;
		perror(fname);
	}
	return status;
}

int conf_load(const char *fname)
{
	FILE *fp;
	int status;
	if((fp=fopen(fname,"rb")))
	{
		fread(&conf_diabaig, sizeof(struct conf),1,fp);
		status=0;
	}
	else
	{
		status=1;
		//perror(fname);
	}
	return status;
}

int cmpint(const void *a, const void *b)
{
	return (*(int *)a- *(int *)b);
}
int conf_check()
{
	int c=0;
	int keys[]={conf_diabaig.move_north, conf_diabaig.move_east, conf_diabaig.move_south, conf_diabaig.move_west, 
				conf_diabaig.move_northeast, conf_diabaig.move_northwest, conf_diabaig.move_southeast, conf_diabaig.move_southwest, 
				conf_diabaig.drink,conf_diabaig.read,conf_diabaig.equip,conf_diabaig.throw,
				conf_diabaig.eat,conf_diabaig.apply,conf_diabaig.inventory,conf_diabaig.search,conf_diabaig.fire,
				conf_diabaig.rest};
	//int count[17];
	//memset(count,0,sizeof(int)*17);
	int size=sizeof(keys)/sizeof(int);

	if(sizeof(keys)!=sizeof(struct conf)) c=-1;
	else
	{
		qsort(keys,size,sizeof(int),cmpint);
		for(int i=1; i<size; i++)
		{
			if(keys[i]==keys[i-1])
			{
				c=keys[i];
				break;
			}
		}
	}

	return c;
}

void overlay_conf()
{
	const char *s="[   ]";
	int x=4, y=7;
	wmove(win,y+0,x-2); waddstr(win,s); wmove(win,y+0,x); waddch(win, conf_diabaig.move_north);
	wmove(win,y+1,x-2); waddstr(win,s); wmove(win,y+1,x); waddch(win, conf_diabaig.move_south);
	wmove(win,y+2,x-2); waddstr(win,s); wmove(win,y+2,x); waddch(win, conf_diabaig.move_west);
	wmove(win,y+3,x-2); waddstr(win,s); wmove(win,y+3,x); waddch(win, conf_diabaig.move_east);
	wmove(win,y+4,x-2); waddstr(win,s); wmove(win,y+4,x); waddch(win, conf_diabaig.move_northwest);
	wmove(win,y+5,x-2); waddstr(win,s); wmove(win,y+5,x); waddch(win, conf_diabaig.move_northeast);
	wmove(win,y+6,x-2); waddstr(win,s); wmove(win,y+6,x); waddch(win, conf_diabaig.move_southwest);
	wmove(win,y+7,x-2); waddstr(win,s); wmove(win,y+7,x); waddch(win, conf_diabaig.move_southeast);
	//wmove(win,y+8,x); waddch(win, conf_diabaig.descend);
	//wmove(win,y+9,x); waddch(win, conf_diabaig.ascend);

	wmove(win,y+9, x-2); waddstr(win,s); wmove(win,y+9, x); waddch(win, conf_diabaig.eat);
	wmove(win,y+10,x-2); waddstr(win,s); wmove(win,y+10,x); waddch(win, conf_diabaig.drink);
	wmove(win,y+11,x-2); waddstr(win,s); wmove(win,y+11,x); waddch(win, conf_diabaig.read);
	wmove(win,y+12,x-2); waddstr(win,s); wmove(win,y+12,x); waddch(win, conf_diabaig.throw);
	wmove(win,y+13,x-2); waddstr(win,s); wmove(win,y+13,x); waddch(win, conf_diabaig.apply);
	wmove(win,y+14,x-2); waddstr(win,s); wmove(win,y+14,x); waddch(win, conf_diabaig.fire);
	wmove(win,y+15,x-2); waddstr(win,s); wmove(win,y+15,x); waddch(win, conf_diabaig.search);
	wmove(win,y+16,x-2); waddstr(win,s); wmove(win,y+16,x); waddch(win, conf_diabaig.inventory);
	wmove(win,y+17,x-2); waddstr(win,s); wmove(win,y+17,x); waddch(win, conf_diabaig.rest);
	//wmove(win,101,4); waddch(win, conf_diabaig.move_north);
}

void conf_set()
{
	int input=0;
	int id=0;
	int max=19;
	int rolling=1;

	//wclear(win);
	//wrefresh(win);
	display_dathead(res_conf_txt, res_conf_txt_len);
	//display_dathead(docs_credits_txt, docs_credits_txt_len);
	//wborder(win,0,0,0,0,0,0,0,0);
	//wrefresh(win);

	int di=0;
	int RESTORE_DEF=17;
	int RETURN=18;

	struct conf tmp;
	memcpy(&tmp,&conf_diabaig,sizeof(struct conf));
	while(rolling)
	{
		
		overlay_conf();

		input=wgetch(win);
		wmove(win,id+di+7,1);
		waddch(win,' ');

		switch(input)
		{
			case KEY_DOWN:case KEY_RIGHT: id= (id+1)%max; break;
			case KEY_UP: case KEY_LEFT:   id= (id+max-1)%max;break;

			 //not allowed
			case '1': case '2': case '3':
			case '>': case '<':
			case '?': case ':':
			case 'Q': case 'S':
			case ',': case '.':
			case 'D': case 'W':
			case 'Y': case 'U': case 'B': case 'N':
			case 'H': case 'J': case 'K': case 'L':
			case ' ':
										  wmove(win,25,4);
										  wprintw(win,"Key '%c' is reservered",input);
				 break;

			case '\n':
				 if(id==RESTORE_DEF) conf_diabaig=conf_default();
				 if(id==RETURN) rolling=0;
				 break;

			case KEY_BACKSPACE: 
			case 127:
			case '\b':
				 rolling=0;break;


			default:
				 switch(id)
				 {
					 case 0: conf_diabaig.move_north=input; break;
					 case 1: conf_diabaig.move_south=input; break;
					 case 2: conf_diabaig.move_west=input; break;
					 case 3: conf_diabaig.move_east=input; break;
					 case 4: conf_diabaig.move_northwest=input; break;
					 case 5: conf_diabaig.move_northeast=input; break;
					 case 6: conf_diabaig.move_southwest=input; break;
					 case 7: conf_diabaig.move_southeast=input; break;

					 case 8: conf_diabaig.eat=input; break;
					 case 9: conf_diabaig.drink=input; break;
					 case 10: conf_diabaig.read=input; break;
					 case 11: conf_diabaig.throw=input; break;
					 case 12: conf_diabaig.apply=input; break;
					 case 13: conf_diabaig.fire=input; break;
					 case 14: conf_diabaig.search=input; break;
					 case 15: conf_diabaig.inventory=input; break;
					 case 16: conf_diabaig.rest=input; break;



				 }
		}
		if(id<8) di=0;
		if(id>=8) di=1;
		if(id>=17) di=4;

		wmove(win,id+di+7,1);
		waddch(win,'>');
		wrefresh(win);

		int dup=conf_check();
		wmove(win, 26,2);
		if(dup)
		{
			wattron(win,A_BLINK);
			wprintw(win,"WARNING! duplicate key bindings '%c'",dup);
			wattroff(win,A_BLINK);
			rolling=1;
		}
		else wclrtoeol(win);

		wborder(win,0,0,0,0,0,0,0,0);
	}
	if(memcmp(&conf_diabaig,&tmp,sizeof(struct conf))) conf_save(".diabaigrc");
}
