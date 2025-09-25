#include "diabaig.h"

int getdirection(coord a, coord b)
{
	int dx=b.x-a.x;
	int dy=b.y-a.y;
	int direction=nodir;

	if( (dx==0) && (dy==0)) direction=nodir;
	else if( (dx==0) && (dy>=0)) direction=south;
	else if( (dx==0) && (dy<=0)) direction=north;
	else if( (dx>=0) && (dy==0)) direction=east;
	else if( (dx<=0) && (dy==0)) direction=west;
	else if( (dx>=0) && (dy>=0)) direction=southeast;
	else if( (dx>=0) && (dy<=0)) direction=northeast;
	else if( (dx<=0) && (dy>=0)) direction=southwest;
	else if( (dx<=0) && (dy<=0)) direction=northwest;

	return direction;
}


int get_wall_direction(coord pos, int direction)
{
	int x=pos.x,y=pos.y;
	while(!obstructs(x,y))// || tileat(x,y)->c!=DOOR)
	{
		pos.x=x;
		pos.y=y;
		switch(direction)
		{
			case north: y--; break;
			case south: y++; break;
			case west: x--; break;
			case east: x++; break;
			case northeast: y--;x++;break;
			case northwest: y--;x--;break;
			case southeast: y++;x++;break;
			case southwest: y++;x--;break;
			default:
				return pos.y*XMAX+pos.x;
				break;
		}
		if( (x<0) || (x>=XMAX) || (y<0) || (y>=YMAX))break;
	}
	return pos.y*XMAX+pos.x;
}

int get_first_thing_direction(coord pos, int direction)
{
	int x=pos.x,y=pos.y;
	while(!obstructs(x,y))// || tileat(x,y)->c!=DOOR)
	{
		pos.x=x;
		pos.y=y;
		switch(direction)
		{
			case north: y--; break;
			case south: y++; break;
			case west: x--; break;
			case east: x++; break;
			case northeast: y--;x++;break;
			case northwest: y--;x--;break;
			case southeast: y++;x++;break;
			case southwest: y++;x--;break;
			default: break;
		}
		if( (x<0) || (x>=XMAX) || (y<0) || (y>=YMAX))break;
		if(moat(x,y))
		{
			pos.x=x;
			pos.y=y;
			break;
		}
		if(tileat(x,y)->c==DOOR) break;
	}
	return pos.y*XMAX+pos.x;
}

tile *get_tile_direct(Entity *src, int direction)
{
	tile *t=NULL;
	if(src)
	{
		int id=get_first_thing_direction(src->pos,direction);
		if(id>=0 && id<(XMAX*YMAX)) t=&db.tiles[id];
	}
	return t;
}

Entity *get_target_direct(Entity *src, int direction) 
{
	Entity *target=NULL;
	if(src)
	{
		int id=get_first_thing_direction(src->pos, direction);
		if(id>=0 && id<(XMAX*YMAX)) target=db.tiles[id].creature;
	}
	return target;
}

Entity **get_target_radius(Entity *src, int radius)
{
	int ncreatures=0;
	int tmp[DBSIZE_CREATURES];
	Entity **dest;
	memset(tmp,-1,sizeof(int)*DBSIZE_CREATURES);

	if(src)
	{
		for(Entity *e=db.creatures; e<&db.creatures[DBSIZE_CREATURES]; e++)
		{
			if((e->flags&ISACTIVE)&&(e->pos.z==src->pos.z))
			{
				if(sqrt( pow(src->pos.x-e->pos.x,2) + pow(src->pos.y-e->pos.y,2)) <=radius)
				{
					tmp[ncreatures++]=e->id;
				}
			}
		}
		dest=malloc((ncreatures+1)*sizeof(Entity*));
		dest[ncreatures]=NULL;
		for(int i=0;i<ncreatures;i++)
		{
			dest[i]=&db.creatures[tmp[i]];
		}
		return dest;
	}
	else return NULL;
}

