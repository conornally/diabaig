#include "diabaig.h"

static int _equip_select()
{
	int id;
	Entity *tmp;

	for(int i=0;i<26;i++)
	{
		if((id=db.inventory[i])!=-1)
		{
			tmp=&db.objects[id];
			if(tmp->_o.type==WEAPON || tmp->_o.type==ARMOUR || tmp->_o.type==RING)
			{
				wprintw(win,"%c) %s ",i+'a', getname(tmp));
				if(tmp->id==db.cur_mainhand) wprintw(win,"(in mainhand) ");
				if(tmp->id==db.cur_offhand) wprintw(win,"(in offhand) ");
				if(tmp->id==db.cur_armour) wprintw(win,"(being worn) ");
				if(tmp->id==db.cur_ringR) wprintw(win,"(worn on right hand) ");
				if(tmp->id==db.cur_ringL) wprintw(win,"(worn on left hand) ");
				if(tmp->id==db.toggle_mainhand) wprintw(win,"(quick mainhand) ");
				if(tmp->id==db.toggle_offhand) wprintw(win,"(quick offhand) ");
				waddch(win,'\n');
			}
		}
	}
	wprintw(win,"select a piece of equipment:");
	wrefresh(win);
	int sel=getch()-'a';
	if(sel>=0 && sel<26 && (id=db.inventory[sel])!=-1)
	{
		return id;
	}
	else return -1;
}

static int _do_equip_weapon(Entity *item, int *slot)
{
	if(!item) return RETURN_FAIL;

	int status=RETURN_UNDEF;
	int id=item->id;
	int type=item->_o.type;
	//int which=item->_o.which;

	switch(type)
	{
		case WEAPON:
			_do_dequip(id);
			if(item->_o.flags & W_TWOHAND && slot!=&db.cur_offhand) //can only equip it to mainhand in isolation
			{
				db.toggle_mainhand=db.cur_mainhand; //save "snapshot"
				db.toggle_offhand=db.cur_offhand;
				_do_dequip(db.cur_mainhand);
				_do_dequip(db.cur_offhand);
				db.cur_mainhand=id;
				db.cur_offhand=id;
				status=RETURN_SUCCESS;
			}
			else if(item->_o.flags&W_ONEHAND)
			{
				if(slot==&db.cur_mainhand) 
				{
					//if(db.cur_mainhand!=-1) db.toggle_mainhand=db.cur_mainhand; //save "snapshot"
					_do_dequip(db.cur_mainhand);
					db.cur_mainhand=id;
					status=RETURN_SUCCESS;
				}
				if(slot==&db.cur_offhand) 
				{
					//if(db.cur_offhand!=-1) db.toggle_offhand=db.cur_offhand;
					_do_dequip(db.cur_offhand);
					db.cur_offhand=id;
					status=RETURN_SUCCESS;
				}
			}
			else if(item->_o.flags&W_ARROW)
			{
				db.quiver=id;
				status=RETURN_SUCCESS;
			}


			break;
		case ARMOUR:
			break;
		case RING:
			break;

		default: break;
	}
	return status;

}

