#include "diabaig.h"

/*	ISAGRO
 *		>	if player in room, run to player
 *		>	cantrack set true and it "follows" player
 *		>	else just stand in room
 *
 *	CANTRACK AGRO
 *		>	asap, begin tracking player
 *		>	if player is the next step, attack
 *
 *	CANTRACK FRIENDLY
 *		>	track player until threshold closeness
 *		>	then do wander
 */




/*
static int _movealongbfs(Entity *e, nav_node *next)
{
	int status=RETURN_UNDEF;
	if(e->_c.stamina<=0) return RETURN_FAIL;
	int dx=next->vertex%XMAX - e->pos.x;
	int dy=next->vertex/XMAX - e->pos.y;
	if(dx>0 && dy==0) status=walk(e,east);
	if(dx<0 && dy==0) status=walk(e,west);
	if(dx==0 && dy>0) status=walk(e,south);
	if(dx==0 && dy<0) status=walk(e,north);
	
	if(dx>0 && dy<0) status=walk(e,northeast);
	if(dx>0 && dy>0) status=walk(e,southeast);
	if(dx<0 && dy<0) status=walk(e,northwest);
	if(dx<0 && dy>0) status=walk(e,southwest);
	return status;
}
*/

int wander(Entity *e)
{
	//int dir=rng(9)-1;

	int dir=nodir;
	int dirs[8]={1,1,1,1,1,1,1,1};
	for(int i=0; i<8; i++)
	{
		int id=db.tiles[e->pos.y*XMAX+e->pos.x].neighbours[i];
		if((db.tiles[id].flags&ML_OBSTRUCTS) || (db.tiles[id].c==DOOR))
		{
			dirs[i]=0;
		}
	}
	dir=weighted_pick(dirs,8);

	return walk(e,dir);
}

int hunt(Entity* e, Entity* target)
{
	int status=RETURN_UNDEF;
	int next=-1, direction=nodir;
	int keep=1;
	nav_node *nodemap=NULL;

	if(e && target)
	{
		if(e->_c.flags&ISBLIND) return wander(e);
		if(target->_c.flags&ISINVIS) return RETURN_FAIL;

		if(target==player)
		{
			if(e->_c.last_seen_player < MAX_PATHMEM)
			{
				nodemap=path_memory[e->_c.last_seen_player];
				//msg("%s %d",getname(e),e->_c.last_seen_player);
			}
		}
		else
		{
			nodemap=dijk_new();
			dijk_addsrc(nodemap, target->pos.y*XMAX + target->pos.x,0);
			keep=0;
		}
		next=dijk_getpath(e,nodemap);

		if(next!=-1)
		{
			coord p={next%XMAX, next/XMAX, db.cur_level};
			direction=getdirection( e->pos, p);
			status=walk(e, direction);
		}
		else wander(e);

		if(!keep) free(nodemap);
	}
	else status=RETURN_FAIL;
	return status;
}

/*
int track_player(Entity *e)
{
	int status=RETURN_UNDEF;
	int player_vertex = player->pos.y*XMAX+player->pos.x;

	nav_node *node, *parent;
	if(e)
	{
		if(e->_c.flags&ISBLIND) return wander(e);
		if(player->_c.flags&ISINVIS) return RETURN_FAIL;

		node=&bfs_nodes[e->pos.y*XMAX+e->pos.x];
		if(node->parent!=-1 && node->parent!=player_vertex)
		{
			parent=&bfs_nodes[node->parent];
			status=_movealongbfs(e,parent);
		}
	}
	else status=RETURN_FAIL;
	return status;
}

int do_agro(Entity *e)
{
	int status=RETURN_UNDEF;
	int player_vertex = player->pos.y*XMAX+player->pos.x;

	nav_node *parent, *node=&bfs_nodes[e->pos.y*XMAX+e->pos.x];

	if(e)
	{
		if(e->_c.flags&ISBLIND) return wander(e);
		if(player->_c.flags&ISINVIS) return RETURN_FAIL;
		//if(e->_c._inroom==player->_c._inroom) e->_c.flags|=CANTRACK;
		if(node->parent!=-1 && node->parent==player_vertex)
		{
			parent=&bfs_nodes[node->parent];
			status=_movealongbfs(e,parent);
		}
	}
	else status=RETURN_FAIL;
	return status;
}
*/

