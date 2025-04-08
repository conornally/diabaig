#include "diabaig.h"

struct _autopilot autopilot;

int start_autopilot()
{
	int status=RETURN_UNDEF;
	int direction=pick_direction();
	if(direction)
	{
		autopilot.active=true;
		autopilot.direction=direction;
		status=RETURN_SUCCESS;
	}
	return status;
}

int do_autopilot()
{
	int status=RETURN_UNDEF;
	int dx1=-1,dx2=1,dy1=-1,dy2=1; //search area for tiles of interest
	coord p=player->pos;

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
		autopilot.active=false;
		status=RETURN_SUCCESS;
	}
	else if(walk(player, autopilot.direction)==RETURN_STATUSA) // The walk succeeds but there might be something of interest
	{
		p=player->pos;

		for(int dx=dx1; dx<=dx2; dx++)
		for(int dy=dy1; dy<=dy2; dy++)
		{
			if( tileat(p.x+dx,p.y+dy)->c==DOOR||
				tileat(p.x+dx,p.y+dy)->c==DOWNSTAIRS ||
				tileat(p.x+dx,p.y+dy)->c==UPSTAIRS ||
				objat(p.x+dx, p.y+dy) || (moat(p.x+dx,p.y+dy)&&moat(p.x+dx,p.y+dy)!=player) ||

				(dy && (autopilot.direction==east||autopilot.direction==west)&&(tileat(p.x,p.y+dy)->c==PASSAGE)) ||// next to a passage?
				(dx && (autopilot.direction==north||autopilot.direction==south)&&(tileat(p.x+dx,p.y)->c==PASSAGE)) // next to a passage?
				)
			{
				autopilot.active=false;
				status=RETURN_SUCCESS;
			}
		}
	}
	else // the walk failed
	{
		autopilot.active=false;
		status=RETURN_SUCCESS;
	}


	/*

	if(walk(player,autopilot.direction)!=RETURN_STATUSA || //failed walk
			tileat(player->pos.x,player->pos.y)->c==DOOR)  //reached door
	{
		autopilot.active=false;
		status=RETURN_SUCCESS;
	}
	*/
	return status;
}
		

