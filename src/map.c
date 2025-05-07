#include "diabaig.h"

#define isleaf(r) 	  ( !( (r)->a || (r)->b) )

struct _region
{
	int x,y,w,h;
	struct _region *a,*b;
	int cx,cy;
};
typedef struct _region region;
static int map[XMAX*YMAX];

static void placeroom(room r, int index);
static void connect_vertical(int x1, int y1, int y2);
static void connect_horizont(int x1, int x2, int y1);
static void _region_split(int n, region *r);
static void _region_genroom(region *r);
static void _region_genpassage(region *r);
static void _region_free(region *r);

static void post_process();
static void produce_map();
static void place_upstair(room *r, level *l);
static void place_dnstair(room *r, level *l);

static void build_level_TESTARENA();
static void build_level_BSP();
static void build_level_Dragon();

//static void _place_rand_guardian(room *r);

int get_efflevel()
{
	int lvl=db.cur_level;
	if(!getdragon())
	{
		lvl=2*SECONDARYBOSS- db.cur_level;
	}
	return lvl;
}

void construct_level(level *lvl)
{
	wclear(win);
	wrefresh(win);
	srand(lvl->seed);
	_log("building level %d",db.cur_level);
	memset(db.tiles,'\0', sizeof(db.tiles));
	memset(db.rooms,'\0', sizeof(db.rooms));
	for(int i=0;i<XMAX*YMAX;i++)map[i]=STONE;

	regen_shop(MAXSHOP-rng(2));
	//db.levels[db.cur_level].flags|=L_SHOP;

	if(testarena)
	{
		build_level_TESTARENA();
		post_process();
		produce_map();

        //shoproom(&db.rooms[0]);
		//db.gold+=1000;
	}
	else if(db.cur_level==SECONDARYBOSS)
	{
		build_level_Dragon();
		post_process();

		int weights[3]={1,1,1};

		int up,rag,xlt;
		weights[(up =weighted_pick(weights,3))]=0;
		weights[(rag=weighted_pick(weights,3))]=0;
		weights[(xlt=weighted_pick(weights,3))]=0;

		_log("NROOMS %d",db.nrooms);


		//room *uproom=&db.rooms[up+7];
		shoproom(&db.rooms[rag+7]);
		place_upstair(&db.rooms[up+7], lvl);
		placeinroom(&db.rooms[xlt+7], _new_monster('X'));

		produce_map();

		if(!(db.levels[db.cur_level].flags&L_VISITED)) // The way down
		{
			Entity* dragon=getdragon();
			add_daemon(dragon, D_SLEEP, 100+rng(200));
			dragon->_c.flags|=ISSLEEP;
			placeinroom(&db.rooms[0], dragon);

			int gold=0;
			int RADIUS=7;
			Entity *item=NULL;

			while(gold<20)
			{
				int x=rng(2*RADIUS)-RADIUS+dragon->pos.x;
				int y=rng(2*RADIUS)-RADIUS+dragon->pos.y;
				float dd= pow(x-dragon->pos.x,2)+pow(y-dragon->pos.y,2);

				// This is like a pseudo gaussian, it is more likely to land nearby
				if(!obstructs(x,y) && !rng(sqrt(dd)))
				{
					item=_new_obj(GOLD);
					item->pos=(coord){x,y,db.cur_level};
					item->_o.quantity*=2;
					if(objat(x,y) && objat(x,y)->_o.type==GOLD)
					{
						objat(x,y)->_o.quantity+=item->_o.quantity;
						clear_entity(item);
					}
					else objat(x,y)=item;
					gold++;
				}
			}

			for(int i=0; i<2; i++)
			{
				int x=34;
				int y=11+(i*6);
				Entity *e=_new_monster('G');
				e->pos=(coord){x,y,db.cur_level};
				moat(x,y)=e;
				e->_c.flags&=~ISAGRO;
			}
			display_dathead(res_dragon_txt, res_dragon_txt_len);
			wmove(win,2,3);
			wprintw(win,"You are entering the lair of %s the %s",dragonname, dragon_mod);
			wmove(win,YMAX+2, 4+XMAX/2);
			wprintw(win,"press SPACE to proceed carefully");
			wrefresh(win);
			while(wgetch(win)!=' ');
		}
	}
	else
	{
		build_level_BSP();

		post_process();
		int up_i=rng(db.nrooms);
		int dn_i=rng(db.nrooms);
		room *uproom=&db.rooms[up_i];
		room *dnroom=&db.rooms[dn_i];
		if(db.cur_level>0 || db.defeated_dragon) place_upstair(uproom, lvl);
		if(db.cur_level<NLEVELS) place_dnstair(dnroom, lvl);

		produce_map();

		for(room *r=db.rooms; r<&db.rooms[db.nrooms]; r++)
		{
			if(lvl->flags&L_DARK) r->flags |= RM_ISDARK;
			if(rng(100)<db.cur_level) r->flags |= RM_ISDARK;

			if(!(db.levels[db.cur_level].flags&L_VISITED)) // The way down
			{

				if(rng(6)) spawn_monster_inroom(r);
				if(rng(3)) placeinroom(r,new_obj());
				if(r!=uproom)
				{
					if(!rng(80)) treasureroom(r);
					else if(!rng(80)) scholarroom(r);
					else if(!rng(80)) warriorroom(r);
					else if(!rng(80)) dragonnest(r);
					else if(!rng(70)) shoproom(r);
				}
			}
			/// SOMETHING IS WRONG HERE
			else if( db.defeated_dragon && !(db.levels[db.cur_level].flags&L_RETURN) )// The way up
			{
				if(rng(12)) spawn_monster_inroom(r); // increase creatures
				if(rng(2)) placeinroom(r,new_obj()); // fewer items
				
				//no check on uproom or downroom?
				
				if(!rng(80))  treasureroom(r);
				else if(!rng(80)) dragonnest(r);
			}
		}

		//primary boss on descent
		if( (get_efflevel()==PRIMARYBOSS) && !(db.levels[db.cur_level].flags&L_VISITED))
		{
			_log("Mother of Dragons placed");
			int i;
			do{ i=rng(db.nrooms);}while(i==up_i);
			primaryboss_room( &db.rooms[i] );
		}

		if( (get_efflevel()==XOLOTLLAYER) && !(db.levels[db.cur_level].flags&L_VISITED))
		{
			placeinroom( &db.rooms[ rng(db.nrooms) ], _new_monster('X'));
		}
		/*
		// APEX creature
		if(!(db.levels[db.cur_level].flags&L_RETURN) && !rng(APEXRATE))
		{
			int types[]={'B','C','L','S',0};
			Entity *apex=_new_monster(types[rng(clen(types))]);
			placeinroom( &db.rooms[rng(db.nrooms)], apex);
			msg("%s has made its way into diabaig",getname(apex));
			db.levels[db.cur_level].flags|=L_APEXED;
		}
		*/

	}

	init_pathfinding();
	db.levels[db.cur_level].flags|=L_VISITED;
	if(db.defeated_dragon) db.levels[db.cur_level].flags|=L_RETURN;

}