int flee(Entity *e, Entity* target)
{
	// This still isnt right but its not far off
	int status=RETURN_UNDEF;
	nav_node *map=dijk_new();

	if(target==player)
	{
		memcpy(map,dijk_flee, sizeof(dijk_flee));
	}
	else
	{
		dijk_addsrc(map,target->pos.y*XMAX+target->pos.x, 0);

		dijk_scale(map, -1.2);
		dijk_scan(map);
	}

	for(Entity *e=db.creatures; (e-db.creatures)<DBSIZE_CREATURES; e++) dijk_place_entity(map,e);


	int id=dijk_getpath(e,map);

	if(id!=-1)
	{
		coord p={id%XMAX, id/XMAX,0};
		status=walk(e,getdirection( e->pos,p)); 
	}
	if(map) free(map);
	
	return status;
}

void do_first_sight(Entity *e)
{
					  char * lst[]={
						  "\"ah adventurer! care to see my wares?\", says %s",
						  "\"hey adventurer! i have stuff to trade you\", says %s",
						  "\"wait! i have stuff to trade, for gold\", says %s",
						  0};
	if(e)
	{
		if(e->_c.flags & ISAGRO) e->_c.flags |= ISFOLLOW;
		switch(e->_c.type)
		{
			case 'a': 
				msg("%s begins brewing potions",getname(e));
				break;
			case 'i':
				msg("%s conjures a set of illusionary clones",getname(e));
				_spawn_adds(e,'I',2);
				break;
			case 'd':
				_spawn_adds(e,'e',rng(3));
				_spawn_adds(e,'k',rng(2));
				_spawn_adds(e,'j',rng(2));
				_spawn_adds(e,'b',rng(3));
				msg("%s communes with his beasts",getname(e));
				break;

			case 'c': msg("%s meows",getname(e));break;
			case 'f':
				if(!(e->_c._inroom->flags&RM_ISDARK))
					msg("%s stares at you, atop her delicious looking toadstool",getname(e));
			   	break;
			case 'n': _spawn_adds(e,'z',2); msg("%s surrounds himself in undead",getname(e)); break;
			case 'w': _spawn_adds(e,'s',2); msg("%s surrounds herself in spiders",getname(e)); break;
			case 'M': msg("the Mother of Dragons guards this nest");break;
			case 'U': 
					  _spawn_adds(e,'u',2+rng(3)); 
					  _spawn_adds(e,'o',2+rng(3)); 
					  _spawn_adds(e,'g',2+rng(3)); 
					  msg("%s rallies his troops",getname(e));
					  break;
			case 'O': _spawn_adds(e,'b',5+rng(5));
					  msg("%s is distracted by bats",getname(e));
					  break;
			case 'R':
					  msg( lst[ rng(clen(lst))], getname(e) );
					  break;
			case 'V': 
					  _spawn_adds(e,'b',5);
					  break;

			case 'j':
			case 'S':
			case 'C': case 'E': case 'P': case 'L':
					  e->_c.flags|=CANTRACK;
					  break;
					 
		}
	}
}

bool tile_lineofsight(Entity *e, int tile_id)
{
	bool visible=true;
	coord src,dest;
	int x,y;
	float theta;
	if(e)
	{
		src=e->pos;
		dest.x=tile_id%XMAX;
		dest.y=tile_id/XMAX;
		theta=atan2(dest.y-src.y, dest.x-src.x);
		for(float d=0.0; d<=sqrt(pow(src.x-dest.x,2)+pow(src.y-dest.y,2));d+=1.0)
		{
			x=(int)round(d*cos(theta))+src.x;
			y=(int)round(d*sin(theta))+src.y;
			if((y*XMAX+x)==tile_id) break; //reached target
			if(!transparent(x,y) || tileat(x,y)->c==DOOR)
			{
				visible=false;
				break;
			}
		}
	}
	return visible;
}
bool lineofsight(Entity *e, Entity *target)
{
	bool visible=false;
	if(e && target)
	{
		if(target->_c.flags & ISINVIS) visible=false;
		else
			visible=tile_lineofsight(e,target->pos.y*XMAX +target->pos.x);
	}
	return visible;
}








