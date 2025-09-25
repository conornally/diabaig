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
	int stop=0;

	coord p=(coord){next%XMAX, next/XMAX, player->pos.z};
	autopilot.c_inroom=c_inroom(p);
	autopilot.c_adjacent=c_adjacent(p);
	autopilot.o_adjacent=o_adjacent(p);

	if(autopilot.target == (p.y*XMAX+p.x))
	{
		stop=1; 
		autopilot.step++;
	}

	if(autopilot.mode==FOLLOW || autopilot.mode==EXPLORE || autopilot.mode==STRAIGHT)
	{
		next=dijk_getpath(player, autopilot.map);
	}
	coord pnext=(coord){next%XMAX, next/XMAX, player->pos.z};
	direction=getdirection(player->pos, pnext);

	switch(autopilot.mode)
	{
		case REST:
			if(autopilot.c_adjacent||autopilot.c_inroom)
			{
				msg("you are disturbed by the noise of something approaching");
				stop=1;
			}
			else if(player->_c.stat.hp>=player->_c.stat.maxhp)
			{
				msg("you feel fully rested");
				stop=1;
			}
			break;

		case STRAIGHT:
			for(int dx=-1; dx<=1; dx++)
			for(int dy=-1; dy<=1; dy++)
			{
				// Stop next to tiles of interest
				// or junctions in corridors
				if( tileat(p.x+dx,p.y+dy)->c==DOOR||
					tileat(p.x+dx,p.y+dy)->c==DOWNSTAIRS ||
					tileat(p.x+dx,p.y+dy)->c==UPSTAIRS ||
					(dy && (direction==east|| direction==west) &&(tileat(p.x,p.y+dy)->c==PASSAGE)) ||
					(dx && (direction==north||direction==south)&&(tileat(p.x+dx,p.y)->c==PASSAGE)) 
					) stop=1;
			}
			if(autopilot.c_adjacent || autopilot.o_adjacent || obstructs(pnext.x, pnext.y))
				stop=1;
			break;
			
		case FOLLOW: case EXPLORE:
			if(autopilot.c_adjacent || autopilot.o_adjacent || obstructs(pnext.x, pnext.y))
				stop=1;
			break;
		default:
			if(autopilot.c_adjacent) stop=1;
			break;
	}

	if(autopilot.step>=1000) stop=1; //Just a safety measure

	if(stop && autopilot.step)
	{
		stop_autopilot();
		status=RETURN_SUCCESS;
		display();
	}
	else
	{
		if(direction!=nodir) walk(player, direction);
		platform_sleep(ANIM_RATE/2);
	}

	autopilot.step++;
	return status;
}




int start_autopilot()
{
	memset(&autopilot,0,sizeof(struct _autopilot));

	autopilot.active=1;
	autopilot.step=0; //ignore stop alert on first turn
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

int automouse(int x, int y)
{
	int status=RETURN_UNDEF;
	if(x>=0 && x<XMAX && y>=0 && y<YMAX && (tflags(x,y)&MS_EXPLORED) && !obstructs(x,y))
	{
		start_autopilot();
		autopilot.mode=FOLLOW;
		autopilot.target=y*XMAX+x;
		dijk_addsrc(autopilot.map, autopilot.target, 0);
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
	if(player->_c.stat.hp>=player->_c.stat.maxhp)
	{
		msg("you don't need to rest now");
		failed=1;
	}
	else if(autopilot.c_inroom || autopilot.c_adjacent)
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

	if(autopilot.c_adjacent)
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
				for(int dx=-1,dy=-1; (dx<=1)&&(dy<=1); dx++,dy++) //cardinals
				{ //get the walls too
					tile* t=tileat(x+dx,y+dy);
					if( (t->c==VWALL || t->c==HWALL) && !(tflags(x+dx,y+dy)&MS_EXPLORED)) autopilot.map[y*XMAX+x].weight=0;
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
	int status=start_autopilot();
	autopilot.target=get_wall_direction(player->pos, direction);
	autopilot.mode=STRAIGHT;

	dijk_addsrc(autopilot.map, autopilot.target, 0);

	if(autopilot.target==(player->pos.y*XMAX+player->pos.x)) 
	{
		autopilot.active=0;
		status=RETURN_FAIL;
		msg("you can't go this way");
	}

	return status;
}
