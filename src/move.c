#include "diabaig.h"

char *locked[]={
	"the door is barred",
	"the door won't budge",
	"this door won't open",
	"you can't seem to move the door",
	"this door seems to be locked",
	"you push the door, it doesn't move",
	"you press the door, it won't move",
	"you push the door, it won't open",
	"the door handle won't turn",
	"you seem to be locked on this side of the door",
	0
};

bool islegal(coord p)
{
	bool legal=true;
	if( p.z!=db.cur_level) legal=false;
	else if( p.x<=0 || p.x>=(XMAX-1) || p.y<=0 || p.y>=(YMAX-1)) legal=false;
	else if(obstructs(p.x,p.y)) legal=false;
	//else if(tileat(p.x,p.y)->flags & ML_LOCKED) legal=false;
	return legal;
}

int walk(Entity *e, int direction)
{
	// if walk successful, RETURN_STATUSA
	// if bumped creature, RETURN_STATUSB
	// else RETURN_FAIL
	int status=RETURN_UNDEF;
	coord p;
	if(e)
	{
		if(e->_c.stamina<=0) return RETURN_FAIL;
		p=e->pos;
		if(e->_c.flags & ISCONFUSED) direction=rand()%9;
		switch(direction)
		{
			case north: p.y--; break;
			case south: p.y++; break;
			case east: p.x++; break;
			case west: p.x--; break;

			case northeast: p.y--;p.x++; break;
			case northwest: p.y--;p.x--; break;
			case southeast: p.y++;p.x++; break;
			case southwest: p.y++;p.x--; break;
			default: break;
		}
		if(islegal(p))// && !(tileat(x,y)&ML_LOCKED))
		{

			//check door
			if(tileat(p.x,p.y)->flags&ML_LOCKED) player_msg(e,locked[ rng(clen(locked)) ]);


			//do melee attack
			else if(melee(e,moat(p.x,p.y))!=RETURN_SUCCESS)
			{

			//if melee failed, do move
				if(e->_c.flags&ISBOUND) 
				{
					status=RETURN_FAIL;
					if(e==player) msg("you are bound and can't move, try something else");
				}
				//else if(stumble(e) && (tileat(p.x,p.y)->flags&ML_VISIBLE))
				//else if((tileat(p.x,p.y)->flags&ML_VISIBLE))
				else if(stumble(e) && lineofsight(e,player))
				{
					char * lst[]={
						"%s stumbles",
						"%s stumbles on a crack in the floor",
						"%s stumbles on a gap in the flagstones",
						"%s falters",
						"%s trips",
						"%s trips and staggers",
						"%s trips on a gap in the flagstones",
						"%s falters in the poor light",
						"%s flounders in the low visibility",
						0,
					};
					//if(tileat(e->pos.x,e->pos.y)->flags&ML_VISIBLE)
					msg( lst[ rng(clen(lst))], getname(e));
					status=RETURN_STATUSB; //this is a successful return for a failed movemnet
				}
				else
				{
					tileat(e->pos.x, e->pos.y)->creature=NULL;
					e->pos=p;
					tileat(p.x, p.y)->creature=e;
					e->_c.stamina--;
					if(e==player)
					{
						pickup();
					}
					status=RETURN_STATUSA;
				}
			}
			else status=RETURN_STATUSB;
		}
		else if(e==player && obstructs(p.x, p.y))
		{
			char *lst[]={
				"you can't walk that way",
				"there is a wall there",
				"you can't go that way",
				"you run into a wall",
				"the wall obstructs your way",
				"who put that wall there",
				"you walk into the wall in the poor light",
				"you run at the wall",
				"you run at the wall, it does not give way",
				"you run at the wall, it is solid",
				"you try to walk walk into the wall, its made of stone",
				"you try to walk walk into the wall, its made of rough stone",
				"the wall does not budge, despite your best efforts",
				"the wall does not budge, despite your attempts",
				"the wall does not budge, despite your enthusiasm",
				"you run into the wall, don't worry, nobody saw",
				"you run into the wall, don't worry, probably nobody saw",
				"you run into the wall, don't worry, only the bats were watching",
				"you walk at the wall, fortunately, only the bats were watching",
				"you walk at the wall, it is wet",
				"you walk at the wall, it is mossy",
				"you walk at the wall, it is dirty",
				"you walk at the wall, it is solid",
				"this wall is solid",
				"this wall is made of stone",
				"this wall is made of solid stone",
				"this wall is made of rough stone",
				"the path would be much shorter if you could go that way, but you can't",
				"the way is blocked that direction",


				0};
			msg(lst[rng(clen(lst))]);
		}

	}
	else status=RETURN_FAIL;
	return status;
}

int descend(Entity *e)
{
	int status=RETURN_UNDEF;
	if(db.cur_level<(NLEVELS-1) && tileat(e->pos.x,e->pos.y)->c==DOWNSTAIRS)
	{
		db.cur_level++;
		e->pos.z++;
		construct_level(&db.levels[db.cur_level]);

		moat(e->pos.x, e->pos.y)=NULL;
		e->pos.x=db.levels[db.cur_level].upstair%XMAX;
		e->pos.y=db.levels[db.cur_level].upstair/XMAX;
		db.tiles[db.levels[db.cur_level].upstair].creature=e;

		//if( !((db.cur_level+1)%5) ) 
		autosave();

		status=RETURN_SUCCESS;
	}
	else
	{
		status=RETURN_FAIL;
		player_msg(e, "you can't descend here");
	}
	return status;
}
int ascend(Entity *e)
{
	int status=RETURN_UNDEF;
	if(tileat(e->pos.x,e->pos.y)->c==UPSTAIRS)
	{
		if(db.cur_level>0) 
		{
			//autosave leaving mod 5 (leaving dragon)
			//if( !((db.cur_level+1)%5) ) 
			autosave();

			db.cur_level--;
			e->pos.z--;
			construct_level(&db.levels[db.cur_level]);

			moat(e->pos.x, e->pos.y)=NULL;
			e->pos.x=db.levels[db.cur_level].downstair%XMAX;
			e->pos.y=db.levels[db.cur_level].downstair/XMAX;
			db.tiles[db.levels[db.cur_level].downstair].creature=e;
			status=RETURN_SUCCESS;
		}
		else if(db.cur_level==0 && checkfortooth())
		{
			_log("player ascended!");
			game_won=1;
			set_ripdata(RIP_WIN,dragonname);
			game_won=1;
			running=0;
			status=RETURN_SUCCESS;
		}
		else msg("you cannot return now, you have not completed your quest");

	}
	else
	{
		status=RETURN_FAIL;
		player_msg(e, "you can't ascend here");
	}
	return status;
}

int stumble(Entity *e)
{
	int _stumble=false;
	int chance= (e->_c.stat.dex * e->_c.stat.dex)/2;
	if(e->_c.flags & ISFLY)  return false;
	if(e->flags & ISILLUSION) return false;
	
	if(e==player) chance*=10; //player just less likely to stumble
	if(!rng(chance)) 
	{
		_stumble=true;
	}

	return _stumble;
}
