#include "diabaig.h"

void init_db()
{
	memset(&db,0, sizeof(struct _database));
	db.cur_level=0;
	memset(db.inventory, -1, sizeof(db.inventory));
	memset(db.spells, -1, sizeof(db.spells));
	db.cur_mainhand=-1;
	db.cur_offhand=-1;
	db.cur_armour=-1;
	db.cur_ringR=R_NORING;
	db.cur_ringL=R_NORING;
	db.quiver=-1;
	db.toggle_mainhand=-1;
	db.toggle_offhand=-1;

	db.xp=0;
	db.xplvl=0;
	db.hunger=MAXHUNGER;
	for(int i=0; i<NLEVELS; i++)
	{
		db.levels[i].seed=rand();
	}

	autopilot.active=false;
	db.frame=0;
	_log("init: database initialised: %ld bytes",sizeof(db));
}

int pick_one(obj_info *info, int nitems)
{
	int i,max=0, cur=0, rnd;
	for(i=0;i<nitems;i++) max+=info[i].prob;
	rnd=rng(max);
	for(i=0;i<nitems;i++)
	{
		cur+=info[i].prob;
		if(cur>=rnd) break;
	}
	return i;
}


static Entity* get_inactive(Entity *list, int nitems)
{
	int id=0;
	Entity *end;
	for(end=&list[nitems]; list<end; list++,id++)
	{
		if(!(list->flags & ISACTIVE))
		{
			memset(list,'\0',sizeof(Entity));
			list->id=id;
			if(id>nitems) _log("accessing database index greater than %s",nitems);
			break;
		}
	}
	if(list==end) list=NULL;
	return list;
}

void _set_weapon(Entity *e, int which)
{
	if(e && which>=0 &&  which<MAXWEAPON)
	{
		e->flags |= (ISACTIVE|ISOBJ); //just in case
		e->_o.type=WEAPON;
		e->_o.which=which;
		e->_o.flags=0;
		dmg_info info=init_weapon_info[e->_o.which];
		splitdice(info.melee_dmg, e->_o.mod_melee);
		splitdice(info.throw_dmg, e->_o.mod_throw);
		e->_o.mod_def=0;
		e->_o.flags |= info.flags;
		switch(which)
		{
			case DAGGER: e->_o.quantity=1+rng(5); break;
			case ARROW: e->_o.quantity=5+rng(20); break;
			case SHIELD: 		e->_o.mod_def=10; break;
			case TOWERSHIELD: 	e->_o.mod_def=15; break;
			default: e->_o.quantity=1; break;
		}
		if(e->_o.flags&CANSTACK) e->_o.enchant_level=0; //hmmm
	}
}

void _set_armour(Entity *e, int which)
{
	if(e && which>=0 &&  which<MAXARMOUR)
	{
		e->flags |= (ISACTIVE|ISOBJ); //just in case
		e->_o.type=ARMOUR;
		e->_o.which=which;
		e->_o.flags=0;
		e->_o.mod_def=def_info[e->_o.which][0];
		e->_o.mod_res=def_info[e->_o.which][1];
	}

}

void _set_potion(Entity *e, int which)
{
	if(e && which>=0 && which<MAXPOTION)
	{
		e->flags |= (ISACTIVE|ISOBJ);
		e->_o.type=POTION;
		e->_o.which=which;
		e->_o.flags=CANSTACK;
	}
}

void _set_scroll(Entity *e, int which)
{
	if(e && which>=0 && which<MAXSCROLL)
	{
		e->flags |= (ISACTIVE|ISOBJ);
		e->_o.type=SCROLL;
		e->_o.which=which;
		e->_o.flags=CANSTACK;
	}
}

void _set_ring(Entity *e, int which)
{
	if(e && which>=0 && which<MAXRINGS)
	{
		e->flags |= (ISACTIVE|ISOBJ);
		e->_o.type=RING;
		e->_o.which=which;
	}
}

void _set_gold(Entity* e, int which)
{
	if(e && which>=0 && which<2)
	{
		e->flags |= (ISACTIVE|ISOBJ);
		e->_o.type=GOLD;
		e->_o.which=which;
	}
}

