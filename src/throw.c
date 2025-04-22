#include "diabaig.h"

static const char *_throwhit[3]={
	"%s hits %s",
	"%s whistles through the air and hits %s",
	"bulls eye! %s hits %s"
};

static void _ranged_hit(Entity* e, Entity *target, Entity *item)
{
	if(e && target && item)
	{
		_daemon *d=search_daemon(target,D_SLEEP);
		if(d)
		{
			target->_c.flags &= (~ISSLEEP);
			d->time=1;
		}

		_do_weapon_effect(item,target,0);
		if(e==player)
		{
			char *name1=strdup(getname(item));
			char *name2=getname(target);
			msg(_throwhit[rng(3)],name1, name2);

			if(target->_c.stat.hp<=0) murder(e,target);
			free(name1);
		}
	}
}
static void _throw(Entity *e, Entity *item, int dir)
{
	int x=-1,y=-1;
	tile *t=NULL;
	Entity *target=NULL;
	int id=get_first_thing_direction(e->pos,dir);

	if(id>=0 && id<(XMAX*YMAX))
	{
		t=&db.tiles[id];
		target=t->creature;
		x=id%XMAX;
		y=id/XMAX;
	}

	item->pos.x=x;
	item->pos.y=y;
	item->pos.z=e->pos.z;
	animation a=(animation){A_PROJ,item->_o.type,COLOR_PAIR(C_WHITE),player->pos,{x,y,0},0};
	//a.pos=(coord){e->pos.x,e->pos.y};
	//a.target_pos=(coord){x,y};
	//add_animation(a);
	animate(&a);

	int air=AIR, which=item->_o.which;
	float pressure=0;
	switch(item->_o.type)
	{
		case POTION: 
			switch(which)
			{
				case P_BURNING: air=COMBUST; pressure=1.5; break;
				case P_POISON:  air=MIASMA;  pressure=1.5;break;
				case P_SMOKEBOMB:   air=SMOKE; pressure=5; break;
				default: break;
			}
			t->air=air;
			t->air_pressure+=pressure;

			if(target) _dodrink(target,item);
			else msg("%s smashes against the wall",getname(item));
			item->_o.quantity--; 
			if(air!=AIR) potion_info[which].known=1;
			break; 
					
		case WEAPON:
			 if(target)
			 {
				 _ranged_hit(e, target,item);
				 if(item->_o.flags&CANSTACK) item->_o.quantity--;
			 }
			 break;
		default: break;
	}

	if(target && e==player)
	{
		if(target->_c.flags & ISFRIEND) target->_c.flags |= ISSCARED;
		else target->_c.flags |= (ISAGRO|ISFOLLOW);
	}

	Entity *tmp=objat(item->pos.x,item->pos.y);
	if(tmp && (tmp!=item) && (tmp->_o.type==item->_o.type) && (tmp->_o.which==item->_o.which) && (item->_o.flags&CANSTACK))
	{
		tmp->_o.quantity+=item->_o.quantity;
		item->_o.quantity=0;
	}

	//msg("debug: %d %d %d",x,y,item->_o.quantity);
}

int throw_item()
{
	int status=RETURN_UNDEF;
	int direction;
	Entity *stack=menuselect(0,"select item to throw");
	if(stack)
	{
		direction=pick_direction();
		if(direction!=nodir)
		{
			_throw(player,split_stack(stack),direction);
			status=RETURN_SUCCESS;
		}
		else
		{
			msg("invalid direction");
			status=RETURN_FAIL;
		}
	}
	else status=RETURN_FAIL;
	return status;
}

int fire_bow()
{
	int status=RETURN_UNDEF;
	Entity *arrow=NULL, *arrows=NULL, *bow=NULL;

	if(db.cur_mainhand!=-1) bow=&db.objects[db.cur_mainhand];

	//if(db.cur_mainhand!=-1 && ( (bow=&db.objects[db.cur_mainhand])->_o.which==BOW || bow->_o.which==LONGBOW || bow->_o.which==RECURVEBOW ) )
	if(bow && (bow->_o.which==SHORTBOW || bow->_o.which==LONGBOW || bow->_o.which==RECURVEBOW) )
	{
		//bow=&db.objects[db.cur_weapon]; // i dont actually need this
		for(int i=0;i<26;i++)
		{
			if(db.inventory[i]!=-1 && db.objects[db.inventory[i]]._o.type==WEAPON && db.objects[db.inventory[i]]._o.which==ARROW)
			{
				arrows=&db.objects[db.inventory[i]];
				break;
			}
		}
		if(arrows)
		{
			//i can modify arrow damage here
			int direction=pick_direction();
			if(direction!=nodir)
			{
				arrow=split_stack(arrows);
				int mod=bow->_o.enchant_level;

				switch(bow->_o.which)
				{
					case SHORTBOW: 		mod+=1; break;
					case LONGBOW:	mod+=2; break;
					case RECURVEBOW: 	mod+=3; break;
				}
				arrow->_o.mod_throw[0]+=mod;
				//arrow->_o.mod_throw[0]++; //double damage when using bow
				_throw(player,arrow,direction);
				status=RETURN_SUCCESS;
			}
			else
			{
				msg("invalid direction");
				status=RETURN_FAIL;
			}
		}
		else
		{
			msg("you have no arrows");
			status=RETURN_FAIL;
		}
	}
	else
	{
		msg("you must have a bow equipped");
		status=RETURN_FAIL;
	}
	return status;
}