void build_level_TESTARENA()
{
	db.nrooms=0;
	//room r={1,1,10,10};//XMAX-2,YMAX-2};
	room r={1,1,XMAX-2,YMAX-2, 0};
	placeroom(r,db.nrooms++);
	//put mobs and items in here if needed
	for(int i=0;i<0;i++)
	{
		Entity *e=_new_monster('v');
		placeinroom(&db.rooms[0],e);
	}
	for(int i=0; i<1; i++)
	{
		_log("arena placing:%d",i);
		Entity *e=_new_obj(TRINKET);

		//if(!i)_set_weapon(e,BOW);
		//else _set_weapon(e,ARROW);
		placeinroom(&db.rooms[0],e);
		//e=_new_obj(WEAPON);
		//_set_weapon(e,SPEAR);
		//placeinroom(&db.rooms[0],e);
	}
}

void build_level_Dragon()
{
	db.nrooms=0;

	room rr;
	room r={40,9,31,11,0};
	placeroom(r,db.nrooms++);

	for(int i=0; i<3; i++)
	{
		rr=(room){2+r.x+(10*i), r.y-6,7,5,0};
		placeroom(rr,db.nrooms++);
		connect_vertical( rr.x+3, rr.y+1, r.y+1);
	}
	for(int i=0; i<3; i++)
	{
		rr=(room){2+r.x+(10*i), r.y+r.h+1,7,5,0};
		placeroom(rr,db.nrooms++);
		connect_vertical( rr.x+3, rr.y+1, r.y+2);
	}

	for(int i=-1; i<2; i++)
	{
		rr=(room){5, r.y+(r.h/2)+(i*8)-2 , 9, 5,0};
		placeroom(rr,db.nrooms++);
	}
	connect_vertical(rr.x+4, 5,25);

	rr=(room){25,10,10,9,0};
	placeroom(rr,db.nrooms++);
	connect_horizont(rr.x+1, r.x+1, rr.y+1);
	connect_horizont(rr.x+1, r.x+1, rr.y+rr.h-2);
	connect_horizont(rr.x+1, 10, rr.y+rr.h/2);

	// Central Block
	rr=(room){r.x+r.w/2-5,r.y+r.h/2-1,12,3,0};
	placeroom(rr,db.nrooms++);
	for(int x=1; x<rr.w-1; x++) map[ (rr.y+1)*XMAX+x+rr.x]=STONE;
}
bool rooms_overlap(room a, room b)
{
	bool overlap=false;
	if(a.x<=(b.x+b.w) && b.x<=(a.x+a.w) &&
	   a.y<=(b.y+b.h) && b.y<=(a.y+a.h) ) overlap=true;
	return overlap;
}