void _set_food(Entity* e, int which)
{
	if(e && which>=0 && which<MAXFOOD)
	{
		e->flags |= (ISACTIVE|ISOBJ);
		e->_c.flags |= CANSTACK;
		e->_o.type=FOOD;
		e->_o.which=which;
	}
}

void _set_trinket(Entity *e, int which)
{
	if(e && which>=0 && which<MAXTRINKET)
	{
		e->flags |= (ISACTIVE|ISOBJ);
		e->_o.type=TRINKET;
		e->_o.which=which;
		e->_o.flags=CANSTACK;
	}
}

void set_item(Entity *e, int which)
{
	if(e)
	{
		switch(e->_o.type)
		{
			case WEAPON: _set_weapon(e,which); break;
			case ARMOUR: _set_armour(e,which); break;
			case SCROLL: _set_scroll(e,which); break;
			case POTION: _set_potion(e,which); break;
			case FOOD: 	 _set_food(e,which); break;
			case TRINKET: _set_trinket(e,which); break;
			case RING: _set_ring(e,which); break;
			case GOLD: _set_gold(e,which); break;
		}
	}
}

Entity *_new_obj(int type)
{
	Entity *e=get_inactive(db.objects,DBSIZE_OBJECTS);
	if(e)
	{
		e->flags |= (ISACTIVE|ISOBJ);
		e->_o.quantity=1;
		switch(type)
		{
			case POTION: //potion
				_set_potion(e,pick_one(potion_info,MAXPOTION));
				break;
			case SCROLL: //scroll
				_set_scroll(e,pick_one(potion_info,MAXSCROLL));
				//e->_o.type=SCROLL;
				//e->_o.which=pick_one(scroll_info,MAXSCROLL);
				//e->_o.flags |= CANSTACK;
				break;
			case FOOD: //food
				_set_food(e,pick_one(food_info, MAXFOOD));
				//e->_o.type=FOOD;
				//e->_o.which=pick_one(food_info,MAXFOOD);
				//e->_o.flags |= CANSTACK;
				break;
			case GOLD: //gold
				//e->_o.type=GOLD;
				_set_gold(e,pick_one(gold_info,2));
				if(e->_o.which==0) e->_o.quantity= 1+rng(10);
				if(e->_o.which==1) e->_o.quantity= 20+rng(50);
				break;
			case TRINKET: 
				_set_trinket(e,pick_one(trinket_info,MAXTRINKET));
				break;

			case WEAPON: //weapon
				_set_weapon(e,pick_one(weapon_info,MAXWEAPON));
				if(!rng(ENCHANTRATE) && !(e->_o.flags&CANSTACK)) enchant(e);
				else if(!rng(DISENCHANTRATE) && !(e->_o.flags&CANSTACK)) disenchant(e);
				//if(!rng(CURSERATE)) e->_o.flags |= ISCURSED;
				break;
			case ARMOUR: //armour
				_set_armour(e,pick_one(armour_info,MAXARMOUR));
				if(!rng(ENCHANTRATE) && !(e->_o.flags&CANSTACK)) enchant(e);
				else if(!rng(DISENCHANTRATE) && !(e->_o.flags&CANSTACK)) disenchant(e);
				//if(!rng(CURSERATE)) e->_o.flags |= ISCURSED;
				break;
			case RING:
				_set_ring(e,pick_one(ring_info, MAXRINGS));
				if(!rng(CURSERATE)) e->_o.flags |= ISCURSED;
				break;

			default:
				clear_entity(e);
				e=NULL;
				break;
		}
	}
	return e;
}
Entity *new_obj()
{
	//Spawn a completely random object into DB
	int type=pick_one(type_info,MAXOBJTYPES);
	Entity *e;
	switch(type)
	{
		case 0: e=_new_obj(POTION); break;
		case 1: e=_new_obj(SCROLL); break;
		case 2: e=_new_obj(FOOD); break;
		case 3: e=_new_obj(GOLD); break;
		case 4: e=_new_obj(WEAPON); break;
		case 5: e=_new_obj(ARMOUR); break;
		case 6: e=_new_obj(RING); break;
		case 7: e=_new_obj(TRINKET); break;
		default: e=NULL; _log("BAAAD");break;
	}
	return e;
}