// PATHFINDING STUFF
/* BFS reversed will be the primary path finding alg.
 * If a creature can track (e._c.flags & CANTRACK) then it
 * will track from the far side of the map. (this should be
 * limited to only a few creatures cause it will be haunting)
 * If a creature is within line of sight of the player and ISAGRO
 * then it will begin to follow BFS path too.
 *
 * NOTE: i need to add another pathing inside the room to stop traffic
 * jams between multiple creatures on the same path.
 *
 * Creatures that are agro and have seen the player will chase the player
 */


struct queue new_queue()
{
	struct queue q;
	q.front=0;
	q.rear=-1;
	memset(q.items, 0, sizeof(int)*XMAX*YMAX);
	return q;
}
void push_queue(struct queue *q, int id)
{
	q->items[++q->rear]=id;
}

int pull_queue(struct queue *q)
{
	return q->items[q->front++];
}

void reset_node(nav_node *node)
{
	if(node)
	{
		node->visited=false;
		node->parent=-1;
		node->depth=0;
	}
}

/*
void _init_pathfinding( nav_node nodes[])
{
	_log("beginning init pathfinding");
	int id=0, x,y;
	nav_node *node;
	for(node=&nodes[0]; node<&nodes[XMAX*YMAX]; node++, id++)
	{
		x=id%XMAX;
		y=id/XMAX;
		reset_node(node);
		node->vertex=id;
		for(int n=0;n<8;n++) node->neighbours[n]=-1;
		
		if(x>0 && 		!obstructs(x-1,y)) node->neighbours[0]=id-1;
		if(x<(XMAX-1)&& !obstructs(x+1,y)) node->neighbours[1]=id+1;
		if(y>0 && 		!obstructs(x,y-1)) node->neighbours[2]=id-XMAX;
		if(y<(YMAX-1)&& !obstructs(x,y+1)) node->neighbours[3]=id+XMAX;

		if(x>0 && y>0 && !obstructs(x-1,y-1)) node->neighbours[4]=id-1-XMAX;
		if(x>0 && y<(YMAX-1) && !obstructs(x-1,y+1)) node->neighbours[5]=id-1+XMAX;
		if(x<(XMAX-1) && y>0 && !obstructs(x+1,y-1)) node->neighbours[6]=id+1-XMAX;
		if(x<(XMAX-1) && y<(YMAX-1) && !obstructs(x+1,y+1)) node->neighbours[7]=id+1+XMAX;
	}
	//int id=0, x,y;
	//nav_node *node;
	//for(node=&bfs_nodes[0]; node<&bfs_nodes[XMAX*YMAX]; node++, id++)
	//{
	//	x=id%XMAX;
	//	y=id/XMAX;
	//	reset_node(node);
	//	node->vertex=id;
	//	for(int n=0;n<8;n++) node->neighbours[n]=-1;
	//	
	//	if(x>0 && 		!obstructs(x-1,y)) node->neighbours[0]=id-1;
	//	if(x<(XMAX-1)&& !obstructs(x+1,y)) node->neighbours[1]=id+1;
	//	if(y>0 && 		!obstructs(x,y-1)) node->neighbours[2]=id-XMAX;
	//	if(y<(YMAX-1)&& !obstructs(x,y+1)) node->neighbours[3]=id+XMAX;

	//	if(x>0 && y>0 && !obstructs(x-1,y-1)) node->neighbours[4]=id-1-XMAX;
	//	if(x>0 && y<(YMAX-1) && !obstructs(x-1,y+1)) node->neighbours[5]=id-1+XMAX;
	//	if(x<(XMAX-1) && y>0 && !obstructs(x+1,y-1)) node->neighbours[6]=id+1-XMAX;
	//	if(x<(XMAX-1) && y<(YMAX-1) && !obstructs(x+1,y+1)) node->neighbours[7]=id+1+XMAX;
	//}
	//_log("init pathfinding");
}
*/
void init_pathfinding()
{
	//_init_pathfinding( bfs_nodes );
	//_init_pathfinding( dijk_nodes );

	find_neighbours();
	for(int i=0; i<MAX_PATHMEM; i++)
		memcpy(path_memory[i],dijk_nodes,sizeof(dijk_nodes));


	/*
	_init_pathfinding( path_nodes[0]);

	for(int i=1; i<PATHFINDMAX; i++)
	{
		memcpy(path_nodes[i],path_nodes[0], sizeof(nav_node)*XMAX*YMAX);
	}
	*/
}

