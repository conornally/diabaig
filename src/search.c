#include "diabaig.h"
static void _reveal_mimic(Entity *e);
static void _reveal_illusion(Entity *e);
static void _reveal_vampire(Entity* e);

int search()
{
	//tile *t;
	Entity *c, *o;
	for(int x=player->pos.x-1; x<=player->pos.x+1;x++)
	for(int y=player->pos.y-1; y<=player->pos.y+1;y++)
	{
		//t=tileat(x,y);
		o=objat(x,y);
		c=moat(x,y);
		tflags(x,y) &= (~MS_HIDDEN);

		if(o)
		{
			msg("you search and find %s",getname(o));
		}

		if(c)
		{
			switch(c->_c.type)
			{
				case 'm':
					_reveal_mimic(c);
					break;
				case 'I':
					 _reveal_illusion(c);
					 break;
				case 'v':
					 _reveal_vampire(c);
					 break;
				case '@': break;
				default:
					 msg("you search and find %s", getname(c));
					 break;
			}
		}
	}
	return RETURN_SUCCESS;
}

static void _reveal_mimic(Entity *e)
{
	char disguise[12];
	if(e)
	{
		switch(e->_c.form)
		{
			case POTION: sprintf(disguise, "a potion"); break;
			case SCROLL: sprintf(disguise, "a scroll"); break;
			case FOOD: sprintf(disguise, "some food"); break;
			case GOLD: sprintf(disguise, "gold"); break;
			case WEAPON: sprintf(disguise, "a weapon"); break;
			case ARMOUR: sprintf(disguise, "some armour"); break;
			case RING: sprintf(disguise, "a ring"); break;
		}
		msg("that not %s, it's a mimic!",disguise);
		e->_c.form=0;
	}
}

static void _reveal_illusion(Entity *e)
{
	char *lst[]={
		"%s is an illusion!",
		"%s is an illusion! it vanishes",
		"%s vanishes",
		"%s illusion vanishes",
		"%s fades before your eyes",
		"%s fades before your eyes, an illusion!",
		0
	};
	msg(lst[ rng(clen(lst)) ], getname(e));
	clear_entity(e);
}

static void _reveal_vampire(Entity* e)
{
	if(e) msg("this bat has strangely large teeth");
}
