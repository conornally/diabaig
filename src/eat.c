#include "diabaig.h"

static char *_goodeat[]={
	"that %s tasted good",
	"that %s tasted great",
	"that %s tasted amazing",
	"that %s tasted fantastic",
	"what a delicious %s",
	"what a tasty %s",
	"that was a tasty %s",
	"that was a yummy %s",
	"that was a good %s",
	"wow! what a fantastic %s",
	"this is the best %s i've ever eaten",
	NULL,
};
static char *_okeat[]={
	"that %s was ok",
	"that %s was a bit bland",
	"slimey %s, great",
	"%s again?",
	"isn't there something better than %s?",
	"maggotty %s..great",
	"i hope that %s doesn't make me sick",
	"this %s doesn't really taste of anything",
	"%s.. yummy",
	0
};
static char *_badeat[]={
	"that %s wasn't good",
	"that %s was bad",
	"that %s was awful",
	"that %s was a bit off",
	"bleghh! what an awful %s",
	"bleghh! what a bad %s",
	"this %s is off",
	"this %s is bad",
	"was that really a %s? it tasted like dirt",
	"i've eaten better things off the floor that this %s..",
	NULL,
};

static void _restore_hunger(float percent);

static void _speed(Entity *target, Entity *item);
static void _strength(Entity *target, Entity *item);
static void _regen(Entity *target, Entity *item);
static void _noeffect(Entity *target, Entity *item);
static void _poison(Entity *target, Entity *item);
static void _hallucinate(Entity *target, Entity *item);
static void _passout(Entity *target, Entity *item);

struct _effect {int chance; void (*func)(Entity *target, Entity *item); };
static struct _effect effects[]={
	{10,&_speed},
	{10,&_strength},
	{10,&_regen},
	{50,&_noeffect},
	{10,&_poison},
	{5,&_hallucinate},
	{5,&_passout},
	{0,NULL}
};

static struct _effect *_pick_effect()
{
	int total=0, _rng;
	struct _effect *effect= &effects[0];
	while(effect->func)
	{
		total+=effect->chance;
		effect++;
	}
	_rng=rng(total);
	total=0;

	effect= &effects[0];
	while(effect->func)
	{
		if( _rng>=total && _rng<(total+effect->chance) ) break;
		total+=effect->chance;
		effect++;
	}
	return effect;
}

static char *_pick_msg(char *msg_list[])
{
	int total=0;
	while(msg_list[total]) total++;
	return msg_list[rng(total)];
}



static void _restore_hunger(float percent)
{
	db.hunger= MIN(MAXHUNGER, db.hunger+ (int)(MAXHUNGER*percent));
}

static void _speed(Entity *target, Entity *item)
{
	if(target && item)
	{
		add_daemon(target,D_HASTE, 10+rng(10));
		msg(_pick_msg(_goodeat),"mushroom");
		_restore_hunger(1);
	}
}
static void _strength(Entity *target, Entity *item)
{
	if(target&&item)
	{
		target->_c.stat.str[1]++;
		add_daemon(target,D_STRENGTH, 10+rng(10));
		msg(_pick_msg(_goodeat),"mushroom");
		_restore_hunger(1);
	}
}
static void _regen(Entity *target, Entity *item)
{
	if(target&&item)
	{
		add_daemon(target,D_FASTREGEN, 10+rng(10));
		msg(_pick_msg(_goodeat),"mushroom");
		_restore_hunger(1);
	}
}

static void _noeffect(Entity *target, Entity *item)
{
	if(target&&item)
	{
		msg(_pick_msg(_okeat),"mushroom");
		_restore_hunger(1);
	}
}

static void _poison(Entity *target, Entity *item)
{
	if(target&&item)
	{
		add_daemon(target, D_POISON, 10+rng(10));
		msg(_pick_msg(_badeat),"mushroom");
		_restore_hunger(0.75);
	}
}

static void _hallucinate(Entity *target, Entity *item)
{
	if(target&&item)
	{
		add_daemon(target,D_LSD, 100);
		msg(_pick_msg(_badeat),"mushroom");
		_restore_hunger(0.5);
	}
}

static void _passout(Entity *target, Entity *item)
{
	if(target&&item)
	{
		add_daemon(target,D_SLEEP, 3+rng(15));
		msg(_pick_msg(_badeat),"mushroom");
		_restore_hunger(0.5);
	}
}
static void _eat_mushroom(Entity *target, Entity *item)
{
	struct _effect *effect;
	if(target&&item)
	{
		effect=_pick_effect();
		effect->func(target,item);
	}
}

static void _eat_slimemold(Entity *target, Entity *item)
{
	if(target && item)
	{
		_restore_hunger(0.65);
		msg(_pick_msg(_okeat),"slime-mold");
	}
}

int eat()
{
	int status=RETURN_UNDEF;
	Entity *item=menuselect(FOOD,"select item to eat");
	if(item && item->_o.type==FOOD)
	{
		status=_doeat(player,item);
	}
	else
	{
		msg("you can't eat that");
		status=RETURN_FAIL;
	}
	return status;
}

int _doeat(Entity *target, Entity *item)
{
	int status=RETURN_SUCCESS;
	if(!target || !item) return RETURN_FAIL;

	if(item->_o.quantity>1) item=split_stack(item);
	switch(item->_o.which)
	{
		case F_SLIMEMOLD: 	_eat_slimemold(target,item);break;
		case F_MUSHROOM: 	_eat_mushroom(target,item);	break;
		default: msg(_pick_msg(_goodeat),"ration"); _restore_hunger(1); break;
	}
	use(item);
	return status;
}

void faint()
{
	if( !(player->_c.flags&ISSLEEP) && !rng(10))
	{
		player->_c.flags |= ISSLEEP;
		add_daemon(player,D_SLEEP,rng(3)+1);
		msg("you faint from lack of food");

	}
}
void starve()
{
	player->_c.stat.hp=0;
	set_ripdata(RIP_STARVE,"to death");
}