Entity *_new_monster(int type)
{
	Entity *e=NULL;
	int mimic_disguises[]={POTION,SCROLL,FOOD,WEAPON,ARMOUR};
	if(type>='A' && type<='z')
	{
		e=get_inactive(db.creatures, DBSIZE_CREATURES);
		monster_info *ptr=&monsters[type-'A'];
		if(e)
		{
			e->flags|=(ISACTIVE|ISCREATURE);	

			e->_c.flags=ptr->flags;
			e->_c.type=type;//ptr->monster_name[0];
			//e->_c.disguise=e->_c.type;
			e->_c.form=DEFAULTFORM;
			e->_c.stat= (stats){ ptr->hp, ptr->hp,{0,0} , ptr->dex, ptr->def, ptr->res};
			splitdice(ptr->str, e->_c.stat.str);
			e->_c.turn=0;
			e->_c.res_flags=ptr->res_flags;

			switch(type)
			{
				case 'c': ptr->prob=0; break; // save load will not keep this..
				case 'g':
					if(db.cur_level>3)
					{
						int weights[4]={50,20,10,10}; //gNORMAL,gPIKEMAN,gSHIELDBEARER,gCUTTHROAT
						int form=weighted_pick(weights,4);
						e->_c.form=form;
						_log("GOBLIN:%d",form);
						switch(form)
						{
							case gPIKEMAN:
								e->_c.stat.str[0]=2;
								e->_c.stat.str[1]=5;
								break;
							case gSHIELDBEARER:
								e->_c.stat.def=10;
								e->_c.flags|=ISSLOW;
								break;
							case gCUTTHROAT:
								e->_c.stat.dex=12;
								e->_c.flags|=ISSPEED;
								break;
						}
					}
					break;
				case 'l':
					{ //thats just so i can define weights and form
					int weights[5]={30,20,20,20,10}; //lNORMAL,lMARAUDER,lBERSERKER,lGRUNT,lLIEUTENANT
					int form=weighted_pick(weights,5);
					e->_c.form=form;
					_log("LIZRDFOLK:%d",form);
					switch(form)
					{
						case lMARAUDER:
							e->_c.stat.def+=5;
							break;
						case lBERSERKER:
							e->_c.stat.str[0]++;
							e->_c.stat.str[1]++;
							e->_c.stat.def-=5;
							break;
						case lGRUNT:
							e->_c.stat.hp-=10;
							e->_c.stat.maxhp-=10;
							break;
						case lLIEUTENANT:
							e->_c.stat.hp+=15;
							e->_c.stat.maxhp+=15;
							e->_c.stat.def+=5;
							break;
					}
					break;
					}
				case 'r':
					{
					int types[]=  {'b','e','f','g','h','j','k','l','o','q','s','t','u','v','y',0 };
					int weights[]={  5,  5,  5, 10, 10, 15, 10, 10, 15, 15,  5, 10, 10, 5, 10 ,0 }; 
					e->_c.form= types[weighted_pick(weights,len(types))];
					ptr=&monsters[e->_c.form-'A'];
					e->_c.stat= (stats){ ptr->hp, ptr->hp,{0,0} , ptr->dex, ptr->def, ptr->res};
					splitdice(ptr->str, e->_c.stat.str);
					e->_c.stat.maxhp*=1.5;
					e->_c.stat.hp=e->_c.stat.maxhp;
					e->_c.stat.def	*=1.5;
					e->_c.res_flags |= IMMUNE_POISON;
					break;
					}

				case 't':
					switch(rng(6))
					{
						case 0:case 1:case 2: e->_c.form=tNORAML; break;
						case 3:
							e->_c.form=tFIRE;
							e->_c.res_flags |= (RESIST_FIRE|WEAKTO_FROST);
							break;
						case 4:
							e->_c.form=tFROST;
							e->_c.res_flags |= (RESIST_FROST|WEAKTO_FIRE);
							break;
						case 5:
							e->_c.form=tROT;
							e->_c.res_flags |= (IMMUNE_POISON|WEAKTO_FIRE|WEAKTO_FROST);
							e->_c.flags &= ~ ISREGEN;
							break;
					}
					break;
				case 'm': e->_c.form=mimic_disguises[rng(5)]; break;
				case 'v': e->_c.form=vBAT; e->_c.flags|=ISFLY; break;
				case 'L': break;
				case 'H': e->_c.stat.str[0] += rng(3); break;
				case 'D':
						  e->_c.form=rng(3);
						  break;
                case 'R':
                          break;

			}
			/*
			_log("new %c %d %dd%d %d %d %d", e->_c.type,
												e->_c.stat.hp,
												e->_c.stat.str[0],
												e->_c.stat.str[1],
												e->_c.stat.dex,
												e->_c.stat.def,
												e->_c.stat.res);
												*/
		}
	} 
	return e;
}

