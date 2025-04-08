#include "diabaig.h"

void light_room(room *r)
{
	if(r && (r->flags&RM_ISACTIVE) &&  (!(r->flags&RM_ISDARK)||has_ring(R_ELENDIL)||has_wep(TORCH)) && !(player->_c.flags&ISBLIND))
	{
		for(int x=r->x; x<r->x+r->w; x++)
		{
			for(int y=r->y; y<r->y+r->h; y++)
			{
				seen(x,y);
			}
		}
		if(has_ring(R_ELENDIL) && r->flags&RM_ISDARK && !(r->flags&RM_LITBYRING))
		{
			msg("your ring shines with the light of earendil's star");
			r->flags|=RM_LITBYRING;
		}
		if(has_wep(TORCH) && r->flags&RM_ISDARK && !(r->flags&RM_LITBYRING))
		{
			msg("your torch illuminates the dark room");
			r->flags|=RM_LITBYRING;
		}
	}
}

void unlight_room(room *r)
{
	if(r && (r->flags&RM_ISACTIVE))
	{
		for(int x=r->x; x<r->x+r->w; x++)
		for(int y=r->y; y<r->y+r->h; y++)
		{
			tileat(x,y)->flags &= (~ML_VISIBLE);
		}
	}
}

void placeat(Entity *e, int x, int y)
{
	coord p={x,y,db.cur_level};
	if(e && islegal(p))
	{
		e->pos=p;
		if(e->flags & ISCREATURE) moat(x,y)=e;
		else if(e->flags & ISOBJ) objat(x,y)=e;
	}
}

void placeinroom(room *r, Entity *e)
{
	//careful, this will override anything at x,y
	if(r && e)
	{
		int x,y;
		int attempts=0;
		while(1)
		{
			x=r->x+1+rng(r->w-2);
			y=r->y+1+rng(r->h-2);
			if( tileat(x,y)->c!=DOWNSTAIRS && tileat(x,y)->c!=UPSTAIRS && !obstructs(x,y))
			{
				if((e->flags & ISCREATURE) && !moat(x,y)) break;
				if((e->flags & ISOBJ) && !objat(x,y)) break;
			}
			if(attempts++>100) break;
		}
		e->pos.x=x;
		e->pos.y=y;
		e->pos.z=db.cur_level;
		if(e->flags & ISCREATURE)
		{
			moat(x,y)=e;
			e->_c._inroom=r;
			//_log("placing %c %d",e->_c.type, e->_c.form);
		}
		else if(e->flags & ISOBJ) objat(x,y)=e;
		_log("placed %s [%c] (%d,%d) %d",getname(e),(e->flags&ISCREATURE)?'C':'O',x,y, e->id);
	}
}

void seen(int x, int y)
{
	if(x>=0 && x<(XMAX-1) && y>=0 && y<(YMAX-1) )
	{
		tflags(x,y) |= MS_EXPLORED;
		tileat(x,y)->flags |= ML_VISIBLE;
	}
}

room *inroom(Entity *e)
{
	int x=e->pos.x;
	int y=e->pos.y;
	e->_c._inroom=NULL;
	for(room *r=db.rooms; r<&db.rooms[db.nrooms]; r++)
	{
		if( r->x<=x && x<r->x+r->w && r->y<=y && y<r->y+r->h)
		{
			e->_c._inroom=r;
			break;
		}
	}
	return e->_c._inroom;
}

void lock_doors(room *r)
{
	tile *t;
	if(r)
	{
		for(int x=r->x; x<r->x+r->w; x++)
		for(int y=r->y; y<r->y+r->h; y++)
		{
			t=tileat(x,y);
			if(t->c == DOOR) t->flags|=ML_LOCKED;
		}
	}
}

void unlock_doors(room *r)
{
	if(r)
	{
		for(int x=r->x; x<r->x+r->w; x++)
		for(int y=r->y; y<r->y+r->h; y++)
		{
			tileat(x,y)->flags |= ML_LOCKED;
		}
	}
}

void treasureroom(room *r)
{
	if(db.cur_level>0)
	{
		r->flags &= ~RM_ISDARK;
		int nitems=5+rng(5);
		int ncreatures=5+rng(5);
		for(int i=0;i<nitems;i++) placeinroom(r,new_obj());
		for(int i=0;i<ncreatures;i++) spawn_monster_inroom(r);
	}
}

void scholarroom(room *r)
{
	if(db.cur_level>=10)
	{
		r->flags &= ~RM_ISDARK;
		int nitems=5+rng(5);
		int ncreatures=3+rng(3);
		for(int i=0;i<nitems;i++)
		{
			placeinroom(r,_new_obj(SCROLL));
		}
		for(int i=0;i<ncreatures;i++)
		{
			Entity *caster;
			if(!rng(5)) caster=_new_monster('i');
			else
			{
				switch(rng(3))
				{
					case 0: caster=_new_monster('d'); break;
					case 1: caster=_new_monster('w'); break;
					default: caster=_new_monster('g'); break;
				}
			}
			placeinroom(r,caster);
		}
	}
}

void warriorroom(room* r)
{
	int nitems=5+rng(5);
	int ncreatures=5+rng(5);
	int creatures[]={'l','o','u','t'};

	if(db.cur_level>15)
	{
		r->flags &= ~RM_ISDARK;
		for(int i=0; i<ncreatures; i++) placeinroom(r,_new_monster( creatures[rng(4)]) );
		for(int i=0; i<nitems; i++) placeinroom(r,new_obj());
	}
}

void lichlair(room *r)
{
	r->flags &= ~RM_ISDARK;
	placeinroom(r,_new_monster('L'));
	placeinroom(r,_new_monster('n'));
}

void dragonnest(room* r)
{
	int nitems=2+rng(5);
	int ncreatures=2+rng(3);
	int creatures[]={'W','Y'};

	r->flags &= ~RM_ISDARK;
	if(db.cur_level>23)
	{
		for(int i=0; i<nitems; i++) placeinroom(r,_new_obj(GOLD));
		for(int i=0; i<ncreatures; i++) placeinroom(r,_new_monster( creatures[rng(2)]));

		Entity* e=_new_obj(GOLD);
		if(e)
		{
			e->_o.quantity=20+rng(50);
			e->_o.which=1;
			placeinroom(r,e);
		}
	}

}
void primaryboss_room(room* r)
{
	dragonnest(r);
	placeinroom(r,_new_monster('M'));
}

void spawnroom()
{
	room *spawnroom=&db.rooms[rng(db.nrooms)];
	placeinroom(spawnroom, player);
	light_room(spawnroom);

	if(!testarena)
	{
		for(Entity *e=&db.creatures[1]; e<&db.creatures[DBSIZE_CREATURES]; e++)
		{
			//make sure to remove any baddies in the spawn room
			if(e->_c._inroom==spawnroom) clear_entity(e);
		}
		db.xp=0;
	}
	else
	{
		player->_c.stat.maxhp=500;
		player->_c.stat.hp=500;
	}
	add_daemon(player,D_TUTORIAL, 5);
}

void shoproom(room *r)
{
	if(db.cur_level>2 && !(db.levels[db.cur_level].flags&L_SHOP))
	{
		r->flags &= ~RM_ISDARK;
		Entity *raggle=_new_monster('R');

		for(int i=0; i<3+rng(2); i++)
		{
			placeinroom(r, _new_obj(GOLD));
			placeinroom(r, new_obj());
		}
		placeinroom(r,raggle);
		db.levels[db.cur_level].flags|=L_SHOP;
	}

}