static void placeroom(room r, int index)
{
	r.flags|=RM_ISACTIVE;
	if(index>=0 && index<NROOMS)
	{
		db.rooms[index]=r;
		for(int x=r.x; x<r.x+r.w; x++)
		for(int y=r.y; y<r.y+r.h; y++)
		{
			map[y*XMAX+x]=FLOOR;
		}
		for(int y=r.y;y<r.y+r.h; y++)
		{
			map[y*XMAX+r.x]=VWALL;
			map[y*XMAX+r.x+r.w-1]=VWALL;
		}
		for(int x=r.x;x<r.x+r.w; x++)
		{
			map[(r.y)*XMAX+x]=HWALL;
			map[(r.y+r.h-1)*XMAX+x]=HWALL;
		}
	}
}

static void build_level_BSP()
{
	db.nrooms=0;
	region head={1,1,XMAX-2,YMAX-2,NULL,NULL, XMAX/2+rng(2)-1, YMAX/2+rng(2)-1};
	_region_split(4,&head);
	_region_genroom(&head);
	_region_genpassage(&head);
	_region_free(head.a);
	_region_free(head.b);
}

static void _region_split(int n, region *r)
{
	// dw=  w/2 +/- w/10
	bool _split=false;
	region a,b;
	int dw= (int)(r->w/2)*round( 1.0 + (rng(10)-5)/15.0 );
	int dh= (int)(r->h/2)*round( 1.0 + (rng(10)-5)/15.0 );
	if(n>0)
	{
		if(rng(2) && (dw>=R_XMIN && (r->w-dw)>=R_XMIN)) //vertical split
		{
			a=(region){r->x, 	r->y, dw, 	   r->h, NULL, NULL, 0, r->cy};
			b=(region){r->x+dw, r->y, r->w-dw, r->h, NULL, NULL, 0, r->cy};
			a.cx=a.x+a.w/2+rng(2)-1;
			b.cx=b.x+b.w/2+rng(2)-1;
			_split=true;
		}
		else if(dh>=R_YMIN && (r->h-dh)>=R_YMIN) //horizontal split
		{
			a=(region){r->x, r->y,    r->w, dh, 	 NULL, NULL, r->cx, 0};
			b=(region){r->x, r->y+dh, r->w, r->h-dh, NULL, NULL, r->cx, 0};
			a.cy=a.y+a.h/2+rng(2)-1;
			b.cy=b.y+b.h/2+rng(2)-1;
			_split=true;
		}
		if(_split)
		{
			r->a=malloc(sizeof(region));
			r->b=malloc(sizeof(region));
			memcpy(r->a,&a,sizeof(region));
			memcpy(r->b,&b,sizeof(region));
			_region_split(n-1,r->a);
			_region_split(n-1,r->b);
		}
		else _region_split(n-1,r); //try again?
	}
}