void bfs_flood(nav_node nodemap[], int nitems, int root)
{	// This is just BFS but ignores creatures
	struct queue q=new_queue();
	for(int id=0;id<nitems;id++) reset_node(&nodemap[id]);
	assert(root>=0 && root<nitems);
	nodemap[root].visited=true;
	push_queue(&q,root);

	int front,neighbour;
	while(q.front<=q.rear)
	{
		front=pull_queue(&q);
		//t=&db.tiles[front];
		for(int n=0; n<8; n++)
		{
			//neighbour=nodemap[front].neighbours[n];
			neighbour=db.tiles[front].neighbours[n];
			if(neighbour!=-1 && !nodemap[neighbour].visited)
			{
				nodemap[neighbour].visited=true;  
				nodemap[neighbour].parent=front;
				nodemap[neighbour].depth=nodemap[front].depth+1;
				push_queue(&q,neighbour);
			}
		}
	}
}

void bfs(nav_node nodemap[], int nitems, int root)
{
	struct queue q=new_queue();
	for(int id=0;id<nitems;id++) reset_node(&nodemap[id]);
	assert(root>=0 && root<nitems);
	nodemap[root].visited=true;
	push_queue(&q,root);

	int front,neighbour;
	tile *t;
	while(q.front<=q.rear)
	{
		front=pull_queue(&q);
		t=&db.tiles[front];
		//if(t->creature==player || !t->creature)  //late addition, run around other mobs
		if(!t->creature || front==root) //check there is no creature, unless you are looking at the root
		{
			for(int n=0; n<8; n++)
			{
				//neighbour=nodemap[front].neighbours[n];
				neighbour=db.tiles[front].neighbours[n];
				if(neighbour!=-1 && !nodemap[neighbour].visited)
				{
					nodemap[neighbour].visited=true;  
					nodemap[neighbour].parent=front;
					nodemap[neighbour].depth=nodemap[front].depth+1;
					push_queue(&q,neighbour);
				}
			}
		}
	}
}

int getpath(nav_node nodemap[], int from)
{
	int next=-1;
	nav_node node;
	if(nodemap && from!=-1)
	{
		node=nodemap[from];
		if(node.parent!=-1)
		{
			next=node.parent;
		}
	}
	return next;
}

//************************
//All the separate behaviour AIs
//**************************

void _ai_alchemist(Entity *e)
{
	int success=false;
	if( (e->_c.flags & SEENPLAYER) && lineofsight(e,player))
	{
		if(!rng(20) && !(player->_c.flags&ISBURN))
		{
			player->_c.flags|=ISBURN;
			add_daemon(player,D_BURN,5); 
			tileat(player->pos.x,player->pos.y)->air=COMBUST;
			tileat(player->pos.x,player->pos.y)->air_pressure+=0.5;

			success=true;
		}
		else if(!rng(15) && !(player->_c.flags&ISPOISON))
		{
			player->_c.flags|=ISPOISON;
			add_daemon(player,D_POISON,5); 
			tileat(player->pos.x,player->pos.y)->air=MIASMA;
			tileat(player->pos.x,player->pos.y)->air_pressure+=0.5;
			success=true;
		}
		else if(!rng(30) && !(player->_c.flags&ISSLOW))
		{
			player->_c.flags|=ISSLOW;
			add_daemon(player,D_SLOW,5); 
			success=true;
		}
		else if(!rng(30) && !(player->_c.flags&ISCONFUSED))
		{
			player->_c.flags|=ISCONFUSED;
			add_daemon(player,D_CONFUSE,3); 
			success=true;
		}
		else if(!rng(50) && !(player->_c.flags&ISLSD))
		{
			player->_c.flags|=ISLSD;
			add_daemon(player,D_LSD,10); 
			success=true;
		}
		else if(!rng(50) && !(player->_c.flags&ISREGEN))
		{
			player->_c.flags|=ISREGEN;
			add_daemon(player,D_FASTREGEN,5); 
			success=true;
		}

		if(success)
		{
			msg("%s throws a potion",getname(e));
			e->_c.stamina--;
			animation a={A_PROJ,'!', COLOR_PAIR(C_WHITE),e->pos,player->pos,0};
			//a.pos=e->pos;
			//a.target_pos=player->pos;
			animate(&a);
		}
	}
	
}

