#include "diabaig.h"

// THIS HAS SPAGHETTIFIED PRETTY BAD NOW
enum modes{
	NONE,
	REST,
	STRAIGHT,
	FOLLOW,
	EXPLORE,
};

struct _autopilot autopilot;
static int followpath();

int c_inroom()
{
	Entity e;
	for(int id=1; id<DBSIZE_CREATURES; id++)
	{
		e=db.creatures[id];
		if( (e.flags&ISACTIVE) && (e.pos.z==db.cur_level) && e._c._inroom && (e._c._inroom==player->_c._inroom))
		{
			return 1;
		}
	}
	return 0;
}

int c_adjacent(coord p)
{
	Entity e;
	for(int id=1; id<DBSIZE_CREATURES; id++)
	{
		e=db.creatures[id];
		if( (e.flags&ISACTIVE) && (e.pos.z==p.z) && (abs(p.x-e.pos.x)<=1) && (abs(p.y-e.pos.y)<=1))
		{
			return 1;
		}
	}
	return 0;
}

int o_adjacent(coord p)
{
	Entity e;
	for(int id=1; id<DBSIZE_OBJECTS; id++)
	{
		e=db.objects[id];
		if( (e.flags&ISACTIVE) && (e.pos.z==p.z) && (abs(p.x-e.pos.x)<=1) && (abs(p.y-e.pos.y)<=1)
				&& objat(e.pos.x,e.pos.y))
		{
			return 1;
		}
	}
	return 0;
}




int do_autopilot()
{
	int status=RETURN_UNDEF;
	int next=player->pos.y*XMAX+player->pos.x;
	int direction=nodir;

	coord p=(coord){next%XMAX, next/XMAX, player->pos.z};
	autopilot.c_inroom=c_inroom(p);
	autopilot.c_adjacent=c_adjacent(p);
	autopilot.o_adjacent=o_adjacent(p);

	if(autopilot.mode==FOLLOW || autopilot.mode==EXPLORE)
	{
		next=dijk_getpath(player, autopilot.map);
	}
	coord pnext=(coord){next%XMAX, next/XMAX, player->pos.z};
	direction=getdirection(player->pos, pnext);

	int _c_inroom=c_inroom(pnext);
	int _c_adjacent=c_adjacent(pnext);
	int _o_adjacent=o_adjacent(pnext);


	int stop=0;
	switch(autopilot.mode)
	{
		case REST:
			if(autopilot.c_adjacent||autopilot.c_inroom)
			{
				msg("you are woken up");
				stop=1;
			}
			break;

		case FOLLOW:
			if(autopilot.c_adjacent || autopilot.o_adjacent || obstructs(pnext.x, pnext.y)) stop=1;
			break;

		case EXPLORE:
			if(autopilot.c_adjacent || autopilot.o_adjacent ||
			   autopilot.c_inroom || obstructs(pnext.x, pnext.y)) stop=1;
			break;


		default:
			if(autopilot.c_adjacent) stop=1;
			break;
	}

	if(stop)
	{
		stop_autopilot();
		status=RETURN_SUCCESS;
		display();
	}
	else
	{
		if(direction!=nodir)
		{
			walk(player, direction);
		}
	}

	return status;
}




int start_autopilot()
{
	memset(&autopilot,0,sizeof(struct _autopilot));

	autopilot.active=1;
	autopilot.mode=NONE;
	
	// initial states
	coord p=player->pos;
	autopilot.c_inroom=c_inroom(p);
	autopilot.c_adjacent=c_adjacent(p);
	autopilot.o_adjacent=o_adjacent(p);

	nav_node *tmp=dijk_new();
	memcpy(autopilot.map,tmp,sizeof(autopilot.map));
	free(tmp);

	return RETURN_SUCCESS;
}

void stop_autopilot()
{
	autopilot.active=0;
}

