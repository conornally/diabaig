#include "diabaig.h"

nav_node *dijk_new()
{
	nav_node *map=malloc(sizeof(nav_node)*XMAX*YMAX);
	memcpy(map, dijk_nodes, sizeof(dijk_nodes));
	dijk_reset(map);
	return map;
}

void dijk_reset(nav_node map[])
{
	for(nav_node *m=map; (m-map)<(XMAX*YMAX); m++)
	{
		m->weight=DIJK_MAX;
		m->visited=false;
		m->parent=-1;
		m->depth=0;
	}
}

static float *dijk_getneighbours(float map[], int id)
{
	int i=0,x,y;
	float *neighs=(float*)malloc(8*sizeof(float));
	for(float *n=neighs; (n-neighs)<8; n++) *n=DIJK_MAX;

	if(id>=0 && id<XMAX*YMAX)
	{
		x=id%XMAX;
		y=id/XMAX;

		for(int dx=-1;dx<=1;dx++)
		for(int dy=-1;dy<=1;dy++)
		{
			if( !((dx==0)&&(dy==0)) &&  ((x+dx)>=0) && ((x+dx)<XMAX) && ((y+dy)>=0) && ((y+dy)<YMAX) && !obstructs(x+dx,y+dy))
			{
				//neighs[i++]=map[ (y+dy)*XMAX+(x+dx) ];
				neighs[i]=map[ (y+dy)*XMAX+(x+dx) ];
			}
			i++;
		}
	}
	return neighs;
}

void xdijk_scan(float map[])
{
	int id, changed=1;
	float min, *neighs;

	while(changed)
	{
		changed=0;
		for(id=0;id<XMAX*YMAX;id++)
		{
			if( !(db.tiles[id].flags&ML_OBSTRUCTS) ) 
			{
				min=DIJK_MAX;
				neighs=dijk_getneighbours(map, id);

				for(int ii=0; ii<8; ii++) 
				{
					min=MIN(min,neighs[ii]);
				}

				if( (map[id]-min)>1.0)
				{
					map[id]=(min+1.0);
					changed=1;
				}
				free(neighs);
			}
		}
	}
}

static int _dijkscan(nav_node nodemap[])
{
	int id,neighbour,changes=0;
	float min=DIJK_MAX;
	struct queue q= new_queue();

	// Find a starting point + UNVISIT
	for(int i=0;i<XMAX*YMAX;i++)
	{
		nodemap[i].visited=0; // RESET NODE
		if(!(db.tiles[i].flags&ML_OBSTRUCTS) && nodemap[i].weight<min)
		{
			min=nodemap[i].weight;
			id=i;
		}
	}
	push_queue(&q, id);

	while(q.front<=q.rear)
	{
		min=DIJK_MAX;
		id=pull_queue(&q);

		for(int n=0;n<8;n++) //look for minimum neigbour weight
		{
			//neighbour=nodemap[id].neighbours[n];
			neighbour=db.tiles[id].neighbours[n];
			if(neighbour!=-1)
			{
				min=MIN(nodemap[neighbour].weight, min);
				if(!(nodemap[neighbour].visited++)) push_queue(&q,neighbour);
			}
		}
		if( (nodemap[id].weight-min)>1.0)
		{
			nodemap[id].weight=min+1.0;
			changes++;
		}
	}
	return changes;
}

void dijk_scan(nav_node nodemap[])
{
	while(_dijkscan(nodemap));
}

void dijk_addsrc(nav_node map[], int id, float weight)
{
	if(id>=0 && id<(XMAX*YMAX))
	{
		map[id].weight=weight;
		dijk_scan(map);
	}
}
//void dijk_addsrc(float map[], int id, float weight)
//{
//	if(id>=0 && id<(XMAX*YMAX))
//	{
//		map[id]=weight;
//		dijk_scan(map);
//	}
//}

void dijk_scale(nav_node nodemap[], float factor)
{
	for(int id=0; id<XMAX*YMAX; id++)
	{
		if(!(db.tiles[id].flags&ML_OBSTRUCTS))
			nodemap[id].weight*=factor;
	}
}
void dijk_combine(float *dest, float *add)
{
	for(int id=0;id<XMAX*YMAX; id++)
	{
		if((dest[id]!=DIJK_MAX) && (add[id]!=DIJK_MAX)) dest[id]+=add[id];
	}
}

void dijk_place_entity(nav_node map[], Entity *e)
{
	if(e && e->flags&ISACTIVE && e->pos.z==db.cur_level)
	{
		map[e->pos.y*XMAX+e->pos.x].weight= MAX(map[e->pos.y*XMAX+e->pos.x].weight+1,1);
	}
}

int dijk_getpath(Entity *e, nav_node *map)
{ //needs work
	int id=-1;
	float min;
	int ids[8]={-1,-1,-1,-1,-1,-1,-1,-1};
	if(e && map)
	{
		int x=e->pos.x;
		int y=e->pos.y;
		int ii=y*XMAX+x;
		if( (x-1)>=0 ) 	 ids[0]=ii-1;
		if( (x+1)<XMAX ) ids[1]=ii+1;
		if( (y-1)>=0 ) 	 ids[2]=ii-XMAX;
		if( (y+1)<YMAX ) ids[3]=ii+XMAX;
		if( (x-1)>=0   && (y-1)>=0) 	 ids[4]=ii-1-XMAX;
		if( (x+1)<XMAX && (y-1)>=0) 	 ids[5]=ii+1-XMAX;
		if( (x-1)>=0   && (y+1)<YMAX) 	 ids[6]=ii-1+XMAX;
		if( (x+1)<XMAX && (y+1)<YMAX) 	 ids[7]=ii+1+XMAX;

		min=map[ii].weight;
		for(ii=0; ii<8; ii++)
		{
			int i=ids[ii];
			if( !(db.tiles[i].flags&ML_OBSTRUCTS) && (map[i].weight <min))
			{
				// check NO Creatures, unless it is a target
				if(!db.tiles[i].creature || map[i].weight<=0)
				{
					min=map[i].weight;
					id=i;
				}
			}

		}
	}
	return id;
}
//int dijk_getpath(Entity *e, float *map)
//{ //needs work
//	int id=-1;
//	float min=DIJK_MAX;
//	if(e && map)
//	{
//		for(int x=-1; x<=1; x++)
//		for(int y=-1; y<=1; y++)
//		{
//			int i=(e->pos.y+y)*XMAX+ (e->pos.x+x);
//			if( !(db.tiles[i].flags&ML_OBSTRUCTS) && (map[i] <min))
//			{
//				// check NO Creatures, unless it is the target
//				if(!db.tiles[i].creature || map[i]==0)
//				{
//					min=map[i];
//					id=i;
//				}
//			}
//		}
//	}
//	return id;
//}