void _ai_cat(Entity* e)
{ // track player if far away or sometime close by
	int distance;
	if(e)
	{
		distance = pow(e->pos.x-player->pos.x,2) + pow(e->pos.y-player->pos.y,2);
		if(distance > 25 || !rng(5)) e->_c.flags|=CANTRACK;
		else e->_c.flags &= ~CANTRACK;


		if(e->_c.flags&CANTRACK) hunt(e,player);

		if(!rng(100))
		{
			if(db.tiles[e->pos.y*XMAX+e->pos.x].flags&ML_VISIBLE)
			{
				msg("%s meows",getname(e));
			}
		}
	}
}
void _ai_cat_offscreen(Entity* e)
{
	int id=0;
	if(e && !rng(500))
	{
		if(e->pos.z<db.cur_level) id=db.levels[db.cur_level].upstair;
		else if(e->pos.z>db.cur_level) id=db.levels[db.cur_level].downstair;

		db.tiles[id].creature=e;
		e->pos.z=db.cur_level;
		e->pos.x=id%XMAX;
		e->pos.y=id/XMAX;
		msg("you hear the soft pad of light feet");
	}
}

void _ai_druid(Entity *e)
{
	if( (e->_c.flags & SEENPLAYER) && lineofsight(e,player))
	{
		if(!rng(6) && !(player->_c.flags&ISBOUND))
		{
			add_daemon(player,D_BIND,3);
			msg("%s raises a hand and roots grow from the ground and grapple you",getname(e));
			e->_c.stamina--;
		}
	}
}

void _ai_illusionist(Entity *e)
{
	Entity *illusion, *tmp;
	int nillusions, nn=0;
	if( (e->_c.flags & SEENPLAYER) && !(e->flags & ISILLUSION) && (e->_c._inroom==player->_c._inroom) && e->_c._inroom)
	{
		e->_c.flags&=(~(ISAGRO|CANTRACK|ISFOLLOW)); //cannot be agro

		for(int id=0; id<DBSIZE_CREATURES;id++)
		{
			tmp=&db.creatures[id];
			if(tmp && (tmp->flags&ISACTIVE) && (tmp->_c.type=='I') && (tmp->_c.parent==e->id))
			{
				nn+=1;
			}
		}

		if(nn<MAXILLUSIONS && !rng(10))
		{
			nillusions=1+rng(3);
			for(int i=0;i<nillusions;i++)
			{
				illusion=_new_monster('I');
				if(illusion)
				{
					illusion->_c.parent=e->id;
					illusion->_c.form='i';
					illusion->_c.stat=(stats){ 1,0, {1,1}, 1,0 ,0}; //1hp and no str
					illusion->flags |= ISILLUSION;
					placeinroom(e->_c._inroom,illusion);
				}
			}
			moat(e->pos.x,e->pos.y)=NULL;
			placeinroom(e->_c._inroom, e); //teleport illusionist somewhere else in the room
			msg("a mirage! several illusionists appear");
			e->_c.stamina--;
		}
		else if(nn<MAXILLUSIONS && !rng(10)) // spawn illusion creatures
		{
			nillusions=3+rng(3);
			for(int i=0;i<nillusions;i++)
			{
				illusion=new_monster();
				if(illusion)
				{
					illusion->_c.form=illusion->_c.type;
					illusion->_c.type='I';
					illusion->_c.parent=e->id;

					illusion->_c.stat=(stats){ 1,1, {0,0}, 0,0 ,0}; //1hp and no str
					illusion->flags |= (ISILLUSION);
					illusion->_c.flags |= (ISWANDER|CANTRACK|ISFOLLOW);

					if(illusion->_c.form=='m') clear_entity(illusion);
					else placeinroom(e->_c._inroom,illusion);
				}
			}
			msg("%s raises his hands. several creatures materialise in the room",getname(e));
			e->_c.stamina--;
		}
	}
}