static void _region_genroom(region *r)
{
	int _min,_max;
	if(r->a) _region_genroom(r->a);
	if(r->b) _region_genroom(r->b);
	if(r->a==NULL && r->b==NULL)
	{
		room rm;
		rm.flags=0;
		rm.w= R_XMIN+ rng(MIN(R_XMAX,r->w) -R_XMIN);
		rm.h= R_YMIN+ rng(MIN(R_YMAX,r->h) -R_YMIN);
		
		_min=MAX(r->x, r->cx-rm.w)+1;
		_max=MIN(r->cx, r->x+r->w-rm.w);
		rm.x=_min+rng(_max-_min);

		_min=MAX(r->y, r->cy-rm.h)+1;
		_max=MIN(r->cy, r->y+r->h-rm.h);
		rm.y=_min+rng(_max-_min);

		//rm.y= r->y+ (r->h-rm.h)/2;
		placeroom(rm, db.nrooms++);
	}
}


static void _region_genpassage(region *r)
{
	// i need to add a little bit of random in this
	if(r && r->a && r->b)
	{
		if(!isleaf(r->a)) _region_genpassage(r->a);
		if(!isleaf(r->b)) _region_genpassage(r->b);
		int x1=r->a->cx;
		int y1=r->a->cy;
		int x2=r->b->cx;
		int y2=r->b->cy;
		if(x1==x2) connect_vertical(x1,y1,y2);
		if(y1==y2) connect_horizont(x1,x2,y1);
	}
}

static void _region_free(region *r)
{
	if(r)
	{
		if(r->a) _region_free(r->a);
		if(r->b) _region_free(r->b);
		free(r);
		//_log("freeing region: %d (a:%d,b:%d)",r, r->a?1:0, r->b?1:0);
	}
}

static void connect_vertical(int x1, int y1, int y2)
{
	int m;
	for(int y=MIN(y1,y2); y<MAX(y1,y2); y++)
	{
		switch( map[y*XMAX +x1] )
		{
			case VWALL: case HWALL: case DOOR: 
				m=DOOR;
				break;
			case FLOOR: m=FLOOR; break;
			//default: m=PASSAGE; break;
			default: m=PASSAGE; break;
		}
		map[y*XMAX+x1]=m;
	}
}
static void connect_horizont(int x1, int x2, int y1)
{
	int m;
	for(int x=MIN(x1,x2); x<MAX(x1,x2); x++)
	{
		switch( map[y1*XMAX +x] )
		{
			case VWALL: case HWALL: case DOOR: m=DOOR; break;
			case FLOOR: m=FLOOR; break;
			default: m=PASSAGE; break;
		}
		map[y1*XMAX+x]=m;
	}
}