Entity *new_monster()
{
	int i,max;
	Entity *e=NULL;
	monster_info *ptr, *end;
	//ptr=monsters;
	ptr=&monsters[rng(MAXMONSTERS)]; //start at a random place in the list // in an atempt to add more randomness
	for(end=&monsters[MAXMONSTERS]; ptr<end; ptr++)
	{
		if(ptr->level<=get_efflevel())//db.cur_level) //and later do a high check
		{
			max=100 + (15*(get_efflevel()-ptr->level)); //+15 for every level below "its level"
			i=rng(max);
			if(i<ptr->prob)
			{
				e=_new_monster(ptr->monster_name[0]);
				break;
			}
		}
		if(ptr==&monsters[MAXMONSTERS-1]) ptr=monsters;
	}
	return e;
}

Entity *new_player()
{
	Entity *e=get_inactive(db.creatures,DBSIZE_CREATURES);
	monster_info *ptr=&player_info;
	if(e)
	{
		e->_c.type='@';
		e->flags |= (ISACTIVE|ISCREATURE);
		e->_c.flags |= ptr->flags;
		e->_c.type=ptr->monster_name[0];
		e->_c.stat= (stats){ ptr->hp, ptr->hp,{0,0} , ptr->dex, ptr->def, ptr->res};
		e->_c.res_flags=ptr->res_flags;
		splitdice(ptr->str, e->_c.stat.str);
		add_daemon(e,D_REGEN,REGENDURATION);
	}
	_log("player created %d",e->id);
	return e;
}

void clear_entity(Entity *e)
{
	int id;
	Entity *list=NULL;
	int x,y;
	if(e)
	{
		id=e->id;
		x=e->pos.x;
		y=e->pos.y;
		if(e->flags & ISOBJ) list=db.objects;
		if(e->flags & ISCREATURE) list=db.creatures;
		if(objat(x,y)==e) objat(x,y)=NULL;
		if(moat(x,y)==e) moat(x,y)=NULL;
		if(list) memset(&list[id],'\0', sizeof(Entity));
	}
}

Entity *split_stack(Entity *item)
{
	if(item)
	{
		Entity *e=get_inactive(db.objects, DBSIZE_OBJECTS);
		if(e)
		{
			int id=e->id;
			memcpy(e,item,sizeof(Entity));
			e->_o.quantity=1;
			e->id=id;
			item->_o.quantity--;
			item->_o.potion_effect[1]--;
		}
	return e;
	}
	else return NULL;
}

/*
tile* tileat(int x, int y)
{
	tile* t=NULL;
	if(x>=0 && x<XMAX && y>=0 && y<YMAX) t=&db.tiles[y*XMAX + x];

	return t;
}

Entity* moat(int x, int y)
{
	Entity* e=NULL;
	tile* t=tileat(x,y);
	if(t) e=t->creature;

	return e;
}

Entity* objat(int x, int y)
{
	Entity* e=NULL;
	tile* t=tileat(x,y);
	if(t) e=t->object;

	return e;
}
*/