void _ai_necromancer(Entity *e)
{
	//if( (e->_c.flags & SEENPLAYER) && !(e->flags & ISILLUSION) && (e->_c._inroom==player->_c._inroom) && e->_c._inroom)
	if( (e->_c.flags & SEENPLAYER) && (e->_c._inroom==player->_c._inroom) && e->_c._inroom)
	{
		if(!rng(6))
		{
			int type= rng(3)?'r':'z';
			_spawn_adds(e,type,2+rng(2));
			msg("%s chants a spell, several reanimated corpses burst through the ground", getname(e));
			e->_c.stamina--;
		}

		else if(!rng(10))
		{
			int hp=modify_damage(player,10+rng(10), DMG_MAGIC);
			player->_c.stat.hp-=hp;
			add_daemon(e,D_FASTREGEN,hp);
			msg("%s points at you, you feel your life being drained out",getname(e));
			set_ripdata(RIP_KILLED,getname(e));
		}
	}
}

void _ai_vampire(Entity *e)
{
	if(e->_c.form==vBAT) wander(e);
}

void _ai_witch(Entity *e)
{
	int nspiders;
	//if((e->_c.flags & SEENPLAYER) && !(e->flags&ISILLUSION))
	if((e->_c.flags & SEENPLAYER) )
	{
		if(!rng(5) && lineofsight(e,player))
		{
			nspiders=1+rng(2);
			_spawn_adds(e,'s',nspiders);
			if(nspiders==1) msg("%s chants a spell. a spider crawls out of a crack in the floor",getname(e));
			else msg("%s chants a spell. spiders crawl out of cracks in the floor",getname(e));
			e->_c.stamina--;
		}
		else if(!rng(3) && lineofsight(e,player))
		{
			_spawn_adds(e,'b',2);
			msg("bats crawl from the fabric of %s cloak",getname(e));
		}
	}
}

void _ai_yeti(Entity *e)
{
	if((e->_c.flags&ISAGRO) &&lineofsight(e,player) && !rng(8))
	{
		tileat(e->pos.x,e->pos.y)->air_pressure+=1.5;
		tileat(e->pos.x,e->pos.y)->air=MIST;
		e->_c.stamina--;
	}
}

void _ai_Lindworm(Entity *e)
{
	if( db.tiles[ e->pos.y*XMAX+e->pos.x].flags&ML_VISIBLE )
	{
		if(!rng(OBSLIZ_HYPNORATE))
		{
			msg("%s's gaze is petrifying, you cannot move ",getname(e));
			add_daemon(player, D_FREEZE, 3+rng(5));

		}
	}

}

void _ai_MOD(Entity* e)
{
	if(e->_c.flags&SEENPLAYER)
	{
		if(!rng(15))
		{
			_spawn_adds(e,'Y',2);
			msg("more dragons appear in the nest");
		}
	}
}

void _ai_FellBeast(Entity *e)
{
	if(!rng(8) && (e->_c.flags&SEENPLAYER))
	{
		tileat(e->pos.x,e->pos.y)->air_pressure+=2;
		tileat(e->pos.x,e->pos.y)->air=MIASMA;
	}

}

