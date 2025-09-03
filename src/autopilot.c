#include "diabaig.h"

nav_node *autoroute;
struct _autopilot autopilot;
static int followpath();

int start_autopilot()
{
	int status=RETURN_UNDEF;
	int direction=pick_direction();
	if(direction)
	{
		autopilot.active=true;
		autopilot.target=-1;
		autopilot.ignore=0;
		autopilot.direction=direction;
		status=RETURN_SUCCESS;
	}
	return status;
}

void stop_autopilot()
{
	autopilot.target=-1;
	autopilot.active=false;
	autopilot.ignore=0;
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
		status=RETURN_SUCCESS;
	}
	else if(walk(player, autopilot.direction)==RETURN_STATUSA) // The walk succeeds but there might be something of interest
	{
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
		status=RETURN_SUCCESS;
	}

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

		platform_sleep(ANIM_RATE);

		Entity **lst=get_target_adjacent(player);
		if(lst[0]!=NULL) stop_autopilot();
		free(lst);

	}
	else
	{	
		stop_autopilot();
		status=RETURN_SUCCESS;
	}
	return status;
}