int equip()
{
	wclear(win);
	wrefresh(win);
	wprintw(win,"equip item:\n");
	int id=_equip_select();
	return _equip(id);
}
int _equip(int id)
{
	int status=RETURN_UNDEF;
	Entity *item;
	int *slot=NULL;
	wclear(win);
	wrefresh(win);
	if(id!=-1)
	{
		item=&db.objects[id];
		switch(item->_o.type)
		{
			case WEAPON:
				if(item->_o.flags&W_TWOHAND) slot=&db.cur_mainhand;
				if(item->_o.flags&W_ONEHAND) 
				{
					wprintw(win,"\nequip %s to mainhand or offhand (m/o):",getname(item));
					wrefresh(win);
					switch(getch())
					{
						case 'm': case 'M': slot=&db.cur_mainhand; break;
						case 'o': case 'O': slot=&db.cur_offhand; break;
					}
				}
				if(item->_o.flags&W_ARROW) slot=&db.quiver;
				status=_do_equip_weapon(item,slot);
				break;

			case ARMOUR:
				if(db.cur_armour!=-1) _do_dequip(db.cur_armour);
				db.cur_armour=id;
				status=RETURN_SUCCESS;
				break;

			case RING:
				//unequip if doubled
				//if noring R equip R
				//if noring L equip L
				//else choose R/L
				//equip R/L
				if(db.cur_ringR==id || db.cur_ringL==id)
				{
					_do_dequip(id);
					//_do_unequip_ring(item); //now done inside _do_dequip
				}
				if(db.cur_ringR==R_NORING)
				{
					_do_equip_ring(item);
					db.cur_ringR=id;
					status=RETURN_SUCCESS;
				}
				else if(db.cur_ringL==R_NORING)
				{
					_do_equip_ring(item);
					db.cur_ringL=id;
					status=RETURN_SUCCESS;
				}
				else
				{
					wprintw(win,"\nequip %s on right or left hand (r/l):",getname(item));
					wrefresh(win);
					switch(getch())
					{
						case 'r': 
							_do_dequip(db.cur_ringR);
							_do_equip_ring(item);
							db.cur_ringR=id;
							status=RETURN_SUCCESS;
							break;
						case 'l': 
							_do_dequip(db.cur_ringL);
							_do_equip_ring(item);
							db.cur_ringL=id;
							status=RETURN_SUCCESS;
							break;
						default:
							msg("invalid selection");
					}
				}

				break;

			default:
				msg("you can't equip that");
				status=RETURN_FAIL;
				break;
		}
	}
	else
	{
		msg("invalid selection");
		status=RETURN_FAIL;
	}
	if(status==RETURN_SUCCESS) msg("you equip %s",getname(&db.objects[id]));

	wclear(win);
	wrefresh(win);
	return status;
}
int _do_dequip(int id)
{
	//if(id!=-1)
	{
		//if(id>=0 && id<DBSIZE_OBJECTS && db.objects[id]._o.flags&ISCURSED)
		//{
		//	return 1;
		//}

		if(id==db.cur_mainhand)
		{
			//db.toggle_mainhand=id;
			db.cur_mainhand=-1;
		}
		if(id==db.cur_offhand)
		{
			//db.toggle_offhand=id;
			db.cur_offhand=-1;
		}
		if(id==db.cur_armour) db.cur_armour=-1;
		if(id==db.quiver) db.quiver=-1;
		if(id==db.cur_ringR)
		{
			_do_unequip_ring(&db.objects[id]);
			db.cur_ringR=R_NORING;
		}
		if(id==db.cur_ringL)
		{
			_do_unequip_ring(&db.objects[id]);
			db.cur_ringL=R_NORING;
		}
	}
	return 0;
}


int toggle_equip()
{
	int mainhand=db.toggle_mainhand;
	int offhand =db.toggle_offhand;
	if( mainhand==-1 && offhand==-1)
	{
		msg("nothing to toggle");
		return RETURN_FAIL;
	}

	Entity *new_main=NULL, *new_off=NULL;// *old_main=NULL, *old_off=NULL;
	

	db.toggle_mainhand=db.cur_mainhand;
	db.toggle_offhand=db.cur_offhand;

	for(int i=0;i<26;i++)
	{
		if(db.inventory[i]==mainhand) db.cur_mainhand=mainhand;
		if(db.inventory[i]==offhand) db.cur_offhand=offhand;
	}

	if(db.cur_mainhand!=-1) new_main=&db.objects[db.cur_mainhand];
	if(db.cur_offhand!=-1) new_off=&db.objects[db.cur_offhand];
	//if(db.toggle_mainhand!=-1) old_main=&db.objects[db.toggle_mainhand];
	//if(db.toggle_offhand!=-1) old_off=&db.objects[db.toggle_offhand];

	char new[MSGSZ];
	char *head=new;
	head+=sprintf(new,"you re-equip");
	if(new_main && new_main==new_off) //new twohand
		head+=sprintf(head," your %s",getbasic_name(new_main));
	if(new_main && new_main!=new_off) //new onehand main
		head+=sprintf(head," your %s",getbasic_name(new_main));
	if(new_main && new_off && new_main!=new_off) head+=sprintf(head," and");
	if(new_off && new_main!=new_off) //new onehand main
		head+=sprintf(head," your %s",getbasic_name(new_off));
	if(head!=new) msg(new);
	return RETURN_SUCCESS;
}

int has_wep(int which)
{
	int wearing=0;
	if(db.cur_mainhand!=-1 && db.objects[db.cur_mainhand]._o.which==which) wearing=1;
	if(db.cur_offhand!=-1 && db.objects[db.cur_offhand]._o.which==which) wearing=1;
	return wearing;
}