void _ai_ObsidianLizard(Entity *e)
{
	Entity **lst=get_target_type('b');
	Entity *t;
	if(lst[0] && !(e->_c.flags&ISBLIND))
	{
		nav_node *map=dijk_new();
		if(e->_c.flags&SEENPLAYER) memcpy(map,dijk_nodes, sizeof(dijk_nodes));
			//dijk_addsrc(map, player->pos.y*XMAX+player->pos.x,0);


		int i=0;//,d=INT_MAX, target=0;
		while( (t=lst[i]) )
		{
			dijk_addsrc(map, t->pos.y*XMAX+t->pos.x,-10);
			i++;
		}
		
		int id=dijk_getpath(e,map);
		if(id!=-1)
		{
			coord p={id%XMAX, id/XMAX,0};
			walk(e,getdirection( e->pos,p)); 
		}
		
	}
	else
	{
		//if( dijk_nodes[ e->pos.y*XMAX+e->pos.x].weight <5 )
		if( db.tiles[ e->pos.y*XMAX+e->pos.x].flags&ML_VISIBLE )
		{
			if(!rng(OBSLIZ_HYPNORATE))
			{
				msg("%s dazzles you with its colourful scales, you are entranced",getname(e));
				add_daemon(player, D_SLEEP, 2+rng(2));

			}
		}

	}
	free(lst);
}

void _ai_Jaguar(Entity *e)
{

	if(get_efflevel()>= monsters['b'-'A'].level && !rng(SHAPESHIFTRATE))
	{
		Entity *t=_new_monster('T');
		t->_c.flags|=(e->_c.flags);
		t->pos=e->pos;
		t->_c.stat.hp=e->_c.stat.hp;

		char *name=strdup(getname(e));
		msg("%s shapeshifts into %s",name,getname(t));
		free(name);

		memcpy(e,t,sizeof(Entity));
		clear_entity(t);
	}
}

void _ai_Tezcatlipoca(Entity *e)
{
	//Targets the Quetzalcoatl as well as the player
	Entity **lst=get_target_type('Q');
	Entity *t;
	nav_node *map=dijk_new();
	//dijk_addsrc(map, player->pos.y*XMAX+player->pos.x,0);
	//if(e->_c.flags&SEENPLAYER && !(player->_c.flags&ISINVIS)) dijk_addsrc(map, player->pos.y*XMAX+player->pos.x,0);
	if(e->_c.flags&SEENPLAYER) memcpy(map,dijk_nodes, sizeof(dijk_nodes));

	if(lst[0] && !(e->_c.flags&ISBLIND))
	{
		int i=0;
		while( (t=lst[i]) )
		{
			dijk_addsrc(map, t->pos.y*XMAX+t->pos.x,0);
			i++;
		}
	}
		
	int id=dijk_getpath(e,map);
	if(id!=-1)
	{
		coord p={id%XMAX, id/XMAX,0};
		walk(e,getdirection( e->pos,p)); 
	}

	if(!rng(SHAPESHIFTRATE))
	{
		Entity *j=_new_monster('J');
		j->_c.flags|=(e->_c.flags);
		j->pos=e->pos;
		j->_c.stat.hp=e->_c.stat.hp;

		char *name=strdup(getname(e));
		msg("%s shapeshifts into %s",name,getname(j));
		free(name);
		memcpy(e,j,sizeof(Entity));
		clear_entity(j);

	}

}
void _ai_Quetzalcoatl(Entity *e)
{
	//Targets the Tezcatlipoca as well as the player
	Entity **lst=get_target_type('T');
	Entity *t;
	nav_node *map=dijk_new();
	//dijk_addsrc(map, player->pos.y*XMAX+player->pos.x,0);
	//if(e->_c.flags&SEENPLAYER && !(player->_c.flags&ISINVIS)) dijk_addsrc(map, player->pos.y*XMAX+player->pos.x,0);
	if(e->_c.flags&SEENPLAYER) memcpy(map,dijk_nodes, sizeof(dijk_nodes));

	if(lst[0] && !(e->_c.flags&ISBLIND))
	{
		int i=0;
		while( (t=lst[i]) )
		{
			dijk_addsrc(map, t->pos.y*XMAX+t->pos.x,0);
			i++;
		}
	}
		
	int id=dijk_getpath(e,map);
	if(id!=-1)
	{
		coord p={id%XMAX, id/XMAX,0};
		walk(e,getdirection( e->pos,p)); 
	}

}

