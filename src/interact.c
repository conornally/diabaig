#include "diabaig.h"

static int _interact_cat(Entity* e, Entity* target);
static int _interact_raggle(Entity* e, Entity* target);
static int _interact_Xolotl(Entity* e, Entity* target);

int interact(Entity* e, Entity* target)
{
	if(e && target)
	{
		switch( target->_c.type )
		{
			case 'c': _interact_cat(e,target); break;
			case 'R': _interact_raggle(e,target); break;
			case 'X': _interact_Xolotl(e,target); break;
			default: break;
		}
	}
	return RETURN_SUCCESS;
}


static int _interact_cat(Entity* e, Entity* target)
{
	char * lst[]={
		"%s strokes %s",
		"%s bends down and strokes %s",
		"%s gives some fuss to %s",
		"%s pats the head of %s",
		"%s tries to get the annention of %s",
		"%s says \"pss-pss-pss\" to %s, it is uninterested",
		"%s tries to stroke %s, it swerves out of the way",
		"%s scratches behind %s's ear",
		"%s plays with %s",

		0};
	char* name=strdup(getname(e));
	msg(lst[ rng(clen(lst))], name, getname(target));
	free(name);

	return 0;
}

void regen_shop(int num)
{
    int types[]=    {SCROLL,POTION,RING,ARMOUR,WEAPON,FOOD};
    int weights[]=  {5,10,2,1,1,3};
    int prices[]=   {50, 25, 200, 100, 100, 25};

    for(int i=0;i<MAXSHOP;i++)
    { // CLEAR ANY CURRENT ITEMS
        if(db.shop[i].id>=0 && db.shop[i].id<DBSIZE_OBJECTS) clear_entity(&db.objects[ db.shop[i].id ]);
    }

    memset(db.shop, -1, sizeof(db.shop));
    for(int i=0; i<MIN(num,MAXSHOP); i++)
    {
        int id=weighted_pick(weights, 6);
        int type=types[id];
        int which=rng(getmax(type));
        obj_info info=getinfo(type)[which];

        Entity *e=_new_obj(type);
        switch(type)
        {
            case POTION: _set_potion(e, which); break;
            case SCROLL: _set_scroll(e, which); break;
            case WEAPON: 
                _set_weapon(e, which); 
                if(which==ARROW) e->_o.quantity=10+rng(10);
                if(which==DAGGER) e->_o.quantity=2+rng(5);
                break;
            case ARMOUR: _set_armour(e, which); break;
            case RING: _set_ring(e, which);     break;
            case FOOD: e->_o.which=F_RATION;    break;
        }

        db.shop[i].id=e->id;
        db.shop[i].ident= ( info.known || rng(2)); //50/50 chance if it is not known
        db.shop[i].price= prices[id]+ (50-2*info.prob); //not so great

		if(db.buy[db.cur_level][i]) 
		{
			clear_entity(e);
			db.shop[i].id=-1;
		}
    }
}

static int _interact_raggle(Entity *e, Entity *target)
{
	int id;
    Entity *item;
    shop_item *shop;
    char entry[NCOLS];

	char * welcomelst[]={
		"\"want to see my wares, they are 'ethically' sourced\"",
		"\"want to see my wares, no one came to much harm to get them\"",
		"\"i took all these from the last guys body after they met their demise\"",
		"\"i took all these from the last guys body after they met a quaggoth\"",
		"\"you don't want to know where i found all this stuff\"",
		"\"i won't tell you where i found all this stuff\"",
		"\"you can make a lot of money down here, if you avoid the goblins\"",
		"\"if you die in here, ill sell your stuff to the next adventurer\"",
		0};
	char * buylst[]={
		"%s grins, \"a pleasure doing business with you\"",
		"%s pockets the gold, \"if you die, i will take that back\"",
		"%s pockets the gold, \"there is plenty more where that came from\"",
		"%s eyes up your gold, \"need anything else?\"",
		0 };

	char * poorlst[]={
		"\"you don't have enough for that\" hisses %s",
		"%s snarls \"what are you trying to pull, this is a quality item\"",
		"%s looks at you distastefully, \"no gold, no goods!\"",
		"\"come back with more money\", says %s",
		"\"get out of here you skint begger\", says %s",
		"\"i'm not offering handouts, come back with gold\", says %s",
		"%s growls \"i'm not interested unless you have money\"",
		0};

    display_dathead(res_shop_txt, res_shop_txt_len);
    wmove(win, 19, 4);
	wprintw(win,"Raggle grins at you and says:");
    wmove(win, 20, 4);
	waddstr(win,welcomelst[ rng(clen(welcomelst)) ]);

    wmove(win, 22, 0);
    for(int i=0; i<MAXSHOP; i++)
    {
		wmove(win,22+i, 5);
        shop=&db.shop[i];
        if(shop->id>=0 && shop->id<DBSIZE_OBJECTS)
        {
            item=&db.objects[ shop->id ];
            int known= getinfo( item->_o.type )[item->_o.which].known;

            getinfo( item->_o.type )[item->_o.which].known=shop->ident;
            sprintf(entry, "%c) [%d gold] %s", 'a'+i, shop->price, getname(&db.objects[shop->id]));
            //wprintw(win,"  | %-71s|\n",entry);
            waddstr(win,entry);
            getinfo( item->_o.type )[item->_o.which].known=known; //set the global ident back
        }
		else waddstr(win,"   --- [ SOLD OUT ]---");
    }
	wmove(win,NROWS-3,5);
	wprintw(win,"you have %d gold to spend",db.gold);
    wborder(win,0,0,0,0,0,0,0,0);
	display_frameheader("Raggle's \"Ethically Sourced\" Wares");
    wrefresh(win);

	int input=getch();
	if(input>='a' && input<('a'+MAXSHOP))
	{
		shop=&db.shop[input-'a'];
		id=shop->id;
		if(id>=0 && id<DBSIZE_OBJECTS)
		{
			if( db.gold>=shop->price) 
			{
				item=&db.objects[id];
				getinfo( item->_o.type )[item->_o.which].known=shop->ident; //set the global ident back
				objat(e->pos.x,e->pos.y)=item;
				msg(buylst[rng(clen(buylst))],getname(target));
				pickup();

				db.buy[db.cur_level][input-'a']=1;
				shop->id=-1;
				db.gold-=shop->price;

				//for(int i=0; i<(MAXSHOP-1); i++)
				//{
				//	if((db.shop[i].id=-1)&&(db.shop[i+1].id>=0))
				//	{
				//		memcpy(&db.shop[i],&db.shop[i+1],sizeof(shop_item));
				//		memset(&db.shop[i+1],-1,sizeof(shop_item));

				//		db.buy[db.cur_level][i]=db.buy[db.cur_level][i+1];
				//		db.buy[db.cur_level][i+1]=0;
				//	}
				//}

			}
			else msg(poorlst[rng(clen(poorlst))],getname(target));
		}
	}
	else msg("invalid selection");
    return 1;




}

static int _interact_Xolotl(Entity* e, Entity* target)
{

	int input;
	if(e && target)
	{
		display_dathead(res_xolotl_txt, res_xolotl_txt_len);
		display_frameheader("Xolotl's Trade Offer");

		input=wgetch(win);
		if(input=='a')
		{
			player->_c.flags|=ISREBIRTH;
			msg("ok");
		}
	}
	return 0;
}
