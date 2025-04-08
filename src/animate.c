#include "diabaig.h"
#define MAXANIMS 20

static char animation_layer[XMAX*YMAX];
static int  wattr_layer[XMAX*YMAX];
//static animation animations[MAXANIMS];

int _aframe_projectile(animation* a, int frame)
{
	float x=a->target_pos.x-a->pos.x;
	float y=a->target_pos.y-a->pos.y;
	float dd=sqrt( pow(x,2) + pow(y,2) );
	float dx=x/dd;
	float dy=y/dd;

	int ii= ( (int)round(a->pos.y+(dy*frame))*XMAX + (int)round(a->pos.x+(dx*frame)) );

	animation_layer[ii]=a->symbol;
	wattr_layer[ii]=a->colour;

	//Either it gets to the dark or it goes out of sight
	return (frame>=(int)round(dd)) || !(db.tiles[ii].flags&ML_VISIBLE);
}

int _aframe_AOE(animation* a, int frame)
{
	int x,y;
	int still_visible=0;
	for(float d=0; d<360; d++)
	{
		x= (int)(a->pos.x + (frame*cos(3.14*d/180.0)));
		y= (int)(a->pos.y + (frame*sin(3.14*d/180.0)));
		if( (x>=0)&&(x<XMAX)&&(y>=0)&&(y<YMAX))
		{
			animation_layer[y*XMAX+x]=a->symbol;
			wattr_layer[y*XMAX+x]=a->colour;
			if(db.tiles[y*XMAX+x].flags&ML_VISIBLE) still_visible=1;
		}
	}
	return frame>=(a->pos.z) || !still_visible;
}

int _aframe_shootdir(animation* a, int frame)
{
	int x=a->pos.x;
	int y=a->pos.y;
	int dir=a->pos.z;
	int hit=0;
	frame++;

	switch(dir)
	{
		case north: y-=frame; break;
		case south: y+=frame; break;
		case east:  x+=frame; break;
		case west:  x-=frame; break;
		case northwest: y-=frame; x-=frame; break;
		case northeast: y-=frame; x+=frame; break;
		case southwest: y+=frame; x-=frame; break;
		case southeast: y+=frame; x+=frame; break;
	}

	if( (x>=0)&&(x<XMAX)&&(y>=0)&&(y<YMAX))
	{
		animation_layer[y*XMAX+x]=a->symbol;
		wattr_layer[y*XMAX+x]=a->colour;
		hit= (obstructs(x,y) || moat(x,y));
	}


	return hit || !(db.tiles[y*XMAX+x].flags&ML_VISIBLE) ;

}

int _aframe_laser(animation* a, int frame)
{
	float x=a->target_pos.x-a->pos.x;
	float y=a->target_pos.y-a->pos.y;
	float dd=sqrt( pow(x,2) + pow(y,2) );
	float dx=x/dd;
	float dy=y/dd;

	int length=a->pos.z;
	int ii;

	for(int l=length; l>0; l--)
	{
		int nsteps=MAX(1,frame-l);
		ii= ( (int)round(a->pos.y+(dy*nsteps))*XMAX + (int)round(a->pos.x+(dx*nsteps)) );
		animation_layer[ii]=a->symbol;
		wattr_layer[ii]=a->colour;
	}

	//Either it gets to the dark or it goes out of sight
	return (frame>=(int)round(dd)) || !(db.tiles[ii].flags&ML_VISIBLE) || db.tiles[ii].flags&ML_OBSTRUCTS;
}

int _aframe_twirl(animation* a, int frame)
{
	int dx=0,dy=0,c='|';
	switch(frame)
	{
		case 0: dx=0, dy--; c='|'; break;
		case 1: dx++, dy--; c='/'; break;
		case 2: dx++, dy=0; c='-'; break;
		case 3: dx++, dy++; c='\\'; break;
		case 4: dx=0, dy++; c='|'; break;
		case 5: dx--, dy++; c='/'; break;
		case 6: dx--, dy=0; c='-'; break;
		case 7: dx--, dy--; c='\\'; break;
		default: break;
	}
	animation_layer[ (a->pos.y+dy)*XMAX + (a->pos.x+dx) ]=c;
	wattr_layer[ (a->pos.y+dy)*XMAX + (a->pos.x+dx) ]=a->colour;
	return frame==8;
}