Entity ** get_target_room(Entity *src)
{
	int ncreatures=0;
	int tmp[DBSIZE_CREATURES];
	Entity **dest;
	memset(tmp,-1,sizeof(int)*DBSIZE_CREATURES);

	if(src && src->_c._inroom)
	{
		for(Entity *e=db.creatures; e<&db.creatures[DBSIZE_CREATURES]; e++)
		{
			if((e->flags&ISACTIVE)&&(e->pos.z==src->pos.z))
			{
				if(e->_c._inroom==src->_c._inroom) tmp[ncreatures++]=e->id;
			}
		}
		dest=malloc((ncreatures+1)*sizeof(Entity*));
		dest[ncreatures]=NULL;
		for(int i=0; i<ncreatures; i++) dest[i]=&db.creatures[tmp[i]];
		return dest;
	}
	else return NULL;
}

Entity ** get_target_type(int type)
{
	int ncreatures=0;
	int tmp[DBSIZE_CREATURES];
	Entity **dest=NULL;
	memset(tmp,-1,sizeof(int)*DBSIZE_CREATURES);

	if(type>='A' && type<='z')
	{
		for(Entity *e=db.creatures; e<&db.creatures[DBSIZE_CREATURES]; e++)
		{
			if(e->flags&ISACTIVE && e->pos.z==db.cur_level && e->_c.type==type) tmp[ncreatures++]=e->id;
		}
		dest=malloc((ncreatures+1)*sizeof(Entity*));
		dest[ncreatures]=NULL;
		for(int i=0; i<ncreatures; i++) dest[i]=&db.creatures[tmp[i]];
		return dest;
	}
	else return NULL;
	//return dest;
}

Entity ** get_target_adjacent(Entity *src)
{
	int ncreatures=0;
	int tmp[DBSIZE_CREATURES];
	Entity **dst=NULL;
	memset(tmp,-1,sizeof(int)*DBSIZE_CREATURES);

	if(src)
	{
		for( Entity *e=db.creatures; e<&db.creatures[DBSIZE_CREATURES]; e++)
		{
			if((src!=e) && (e->flags&ISACTIVE) && (e->pos.z==src->pos.z))
			{
				if( abs(e->pos.x-src->pos.x)<=1 && abs(e->pos.y-src->pos.y)<=1)
					tmp[ncreatures++]=e->id;
			}
		}
		dst=malloc((ncreatures+1)*sizeof(Entity*));
		dst[ncreatures]=NULL;
		for(int i=0; i<ncreatures; i++) dst[i]=&db.creatures[tmp[i]];
		return dst;
	}
	else return NULL;
}

Entity ** get_target_visible()
{
	int ncreatures=0;
	int tmp[DBSIZE_CREATURES];
	Entity **dst=NULL;
	memset(tmp,-1,sizeof(int)*DBSIZE_CREATURES);
	light_room(inroom(player));
	light_local_area();

	for( Entity *e=db.creatures; e<&db.creatures[DBSIZE_CREATURES]; e++)
	{
		if(e && e!=player && (e->flags&ISACTIVE) && e->pos.z==player->pos.z)
		{
			if(tileat(e->pos.x,e->pos.y)->flags&ML_VISIBLE)
			{
				tmp[ncreatures++]=e->id;
			}
		}
	}
	dst=malloc((ncreatures+1)*sizeof(Entity*));
	dst[ncreatures]=NULL;
	for(int i=0; i<ncreatures; i++) dst[i]=&db.creatures[tmp[i]];
	return dst;
}


int pick_direction()
{
	int direction;
	light_local_area();
	display();

	wmove(win,MSGY+2,MSGX);
	wclrtoeol(win);
	wborder(win,0,0,0,0,0,0,0,0);
	wprintw(win,"pick direction: ");
	wrefresh(win);
	switch(getch())
	{
		case 'h': case KEY_LEFT: direction=west; break;
		case 'j': case KEY_DOWN: direction=south; break;
		case 'k': case KEY_UP: 	 direction=north; break;
		case 'l': case KEY_RIGHT:direction=east; break;
		case 'y': case KEY_A1: case KEY_HOME: 	direction=northwest; break;
		case 'u': case KEY_A3: case KEY_PPAGE: 	direction=northeast; break;
		case 'b': case KEY_C1: case KEY_END: 	direction=southwest; break;
		case 'n': case KEY_C3: case KEY_NPAGE: 	direction=southeast; break;

		default: direction=nodir; break;
	}
	if(player->_c.flags&ISCONFUSED) direction= 1+rng(8);
	return direction;
}

