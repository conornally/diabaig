#include "diabaig.h"

static void notimplemented(char *s)
{
	msg("%s not yet implemented",s);
}

static void _potion_healing(Entity *item, Entity *target)
{
	int val, tmphp;

	if(item && target)
	{
		if(item->_o.which==P_HEALING) 		val=10+rng(10);
		if(item->_o.which==P_HEALINGPLUS) 	val=30+rng(30);

		tmphp=target->_c.stat.hp+val;
		target->_c.stat.hp= MIN(tmphp, target->_c.stat.maxhp);
		if(target==player) msg("you feel much better");
		else msg("%s has restored its health",getname(target));
	}
}

static void _potion_poison(Entity *item, Entity *target)
{
	if(item && target)
	{
		if(target->_c.flags & RESIST_POISON) msg("the potion has no effect");
		else
		{
			add_daemon(target, D_POISON, 20);
			if(target==player)
			{
				msg("you feel very sick");
				//set_ripdata(RIP_DRANK,"potion of harming");
			}
			else msg("%s is poisoned",getname(target));
		}
	}

}

static void _potion_confusion(Entity *item, Entity *target)
{
	if(item && target)
	{
		target->_c.flags |= ISCONFUSED;
		add_daemon(target, D_CONFUSE, CONFUSEDURATION);
		if(target==player) msg("woah where am i?");
		else msg("the %s becomes disorientated",getname(target));
	}
}

static void _potion_invisibility(Entity *item, Entity *target)
{
	if(item && target)
	{
		target->_c.flags |= ISINVIS;
		add_daemon(target, D_INVIS, INVISDURATION);
		if(target==player) msg("your body fades away. you are invisible");
		else msg("poof! %s vanished",getname(target));
	}
}

static void _potion_dex(Entity *item, Entity *target)
{
	if(item && target)
	{
		target->_c.stat.dex++;	
		if(target==player) msg("you feel more nimble");
		else msg("%s looks more nimble",getname(target));
	}
}

static void _potion_str(Entity *item, Entity *target)
{
	if(item && target)
	{
		target->_c.stat.str[1]++;
		add_daemon(target,D_STRENGTH,100);

		if(target==player) msg("bulging muscles! you feel stronger");
		else msg("the muscles on the %s bulge",getname(target));
	}
}

static void _potion_elixir(Entity *item, Entity *target)
{
	//add much more stuff later
	stats *s;
	if(item && target)
	{
		s=&target->_c.stat;
		s->maxhp += 10+rng(10);
		s->hp=s->maxhp;
		if(target==player)
		{
			msg("you feel fantastic!");
			db.hunger=MAXHUNGER;
		}
		else msg("ooh that was a mistake..");
	}
}

static void _potion_haste(Entity *item, Entity *target)
{
	if(item && target)
	{
		add_daemon(target,D_HASTE,100);
		if(target==player) msg("the world moves slowly, your movement speed increases");
		else msg("suddenly %s movements are much faster",getname(target));
	}
}

static void _potion_slow(Entity *item, Entity *target)
{
	if(item && target)
	{
		add_daemon(target,D_SLOW,50);
		if(target==player) msg("the world moves quickly, your movement speed decreases");
		else msg("suddenly %s movements are much slower",getname(target));
	}
}

static void _potion_sleep(Entity *item, Entity *target)
{
	if(item && target)
	{
		if(target->_c.res_flags & IMMUNE_SLEEP) 
		{
			msg("%s resists the tranquillizing effects",getname(target));
			return;
		}
		target->_c.flags|=ISSLEEP;
		add_daemon(target,D_SLEEP,50);
		if(target==player)
		{
			msg("you fall asleep");
		}
		else msg("%s falls alseep",getname(target));
	}
}

static void _potion_blindness(Entity *item, Entity *target)
{
	if(item && target)
	{
		int duration=50;
		if(target==player)
		{
			msg("you suddenly lose your eyesight");
			duration=200;
		}
		else msg("%s begins to stagger, its blind!",getname(target));

		add_daemon(target,D_BLIND,duration);
	}
}
static void _potion_burning(Entity *item, Entity *target)
{
	if(item && target)
	{
		int duration=25;
		char *t_name=strdup(getname(target));
		if(target==player)
		{
			msg("seering pain! your insides are on fire");
			duration=15;
		}
		else msg("%s splashes over %s, it goes up in flames!",getname(item),t_name);
		add_daemon(target,D_BURN,duration);
		free(t_name);
	}
}
static void _potion_inking(Entity *item, Entity *target)
{
	if(item && target)
	{
		int duration=100;
		char *t_name=strdup(getname(target));
		if(target==player)
		{
			char *a_name= (db.cur_armour!=-1)?armour_info[db.objects[db.cur_armour]._o.which].obj_name:"top";
			msg("oops! you spill %s down your %s",getname(item),a_name);
			duration=50;
		}
		else msg("%s splashes over the body of %s",getname(item),t_name);
		add_daemon(target,D_INK,duration);
		free(t_name);
	}
}

static void _potion_smokebomb(Entity *item, Entity *target)
{
	if(item && target)
	{
		if(target==player) msg("the potion fizzes inside you, smoke comes out your ears");
		tileat(target->pos.x,target->pos.y)->air=SMOKE;
		tileat(target->pos.x,target->pos.y)->air_pressure=1.0;
	}
}

int drink()
{
	int status=RETURN_UNDEF;
	Entity *e=menuselect(POTION, "select potion to drink");
	if(e && e->_o.type==POTION)
	{
		status=_dodrink(player,e);
	}
	else status=RETURN_FAIL;
	return status;
}

int _dodrink(Entity *target, Entity *item)
{
	int status=RETURN_SUCCESS;
	if(!target || ! item) return RETURN_FAIL;
	if(item->_o.quantity>1) item=split_stack(item);
	switch( item->_o.which)
	{
		case P_HEALING :  //deliberate fall through
		case P_HEALINGPLUS:
			_potion_healing(item,target);
			break;
		case P_CONFUSION: _potion_confusion(item,target); break;
		case P_POISON: _potion_poison(item,target); break;
		case P_INVISIBILITY: _potion_invisibility(item,target); break;
		case P_DEX: _potion_dex(item,target); break;
		case P_ELIXIR: _potion_elixir(item,target);break;
		case P_STR: _potion_str(item,target);break;
		case P_HASTE: _potion_haste(item,target);break;
		case P_SLOWNESS: _potion_slow(item,target);break;
		case P_SLEEP: _potion_sleep(item,target);break;
		case P_BLINDNESS: _potion_blindness(item,target);break;
		case P_BURNING: _potion_burning(item,target);break;
		case P_INK: _potion_inking(item,target); break;
		case P_SMOKEBOMB: _potion_smokebomb(item,target); break;
		default :
		{
			obj_info info=potion_info[item->_o.which];
			notimplemented( info.obj_name);
			status=RETURN_FAIL;
			break;
		}
	}
	use(item);
	return status;
}