int _aframe_slice(animation* a, int frame)
{
	int dir=a->pos.z;
	int dirs[3]={ (dir+7)%8, dir, (dir+9)%8 };
	int dx=0,dy=0;
	char c;

	switch(dirs[frame])
	{
		case north: 	dx= 0; dy=-1; c='|'; break;
		case northeast: dx= 1; dy=-1; c='/'; break;
		case east:      dx= 1; dy= 0; c='-'; break;
		case southeast: dx= 1; dy= 1; c='\\'; break;
		case south:     dx= 0; dy= 1; c='|'; break;
		case southwest: dx=-1; dy= 1; c='/'; break;
		case west:      dx=-1; dy= 0; c='-'; break;
		case northwest: dx=-1; dy=-1; c='\\'; break;
	}
	animation_layer[ (a->pos.y+dy)*XMAX + (a->pos.x+dx) ]=c;
	wattr_layer[ (a->pos.y+dy)*XMAX + (a->pos.x+dx) ]=a->colour;
	return frame>=3;
}


int animate(animation* a)
{
	int status=0;
	int finished=0;
	int frame=0;
	if(a)
	{
		while(!finished)
		{
			memset(animation_layer, '\0', sizeof(char)*XMAX*YMAX);


			switch(a->type)
			{
				case A_PROJ: finished=_aframe_projectile(a,frame/1);  break;
				case A_AOE: finished=_aframe_AOE(a,frame/2);		  break;
				case A_SHOOTDIR: finished=_aframe_shootdir(a,frame/2);break;
				case A_LASER: finished=_aframe_laser(a,frame/1);	  break;
				case A_TWIRL: finished=_aframe_twirl(a,frame/3);	  break;
				case A_SLICE: finished=_aframe_slice(a,frame/3);	  break;
				default: finished=1;break;
			}
			display();

			//if(a->colour) wattron(win,COLOR_PAIR(a->colour));
			if(a->colour) wattron(win,a->colour);
			for(int i=0;i<XMAX*YMAX;i++)
			{
				if(animation_layer[i] && db.tiles[i].flags&ML_VISIBLE)
				{
					wmove(win, i/XMAX, i%XMAX);
					waddch(win,animation_layer[i]);
				}
			}
			//if(a->colour) wattroff(win,COLOR_PAIR(a->colour));
			if(a->colour) wattroff(win,a->colour);
			wmove(win,0,0);
			wrefresh(win);
			platform_sleep((int)(ANIM_RATE));
			frame++;
		}
	}
	else status=1;
	return status;
}
/*
void add_animation(animation a)
{
	if(a.type!=A_NONE)
	{
		for(int id=0; id<MAXANIMS; id++)
		{
			if(animations[id].type==A_NONE) 
			{
				animations[id]=a;
				break;
			}
		}
	}
}

int run_animate()
{
// This is way better, it makes so much more sense,
// but it doesnt work, because the event has already
// happened at the time the animation does.
// so the target is already dead, or the item is already
// at objat. or the status has already been applied
// so it doesnt display properly... :(
//
	int frame=0, running=1;
	int sum=0, rate;
	int (*fn)(animation*, int);

	do
	{
		running=0;
		memset(animation_layer, '\0', sizeof(char)*XMAX*YMAX);
		for(animation *a=animations; (a-animations)<MAXANIMS; a++)
		{
			if(a->type!=A_NONE)
			{
				switch(a->type)
				{
					case A_PROJ: 	fn=&_aframe_projectile; rate=1; break; //running+=!_aframe_projectile(a,	frame/1);	break;// 	rate=1; break;
					case A_AOE: 	fn=&_aframe_AOE; 		rate=2; break; //running+=!_aframe_AOE(a,		frame/2);	break;// 			rate=2; break;
					case A_SHOOTDIR:fn=&_aframe_shootdir;   rate=2; break; //running+=!_aframe_shootdir(a,	frame/2);	break;//rate=2; break;
					case A_LASER: 	fn=&_aframe_laser;      rate=1; break; //running+=!_aframe_laser(a,		frame/1);	break;//rate=1; break;
					case A_TWIRL: 	fn=&_aframe_twirl;      rate=3; break; //running+=!_aframe_twirl(a,		frame/3);	break;//rate=3; break;
					case A_SLICE: 	fn=&_aframe_slice;      rate=3; break; //running+=!_aframe_slice(a,		frame/3);	break;//rate=3; break;
				}
				sum++;

				int done=fn(a,frame/rate);
				if(done)
				{
					a->type=A_NONE;
				}
				else running=1;


			}
		}

		if(!sum) break;
		display();
		
		//if(a->colour) wattron(win,a->colour);
		for(int i=0;i<XMAX*YMAX;i++)
		{
			if(animation_layer[i] && db.tiles[i].flags&ML_VISIBLE)
			{
				wattron(win,wattr_layer[i]);
				wmove(win, i/XMAX, i%XMAX);
				waddch(win,animation_layer[i]);
				wattroff(win,wattr_layer[i]);
			}
		}
		//if(a->colour) wattroff(win,a->colour);
		wmove(win,0,0);
		wrefresh(win);
		usleep((int)(ANIM_RATE));
		frame++;
	}while(running);
	memset(animations,0,sizeof(animations));
	return 0;
}
*/