static void post_process()
{
	int m,new;
	int nn,ne,nw,ns;
	for(int x=1;x<(XMAX-1);x++)
	for(int y=1;y<(YMAX-1);y++)
	{
		m=(new=map[y*XMAX+x]);
		nn=map[(y-1)*XMAX+x];
		ns=map[(y+1)*XMAX+x];
		nw=map[y*XMAX+(x-1)];
		ne=map[y*XMAX+(x+1)];
		if(m==DOOR && (nn!=PASSAGE && ne!=PASSAGE && nw!=PASSAGE && ns!=PASSAGE))
		{
			if(nn==FLOOR || ns==FLOOR) new=HWALL;
			if(ne==FLOOR || nw==FLOOR) new=VWALL;
		}
		/*
		if(m==STONE)
		{
			if(nn==PASSAGE || ns==PASSAGE || nn==DOOR || ns==DOOR) new=HWALL;
			if(ne==PASSAGE || nw==PASSAGE || nw==DOOR || ne==DOOR) new=VWALL;
		}
		*/
		map[y*XMAX+x]=new;
	}
}

static void produce_map()
{
	int id;
	tile *t;
	for(int x=0;x<XMAX;x++) map[(YMAX-1)*XMAX+x]=STONE;
	for(id=0; id<XMAX*YMAX; id++)
	{
		//db.levels[db.cur_level].tile_flags[id] |= MS_EXPLORED;
		t=&db.tiles[id];
		t->c=map[id];
		t->flags=0;
		switch(map[id])
		{
			case VWALL:
			case HWALL:
			case STONE:
				t->flags |= ML_OBSTRUCTS;
				break;

			case FLOOR: 
			case PASSAGE:
			case DOOR:
			case UPSTAIRS: 
			case DOWNSTAIRS: 
				t->flags |= ML_TRANSPARENT;
				break;
			default:
				t->flags |= ML_OBSTRUCTS;
				break;
		}
	}
	for(int id=0;id<DBSIZE_CREATURES;id++)
	{
		Entity *e=&db.creatures[id];
		if(e && (e->flags&ISACTIVE))
		{
			if(e->pos.z==db.cur_level) moat(e->pos.x,e->pos.y)=e;
		}
	}
}

static void place_upstair(room *r, level *l)
{
	int x=r->x+1 + rng(r->w-2);
	int y=r->y+1 + rng(r->h-2);
	l->upstair=y*XMAX+x;
	map[l->upstair]=UPSTAIRS;
}
static void place_dnstair(room *r, level *l)
{
	int x=r->x+1 + rng(r->w-2);
	int y=r->y+1 + rng(r->h-2);
	l->downstair=y*XMAX+x;
	map[l->downstair]=DOWNSTAIRS;
}

void find_neighbours()
{
	int id, x, y;
	tile *t;
	for(id=0; id<XMAX*YMAX; id++)
	{
		x=id%XMAX;
		y=id/XMAX;
		t=&db.tiles[id];
		memset(t->neighbours, -1, sizeof(int)*8);

		if(t->flags&ML_OBSTRUCTS) continue;

		if(x>0 && 		!obstructs(x-1,y)) t->neighbours[0]=id-1;
		if(x<(XMAX-1)&& !obstructs(x+1,y)) t->neighbours[1]=id+1;
		if(y>0 && 		!obstructs(x,y-1)) t->neighbours[2]=id-XMAX;
		if(y<(YMAX-1)&& !obstructs(x,y+1)) t->neighbours[3]=id+XMAX;
		
		if(x>0 && y>0 && !obstructs(x-1,y-1)) 				t->neighbours[4]=id-1-XMAX;
		if(x>0 && y<(YMAX-1) && !obstructs(x-1,y+1)) 		t->neighbours[5]=id-1+XMAX;
		if(x<(XMAX-1) && y>0 && !obstructs(x+1,y-1)) 		t->neighbours[6]=id+1-XMAX;
		if(x<(XMAX-1) && y<(YMAX-1) && !obstructs(x+1,y+1)) t->neighbours[7]=id+1+XMAX;
	}
}

#undef isleaf
