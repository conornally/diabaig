#include "diabaig.h"

//static void _start_slowhunger(Entity *ring);
static void _start_incmaxhp(Entity *ring);
//static void _start_berserker(Entity *ring);
static void _start_criticaleye(Entity *ring);
static void _start_regeneration(Entity *ring);
static void _start_resist(Entity *ring);
static void _start_mighty(Entity *ring);
static void _start_steadfast(Entity *ring);

static void _stop_incmaxhp(Entity *ring);
//static void _stop_berserker(Entity *ring);
static void _stop_criticaleye(Entity *ring);
static void _stop_regeneration(Entity *ring);
static void _stop_resist(Entity *ring);
static void _stop_mighty(Entity *ring);
static void _stop_steadfast(Entity *ring);

int has_ring(int which)
{
	int wearing=0;
	if(db.cur_ringR!=R_NORING && db.objects[db.cur_ringR]._o.which==which) wearing+=1;
	if(db.cur_ringL!=R_NORING && db.objects[db.cur_ringL]._o.which==which) wearing+=1;
	return wearing;
}

int _do_equip_ring(Entity *e)
{
	int status=RETURN_UNDEF;
	if(e && e->_o.type==RING)
	{
		ring_info[e->_o.which].known=true;
		switch(e->_o.which)
		{
			//case R_SLOWHUNGER: _start_slowhunger(e); break;
			case R_VITALITY: _start_incmaxhp(e); break;
			//case R_BERSERKER: _start_berserker(e);break;
			case R_CRITICALEYE: _start_criticaleye(e);break;
			case R_REGEN: _start_regeneration(e);break;
			case R_FIRERESIST:
			case R_FROSTRESIST:
			case R_POISONRESIST: 
			case R_WAKING: _start_resist(e); break;
			case R_STRENGTH: _start_mighty(e); break;
			case R_DEFENCE: _start_steadfast(e); break;
		}
	}
	else status=RETURN_FAIL;
	return status;
}

int _do_unequip_ring(Entity *e)
{
	int status=RETURN_UNDEF;
	if(e && e->_o.type==RING)
	{
		switch(e->_o.which)
		{
			//case R_SLOWHUNGER: _start_stophunger(e); break;
			case R_VITALITY: _stop_incmaxhp(e); break;
			//case R_BERSERKER: _stop_berserker(e);break;
			case R_CRITICALEYE: _stop_criticaleye(e);break;
			case R_REGEN: _stop_regeneration(e);break;
			case R_FIRERESIST:
			case R_FROSTRESIST:
			case R_POISONRESIST: 
			case R_WAKING: _stop_resist(e); break;
			case R_STRENGTH: _stop_mighty(e); break;
			case R_DEFENCE: _stop_steadfast(e); break;
		}
	}
	else status=RETURN_FAIL;
	return status;
}

//static void _start_slowhunger(Entity *ring)
//{
	// the code for this is elsewhere in update_player
//}

static void _start_incmaxhp(Entity *ring)
{
	if(ring) player->_c.stat.maxhp+=10;
}
/*
static void _start_berserker(Entity *ring)
{
	//NEED TO MAKE SURE LEVELLING UP IS BALANCD WHILE WEARING THIS
	player->_c.stat.maxhp/=2;
	player->_c.stat.hp= MIN(player->_c.stat.hp, player->_c.stat.maxhp);
	player->_c.stat.str[0]*=2;
}
*/
static void _start_criticaleye(Entity *ring)
{
	//NEED TO MAKE SURE LEVELLING UP IS BALANCD WHILE WEARING THIS
	if(ring) player->_c.stat.dex+=5;
}
static void _start_regeneration(Entity *ring)
{
	if(ring) add_daemon(player,D_REGEN,REGENDURATION);
}

static void _start_resist(Entity *ring)
{
	if(ring)
	{
		switch(ring->_o.which)
		{
			case R_FIRERESIST: 
				player->_c.res_flags |= RESIST_FIRE; 
				if(has_ring(R_FIRERESIST)) player->_c.res_flags |= IMMUNE_FIRE; 
				break;
			case R_FROSTRESIST: 
				player->_c.res_flags |= RESIST_FROST; 
				if(has_ring(R_FROSTRESIST)) player->_c.res_flags |= IMMUNE_FROST; 
				break;
			case R_POISONRESIST: 
				player->_c.res_flags |= RESIST_POISON;
				if(has_ring(R_POISONRESIST)) player->_c.res_flags |= IMMUNE_POISON; 
			   	break;
			case R_WAKING:player->_c.res_flags |= IMMUNE_SLEEP; break;
		}
	}
}

static void _start_mighty(Entity *ring)
{
	if(ring) player->_c.stat.str[1]++;
}

static void _start_steadfast(Entity *ring)
{
	if(ring) player->_c.stat.def+=5;
}



static void _stop_incmaxhp(Entity *ring)
{
	if(ring)
	{
		player->_c.stat.maxhp-=10;
		player->_c.stat.hp=MIN(player->_c.stat.hp,player->_c.stat.maxhp);
	}
}
/*
static void _stop_berserker(Entity *ring)
{
	player->_c.stat.maxhp*=2;
	player->_c.stat.str[0]/=2;
}
*/
static void _stop_criticaleye(Entity *ring)
{
	if(ring) player->_c.stat.dex-=5;
}
static void _stop_regeneration(Entity *ring)
{
	if(ring)
	{
		for(_daemon *d=db.daemons; d<&db.daemons[NDAEMONS]; d++)
		{
			if( (d->c_id==0) && (d->type==D_REGEN))
			{
				stop_daemon(d);
				break;
			}
		}
	}
}

static void _stop_resist(Entity *ring)
{
	if(ring)
	{
		switch(ring->_o.which)
		{
			case R_FIRERESIST: 
				if(has_ring(R_FIRERESIST)==2) player->_c.res_flags   &= ~IMMUNE_FIRE; 
				else player->_c.res_flags   &= ~RESIST_FIRE; 
				break;
			case R_FROSTRESIST: 
				if(has_ring(R_FROSTRESIST)==2) player->_c.res_flags   &= ~IMMUNE_FROST; 
				else player->_c.res_flags  &= ~RESIST_FROST; 
				break;
			case R_POISONRESIST: 
				if(has_ring(R_POISONRESIST)==2) player->_c.res_flags   &= ~IMMUNE_POISON; 
				else player->_c.res_flags  &= ~RESIST_POISON; 
				break;
			case R_WAKING: player->_c.res_flags &= ~IMMUNE_SLEEP; break;
		}
	}
}

static void _stop_mighty(Entity *ring)
{
	if(ring) player->_c.stat.str[1]--;
}

static void _stop_steadfast(Entity *ring)
{
	if(ring) player->_c.stat.def-=5;
}