void _ai_VampireLord(Entity *e)
{
	int bcount=0, vcount=0;
	for(int i=0; i<DBSIZE_CREATURES; i++) 
	{
		if(db.creatures[i]._c.parent==e->id)
		{
			if(db.creatures[i]._c.type=='b') bcount++;
			if(db.creatures[i]._c.type=='v') vcount++;
		}
	}
	if(lineofsight(e,player))
	{
		if(bcount<(MAXILLUSIONS/2)) _spawn_adds(e,'b',1);
		if(vcount<2 && !rng(3)) _spawn_adds(e,'v',1);
	}

}

void _ai_Banshee(Entity *e)
{
	int i=0;
	Entity **lst;
	if(lineofsight(e,player))
	{
		if(!rng(15))
		{
			lst=get_target_radius(e,10);
			while(lst[i])
			{
				if(lst[i]!=e && lst[i]->_c.type!='S') add_daemon(lst[i],D_CONFUSE, 3+rng(3));
				i++;
			}
			free(lst);

			msg("%s screams! it is painfully disorientating",getname(e));
			animation a={A_AOE,'.',COLOR_PAIR(C_WHITE)|A_BOLD, e->pos, {0,0,0},0};
			//a.pos=e->pos;
			a.pos.z=10;
			//add_animation(a);
			animate(&a);
		}
		else if(!rng(15))
		{
			_spawn_adds(e,'S',2+rng(2));
			msg("a group of Sidhes flock to %s",getname(e));
		}
		

	}
}

void _ai_Nightmare(Entity *e)
{// THIS IS PRETTY SCAREY
	int adds=0;
	if(lineofsight(e,player))
	{
		if(!rng(10)){_spawn_adds(e,'p',2); adds++;}
		if(!rng(15)){_spawn_adds(e,'S',2); adds++;}
		if(!rng(25)){_spawn_adds(e,'B',1); adds++;}

		if(adds)
		{
			if(e->_c._inroom && (e->_c._inroom->flags&RM_ISDARK))
			{
				e->_c._inroom->flags&=~RM_ISDARK;
				msg("the candles on the walls flicker back into light");
			}
			else
			{
				msg("ghostly nightmares appear before your eyes");
			}
		}
		else if(!rng(8))
		{
			if(e->_c._inroom && !(e->_c._inroom->flags&RM_ISDARK))
			{
				e->_c._inroom->flags|=RM_ISDARK;
				msg("the lights go out!");

			}
		}
		else if(!rng(20))
		{
			coord p=e->pos;
			do
			{
				p.x=e->pos.x + rng(10) - 5;
				p.y=e->pos.y + rng(10) - 5;
			}while(!islegal(p));
			moat(e->pos.x,e->pos.y)=NULL;
			e->pos=p;
			moat(e->pos.x,e->pos.y)=e;
			msg("%s fades away and reappears behind you",getname(e));
		}
	}

}

Entity *_spawn_adds(Entity *e, int type, int number)
{
	// this returns the last add (for utility)
	Entity *add=NULL;

	int count=0;
	for(int i=0; i<DBSIZE_CREATURES; i++) 
	{
		if(db.creatures[i]._c.parent==e->id) count++;
	}
	if(count>MAXADDS) return NULL;

	for(int i=0;i<number;i++)
	{
		if(e && (add=_new_monster(type)))
		{
			if(e->_c._inroom) placeinroom(e->_c._inroom,add);
			else
			{
				coord p;
				int attempts=100;
				do
				{
					p.x=e->pos.x-5 + rng(10);
					p.y=e->pos.y-5 + rng(10);
					p.z=e->pos.z;
					attempts--;
				}while( (!islegal(p) || moat(p.x,p.y)) &&attempts>0);
				
				if(attempts>0)
				{
					moat(p.x,p.y)=add;
					add->pos=p;
				}
			}
			add->_c.parent=e->id;

			if(type=='I')
			{
				add->_c.form='i';
			}
		}
	}
	return add;
}
