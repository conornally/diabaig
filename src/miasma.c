#include <diabaig.h>
int FLUID_MATRIX[NFLUID_TYPES][NFLUID_TYPES]={
{0,0,0,0,0},
{1,0,0,1,0},
{1,1,0,0,0},
{1,0,1,0,0},
{1,0,0,0,0}};

void air_diffuse(float decay, float noise)
{

	float sum=0;
	int  types[XMAX*YMAX];
	float pres[XMAX*YMAX];
	for(int i=0; i<(XMAX*YMAX); i++) 
	{
		pres[i]=db.tiles[i].air_pressure;
		types[i]=db.tiles[i].air;
		sum+=pres[i];
	}
	if(!sum) return;

	for(int id=0; id<(XMAX*YMAX); id++)
	{
		int num=1;
		tile* t=&db.tiles[id];
		float total= t->air_pressure;

		if(t->flags&ML_OBSTRUCTS) 	   continue;
		if(t->c==DOOR && !t->creature)
		{
			//types[id]=AIR;
			//pres[id]=0;
			pres[id]*=0.99; // decay slowly
			continue; //this traps the gas on the door tile and it doesnt even decay
		}

		for(int n=0;n<8;n++)
		{
			int neigh=t->neighbours[n];
			if(neigh>=0 && neigh<(XMAX*YMAX))
			{
				//TYPES
				tile t=db.tiles[neigh];
				if(FLUID_MATRIX[t.air][types[id]] && (t.air_pressure>pres[id])) types[id]=types[neigh];
				total+=db.tiles[neigh].air_pressure;
				num++;
			}
		}

		//DIFFUSE
		pres[id]=(total/num)*decay* ((1.0-noise/2.0)+noise*(float)rand()/(float)RAND_MAX);

		//NULLIFY
		//if(pres[id]<=0.05) 
		if(pres[id]<=0.01){ types[id]=AIR; pres[id]=0;}
	}


	for(int i=0; i<(XMAX*YMAX); i++) 
	{
		db.tiles[i].air_pressure=pres[i];
		db.tiles[i].air=types[i];
	}
}