/*
int start_autopilot(int direction)
{
	int status=RETURN_UNDEF;
	//int direction=pick_direction();
	int target=get_first_thing_direction(player->pos, direction);
	autopilot.target=target;
	autopilot.active=true;
	
	status=do_autopilot();


	//if(direction!=nodir)
	//{
	//	autopilot.active=true;
	//	autopilot.target=-1;
	//	autopilot.ignore=0;
	//	autopilot.direction=direction;
	//	status=RETURN_SUCCESS;
	//}
	return status;
}

void stop_autopilot()
{
	memset(&autopilot, 0, sizeof(struct _autopilot));
	autopilot.target=-1;
	display();
	//autopilot.target=-1;
	//autopilot.active=false;
	//autopilot.ignore=0;
	//autopilot.rest=false;
	//autopilot.direction=nodir;
}

int do_autopilot()
{
	int status=RETURN_UNDEF;
	int dx1=-1,dx2=1,dy1=-1,dy2=1; //search area for tiles of interest
	coord p=player->pos;

	// SPAGHETTI
	if(autopilot.target >=0)
	{
		if(followpath()==RETURN_SUCCESS) return RETURN_SUCCESS;
	}


	switch(autopilot.direction)
	{
		case north: p.y--; dy2=0;break;
		case south: p.y++; dy1=0;break;
		case east:  p.x++; dx1=0;break;
		case west:  p.x--; dx2=0;break;
		case northeast: p.y--;p.x++; dy2=0; dx1=0; break;
		case northwest: p.y--;p.x--; dy2=0; dx2=0; break;
		case southeast: p.y++;p.x++; dy1=0; dx1=0; break;
		case southwest: p.y++;p.x--; dy1=0; dx2=0; break;
		default: break;
	}
	if(obstructs(p.x,p.y)) // Going to hit wall
	{
		stop_autopilot();
		status=RETURN_FAIL;
	}
	else if(autopilot.rest)
	{
		int end=0;
		Entity **lst;

		lst=get_target_visible();
		if(lst && lst[0])
		{
			msg("%s enters the room",getname(lst[0]));
			end=1;
		}
		free(lst);

		lst=get_target_adjacent(player);
		if(lst && lst[0])
		{
			msg("your rest is interupted by %s",getname(lst[0]));
			end=1;
		}
		free(lst);

		if(player->_c.stat.hp>=player->_c.stat.maxhp)
		{
			msg("you have recovered");
			end=1;
		}

		if(end)
		{
			stop_autopilot();
			display(); //HACK
			status=RETURN_FAIL;
		}
	}
	else if(walk(player, autopilot.direction)==RETURN_STATUSA) // The walk succeeds but there might be something of interest
	{
		status=RETURN_SUCCESS;
		p=player->pos;

		for(int dx=dx1; dx<=dx2; dx++)
		for(int dy=dy1; dy<=dy2; dy++)
		{
			// It will always search for creatures but
			// you can turn off the search for items and
			// doors etc.

			if(moat(p.x+dx,p.y+dy)&&moat(p.x+dx,p.y+dy)!=player)
			{
				stop_autopilot();//autopilot.active=false;
				status=RETURN_SUCCESS;
			}
			if( !autopilot.ignore && (
				tileat(p.x+dx,p.y+dy)->c==DOOR||
				tileat(p.x+dx,p.y+dy)->c==DOWNSTAIRS ||
				tileat(p.x+dx,p.y+dy)->c==UPSTAIRS ||
				objat(p.x+dx, p.y+dy) ||

				(dy && (autopilot.direction==east||autopilot.direction==west)&&(tileat(p.x,p.y+dy)->c==PASSAGE)) ||// next to a passage?
				(dx && (autopilot.direction==north||autopilot.direction==south)&&(tileat(p.x+dx,p.y)->c==PASSAGE)) // next to a passage?
				))
			{
				stop_autopilot();//autopilot.active=false;
				status=RETURN_SUCCESS;
			}
		}
	}
	else // the walk failed
	{
		stop_autopilot();
		status=RETURN_FAIL;
		display();
	}
		platform_sleep(ANIM_RATE);

	return status;
}
		

int followpath()
{
	coord next;
	int target=autopilot.target;
	int status=RETURN_UNDEF;
	if(target>=0 && target<(XMAX*YMAX) && //(db.levels[db.cur_level].tile_flags[target]&MS_EXPLORED) &&
		(player->pos.y*XMAX+player->pos.x)!=target)
	{
		autoroute=dijk_new();
		dijk_addsrc(autoroute, autopilot.target, 0);
		int ii=dijk_getpath(player, autoroute);
		next=(coord){ii%XMAX, ii/XMAX, db.cur_level};
		autopilot.direction=getdirection( player->pos, next);
		free(autoroute);


		Entity **lst=get_target_adjacent(player);
		if(lst && lst[0]) stop_autopilot();
		free(lst);

	}
	else
	{	
		stop_autopilot();
		status=RETURN_SUCCESS;
	}
	return status;
}

int dsort(const void *a, const void *b)
{
	return ((nav_node *)a)->weight - ((nav_node *)b)->weight;
}

int autoexplore()
{
	int next=-1;
	//nav_node *ts=dijk_new();

	for(int i=0; i<db.nrooms; i++)
	{
		room r=db.rooms[i];
		for(int x=r.x; x<(r.x+r.w); x++)
		{
			for(int y=r.y; y<(r.y+r.h); y++)
			{
				if(!(tflags(x,y)&MS_EXPLORED))
				{
					autoroute[y*XMAX+x].weight=0;
				}
			}
		}
	}

	dijk_scan(autoroute);
	next=dijk_getpath(player,autoroute);

	if(next)
	{
		autopilot.target=next;
		autopilot.active=true;
	}
	//free(ts);
	return 0;
}
*/

int automouse(int x, int y)
{
	int status=RETURN_UNDEF;
	if(x>=0 && x<XMAX && y>=0 && y<YMAX && (tflags(x,y)&MS_EXPLORED))
	{
		start_autopilot();
		autopilot.mode=FOLLOW;
		dijk_addsrc(autopilot.map, (y*XMAX+x), 0);
		status=RETURN_SUCCESS;
	}
	else msg("invalid target");


	return status;
}
int autorest()
{
	int status=start_autopilot();
	int failed=0;
	autopilot.mode=REST;
	if(autopilot.c_inroom || autopilot.c_adjacent)
	{
		msg("you can't rest, there are creatures nearby");
		failed=1;
	}

	if(failed)
	{
		stop_autopilot();
		status=RETURN_FAIL;
	}
	return status;
}
int autoexplore()
{
	int status=start_autopilot();
	int count=0;
	autopilot.mode=EXPLORE;

	if(autopilot.c_adjacent || autopilot.c_inroom)
	{
		stop_autopilot();
		msg("you can't explore while there are creatures nearby");
		return RETURN_FAIL;
	}


	for(int i=0; i<db.nrooms; i++)
	{
		room r=db.rooms[i];
		for(int x=r.x; x<(r.x+r.w); x++)
		{
			for(int y=r.y; y<(r.y+r.h); y++)
			{
				if(!(tflags(x,y)&MS_EXPLORED))
				{
					autopilot.map[y*XMAX+x].weight=0;
					count++;

					if(tileat(x,y)->obj) autopilot.map[y*XMAX+x].weight--;
				}
			}
		}
	}

	if(count) dijk_scan(autopilot.map);
	else
	{
		stop_autopilot();
		status=RETURN_FAIL;
		msg("floor fully explored");
	}
	return status;
}
int autodirection(int direction)
{
	direction++;
	return RETURN_UNDEF;
}
