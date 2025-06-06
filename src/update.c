#include "diabaig.h"
#define NNOISES 5
static int frame=0;
static char *_noises[NNOISES]={
	"you hear something approaching",
	"you hear some eery noises",
	"you hear footsteps",
	"you suddenly feel tense",
	"you hear something moves in another room"
};

static int update_player();
static int update_entity(Entity *e);
static int update_object(Entity *e);
static int offscreen_update_entity(Entity *e);

int update()
{
	int id;
	Entity *e;

	//char _tmp_msg[MSGSZ];
	//strncpy(_tmp_msg,message_queue[1], MSGSZ);

	set_ripdata(RIP_UNKOWN,"unknown reasons");
	//db.hunger--;
	for(id=0;id<db.nrooms;id++)
	{
		unlight_room(&db.rooms[id]);
		if(db.rooms[id].flags & RM_LITBYRING && player->_c._inroom != &db.rooms[id]) 
			db.rooms[id].flags &= ~RM_LITBYRING;
	}
	//light_room( inroom(player));

	air_diffuse(0.98,0.5);
	if(frame%2) //status effects happen every second udpate (possbily, i dont like this as all
	{
		for(id=0; id<NDAEMONS; id++)
		{
			if(db.daemons[id].type!=D_NONE) update_daemon(&db.daemons[id]);	
		}
	}

	for(id=0; id<XMAX*YMAX; id++) //update tiles
	{
		tile *t=&db.tiles[id];
		t->flags &=(~ML_VISIBLE);
		if(t->air_pressure>0.15)
		{
			if( (e=t->obj) && (
				(t->air==COMBUST && e->_o.type==SCROLL) ||
				(t->air==MIASMA && e->_o.type==FOOD) 	||
				(t->air==MIST   && e->_o.type==POTION)))
				{
					char *messages[4]={
						"%s was burnt to a crisp",
						"%s could not withstand the ice",
						"%s rots in the miasma",
						"%s was destroyed"};
					msg(messages[MIN(t->air-1,3)], getname(e));


					clear_entity(e);
				}
		}
		
	}

	for(id=0;id<DBSIZE_CREATURES; id++)
	{
		e=&db.creatures[id];
		if(e && (e->flags & ISACTIVE))// && e->pos.z==db.cur_level)
		{
			if(e->pos.z==db.cur_level)
			{
				update_entity(e);
				if(!moat(e->pos.x,e->pos.y)) moat(e->pos.x,e->pos.y)=e;
			}
			else
			{
				offscreen_update_entity(e);
			}
		}
	}

	for(id=0;id<DBSIZE_OBJECTS; id++)
	{
		e=&db.objects[id];
		if(e && (e->flags & ISACTIVE)) 
		{
			update_object(e);
		}
	}

	if(db.hunger<FAINT_LIMIT) faint();
	if(db.hunger<=0) starve();

	unlight_room(player->_c._inroom); //can unlight not just go here?
	light_room(inroom(player));
	light_local_area();

	if(!messaged) msg("\0");
	frame++;


	//FILE *fp=fopen("/tmp/diabaig.debug","w");
	//if(fp)
	//{
	//	for(int i=0;i<sizeof(db);i++)
	//	{
	//		//_daemon d=db.daemons[i];
	//		fprintf(fp,"%c",*(&db+i));
	//	}
	//		fflush(fp);
	//	fclose(fp);
	//}

	return 0;
}

static int update_entity(Entity *e)
{
	int status=RETURN_UNDEF;
	int flag=e->_c.flags;
	e->_c.stamina=1;

	status_system(e);

	if(e->_c.stat.hp<=0) murder(player,e);
	if(flag & (ISSLEEP|ISSTUN)) 
	{
		if(e==player)
		{
			platform_sleep(ANIM_RATE*2);
			flushinp();
		}
		return RETURN_SUCCESS;
	}
	if(flag & ISFREEZE)
	{
		if(e!=player) return RETURN_SUCCESS;
	}

	if(e->_c.turn-- <=0)
	{
		switch(e->_c.type)
		{
			//special updates
			case '@': update_player(); break;
			case 'a': _ai_alchemist(e); break;
			case 'c': _ai_cat(e); break;
			case 'd': _ai_druid(e); break;
			case 'i': _ai_illusionist(e); break;
			case 'n': _ai_necromancer(e); break;
			case 'v': _ai_vampire(e); break;
			case 'w': _ai_witch(e); break;
			case 'y': _ai_yeti(e); break;

			case 'B': _ai_Banshee(e); break;
			case 'D': _ai_dragon(e); break;
			case 'E': _ai_ElderDragon(e); break;
			case 'F': _ai_FellBeast(e); break;
			case 'J': _ai_Jaguar(e); break;
			case 'L': _ai_Lindworm(e); break;
			case 'M': _ai_MOD(e); break;
			case 'N': _ai_Nightmare(e); break;
			case 'O': _ai_ObsidianLizard(e); break;
			case 'Q': _ai_Quetzalcoatl(e); break;
			case 'T': _ai_Tezcatlipoca(e); break;
			case 'V': _ai_VampireLord(e); break;
			case 'Y': _ai_youngdragon(e); break;
		}

		if(flag & ISWANDER) status=wander(e);
		//if(flag & CANTRACK) status=hunt(e,player);//track_player(e); //this should not actually attack the player
		if(flag & ISSCARED)
		{
			flee(e,&db.creatures[e->_c.target]);
			//flee_player(e);
		}
		if(flag & ISAGRO)
		{
			if(flag&(ISFOLLOW|CANTRACK) && e->_c.target!=-1) hunt(e,&db.creatures[e->_c.target]);
			else
			{
				// If targetting player, chance to become agro
				if(e->_c.target==player->id && !rng(AGRORATE) && e->_c.type!='M' && e->_c.type!='D')
				{
					e->_c.flags|=CANTRACK; //chance it begins to track
					msg(_noises[rng(NNOISES)]);
				}
			}
		}
		if(e->_c.flags & ISSPEED) e->_c.turn=0;
		else if(e->_c.flags & ISSLOW) e->_c.turn=3;
		else e->_c.turn=1;

		if(e->_c.flags&CANTRACK) e->_c.last_seen_player=0;
		else e->_c.last_seen_player+=1;

		if(e->_c.flags&ISFOLLOW && e->_c.last_seen_player==(MAX_PATHMEM+1)) msg("you have shaken %s off your trail",getname(e));
	}

	inroom(e);
	if(e!=player && !(e->_c.flags&ISBLIND) && !(player->_c.flags&ISINVIS))
	{
		if( (tileat(e->pos.x,e->pos.y)->flags&ML_VISIBLE) )
		{
			if(!(e->_c.flags&SEENPLAYER)) do_first_sight(e);

			e->_c.flags|=SEENPLAYER;
			e->_c.last_seen_player=0;
		}
		else if(0) // LINE OF SIGHHT IN CORRIDOR
		{
		}
		else if(e->_c._inroom && (e->_c._inroom==player->_c._inroom)) // LINE OF SIGHHT IN DARKROOM
		{
			e->_c.last_seen_player=0;
		}
	 	//else
		//{
		//	e->_c.last_seen_player+=1;
		//}

	}
	//if(e!=player && e->_c.type!='R')msg("%s %d",getname(e),e->_c.last_seen_player);



	//if(e!=player)
	//{
	//	if(e->_c._inroom==player->_c._inroom)
	//	{
	//		if(!(e->_c.flags&SEENPLAYER)) do_first_sight(e);
	//		e->_c.flags|=SEENPLAYER;
	//		e->_c.last_seen_player=0;
	//	}
	//	else if( (tileat(e->pos.x,e->pos.y)->flags&ML_VISIBLE) || lineofsight(e,player))
	//	{
	//		e->_
	//	}
	//}
	
	if(!(e->flags&ISACTIVE)) clear_entity(e);
	return status;
}

static int offscreen_update_entity(Entity *e)
{
	switch(e->_c.type)
	{
		case 'c': _ai_cat_offscreen(e); break;
		case 'D': _ai_dragon_offscreen(e); break;
		default: break;

	}
	return false;
}

static int update_object(Entity *e)
{
	int status=0;
	if(e->_o.quantity<=0)
	{
		_do_dequip(e->id);
		if(objat(e->pos.x,e->pos.y)==e) objat(e->pos.x,e->pos.y)=NULL;
		for(int id=0;id<26;id++) 
		{
			if(db.inventory[id]==e->id) db.inventory[id]=-1;
		}
		clear_entity(e);
	}

	//item stacking on the same tile
	if(objat(e->pos.x,e->pos.y)==NULL && e->pos.z==db.cur_level)
	{
		int in_inv=false;
		for(int id=0; id<26; id++)
		{
			if(db.inventory[id]==e->id) in_inv=true;
		}
		if(!in_inv) objat(e->pos.x, e->pos.y)=e;
	}
	return status;
}

static int update_player()
{
	
	messaged=0;
	if( !( has_ring(R_SLOWHUNGER) && rng(2)) ) db.hunger--;
	if(db.hunger==500) msg("you are starting to feel hungry");
	if(db.hunger<150) 
	{
		if(!rng(5))
		{
			player->_c.stat.hp--;
			msg("you are starving");
		}
	}
	
	light_room( inroom(player));
	light_local_area();
	for(int i=0; i<26; i++)
	{
		int id=db.inventory[i];
		if(id!=-1)
		{
			Entity *item=&db.objects[id];
			if(item->_o.quantity<=0)
			{
				db.inventory[i]=-1;
				clear_entity(item);
			}
		}
	}
	for(spell *sp=db.spells;sp<&db.spells[NSPELLS];sp++)
	{
		if(sp->type!=SP_NOTLEARNT)
		{
			int dn=1 + has_ring(R_CONCENTRATION);
			sp->charge=MIN(sp->charge+dn, sp->cooldown);
		}
	}

	if(player->_c.flags&ISREBIRTH && !rng(XOLOTLFOLLOW))
	{
		for(int n=0; n<1+rng(2); n++)
		{
			int i=0;
			while(db.tiles[i].flags&(ML_OBSTRUCTS|ML_VISIBLE))
			{
				i=rng(XMAX*YMAX);
			}
			Entity *e=_new_monster('x');
			db.tiles[i].creature=e;
			e->pos=(coord){i%XMAX, i/XMAX, db.cur_level};
		}
		msg("you hear a howl in the distance");
	}

	if(!rng(CREATURETRICKLE))
	{
		Entity *e=new_monster();
		if(e->_c.flags&ISAGRO) e->_c.flags|=CANTRACK;
		room *r=&db.rooms[rng(db.nrooms)];
		while(r==player->_c._inroom) r=&db.rooms[rng(db.nrooms)];
		placeinroom(r,e);
	}

	if(autopilot.active) do_autopilot();
	else
	{
		int input;//=getch();
		int status=RETURN_UNDEF;
		MEVENT mevent;
		do
		{
			input=wgetch(win);
			
			if(input=='Q') {
				status=quit(); 
				//set_ripdata(RIP_QUIT,"the quest early");
				//status=RETURN_SUCCESS;
				break;
			}
			else if(input=='S'){ 
				status=save(savefile); 
				set_ripdata(RIP_SAVE,"the quest"); 
				status=RETURN_SUCCESS;
				break;
			}
			
			if(player->_c.flags&ISFREEZE) 
			{
				if( !rng(player->_c.stat.res/3) )
				{
					msg("you are frozen in place");
					break;
				}
				else msg("you fight against the ice");
			}

			if(input=='.') status=RETURN_SUCCESS;
			else if(input==conf_diabaig.drink) 	status=drink();
			else if(input==conf_diabaig.read) 	status=read_scroll();
			else if(input==conf_diabaig.equip)	status=equip();
			else if(input==conf_diabaig.throw)	status=throw_item(); 
			else if(input==conf_diabaig.eat)	status=eat(); 
			else if(input==conf_diabaig.search)	status=search(); 
			else if(input==conf_diabaig.fire)	status=fire_bow();
			else if(input==conf_diabaig.apply)	status=apply_potion(); 
			else if(input=='D') status=drop();
			else if(input=='W')	toggle_equip();
			else if(input==',')	pickup();

			else if(input=='1')	status=cast_spell(0);
			else if(input=='2')	status=cast_spell(1);
			else if(input=='3')	status=cast_spell(2);

			else if(input=='?') {	
				show_help(); 		
				status=RETURN_UNDEF; 
			}

			else if(input==conf_diabaig.inventory){
				show_inventory();	
				status=RETURN_UNDEF; 
			}
			//else if(input=='Q') {
			//	running=false; 
			//	set_ripdata(RIP_QUIT,"the quest early");
			//	status=RETURN_SUCCESS;
			//}

			//else if(input=='S'){ 
			//	status=save(savefile); 
			//	set_ripdata(RIP_SAVE,"the quest"); 
			//	status=RETURN_SUCCESS;
			//}

			else if(input==conf_diabaig.move_north || input==KEY_UP   ||input==60610) 	status=walk(player,north);
			else if(input==conf_diabaig.move_south || input==KEY_DOWN ||input==60616) 	status=walk(player,south);
			else if(input==conf_diabaig.move_east  || input==KEY_RIGHT||input==60614)	status=walk(player,east);			
			else if(input==conf_diabaig.move_west  || input==KEY_LEFT ||input==60612)  status=walk(player,west);
			else if(input==conf_diabaig.move_northwest|| input==KEY_A1 || input==KEY_HOME) 	status=walk(player,northwest);
			else if(input==conf_diabaig.move_northeast|| input==KEY_A3 || input==KEY_PPAGE)	status=walk(player,northeast);
			else if(input==conf_diabaig.move_southwest|| input==KEY_C1 || input==KEY_END) 	status=walk(player,southwest);
			else if(input==conf_diabaig.move_southeast|| input==KEY_C3 || input==KEY_NPAGE)	status=walk(player,southeast);
			else if(input=='>') status=descend(player);
			else if(input=='<') status=ascend(player);

			else if(input=='0')
			{
				wclear(win);
				draw_wee_guy(10,10);
				wgetch(win);
			}
			else if(input=='@') show_performance();
			else if(input==':'){
				status=wizard_console(); 
				if(status) msg("command failed");
			}

			else if(input=='H'){autopilot.target=-1;autopilot.active=true; autopilot.direction=west;	 status=RETURN_SUCCESS;}
			else if(input=='J'){autopilot.target=-1;autopilot.active=true; autopilot.direction=south;	 status=RETURN_SUCCESS;}
			else if(input=='K'){autopilot.target=-1;autopilot.active=true; autopilot.direction=north;	 status=RETURN_SUCCESS;}
			else if(input=='L'){autopilot.target=-1;autopilot.active=true; autopilot.direction=east;	 status=RETURN_SUCCESS;}
			else if(input=='Y'){autopilot.target=-1;autopilot.active=true; autopilot.direction=northwest;status=RETURN_SUCCESS;}
			else if(input=='U'){autopilot.target=-1;autopilot.active=true; autopilot.direction=northeast;status=RETURN_SUCCESS;}
			else if(input=='B'){autopilot.target=-1;autopilot.active=true; autopilot.direction=southwest;status=RETURN_SUCCESS;}
			else if(input=='N'){autopilot.target=-1;autopilot.active=true; autopilot.direction=southeast;status=RETURN_SUCCESS;}
			else if(input==KEY_MOUSE)
			{
				if(getmouse(&mevent)==OK && (mevent.bstate&(BUTTON1_PRESSED|BUTTON1_CLICKED)))
				{
					int x=mevent.x;
					int y=mevent.y;
					if(x>=0 && x<XMAX && y>=0 && y<YMAX && db.levels[db.cur_level].tile_flags[y*XMAX+x]&MS_EXPLORED)
					{
						autopilot.ignore=1;
						autopilot.active=true;
						autopilot.target=(mevent.y*XMAX+mevent.x);
						status=RETURN_SUCCESS;
					}
					else msg("invalid target");
				}
			}
			/*
			else if(input=='@') {
				tileat(player->pos.x,player->pos.y)->air=1+rng(4);
				tileat(player->pos.x,player->pos.y)->air_pressure+=0.9;

				status=RETURN_SUCCESS;
				//air_diffuse(0.98,0);
			}
			*/
			else{
				msg("unrecognised command: %c",input); 
				status=RETURN_FAIL;
			}

			//switch(input)
			//{
			//	case 'D':status=drop();break;
			//	case 'd':status=drink();break;
			//	case 'r':status=read_scroll();break;
			//	case 'w':status=equip();break;
			//	case 't':status=throw_item(); break;
			//	case 'e':status=eat(); break;
			//	//case conf_diabaig.eat:status=eat(); break;
			//	case 's':status=search(); break;
			//	case 'f':status=fire_bow();break;
			//	case 'a':status=apply_potion(); break;
			//	case 'W':toggle_equip();break;
			//	case ',':pickup();break;

			//	case '1':status=cast_spell(0); break;
			//	case '2':status=cast_spell(1); break;
			//	case '3':status=cast_spell(2); break;

			//	case '?':show_help(); 		status=RETURN_UNDEF; break;
			//	case 'i':show_inventory();	status=RETURN_UNDEF; break;
			//	case 'Q': running=false; set_ripdata(RIP_QUIT,"the quest early"); break;
			//	case 'S': status=save(savefile); set_ripdata(RIP_SAVE,"the quest"); break;

			//	case 'k': case KEY_UP: 					status=walk(player,north);break;
			//	case 'j': case KEY_DOWN: 				status=walk(player,south);break;
			//	case 'l': case KEY_RIGHT: 				status=walk(player,east);break;
			//	case 'h': case KEY_LEFT: 				status=walk(player,west);break;
			//	case 'y': case KEY_A1: case KEY_HOME:  	status=walk(player,northwest);break;
			//	case 'u': case KEY_A3: case KEY_PPAGE: 	status=walk(player,northeast);break;
			//	case 'b': case KEY_C1: case KEY_END: 	status=walk(player,southwest);break;
			//	case 'n': case KEY_C3: case KEY_NPAGE: 	status=walk(player,southeast);break;
			//	case '>': status=descend(player);break;
			//	case '<': status=ascend(player); break;

			//	case '@': show_performance(); break;
			//	case ':': status=wizard_console(); 
			//			  if(status) msg("command failed");
			//			  break;
			//	case '#':
			//			  display_dijkstra();
			//			  wrefresh(win);
			//			  getch();
			//			  break;

			//	case 'H': autopilot.active=true; autopilot.direction=west; break;
			//	case 'J': autopilot.active=true; autopilot.direction=south; break;
			//	case 'K': autopilot.active=true; autopilot.direction=north; break;
			//	case 'L': autopilot.active=true; autopilot.direction=east; break;
			//	case 'Y': autopilot.active=true; autopilot.direction=northwest; break;
			//	case 'U': autopilot.active=true; autopilot.direction=northeast; break;
			//	case 'B': autopilot.active=true; autopilot.direction=southwest; break;
			//	case 'N': autopilot.active=true; autopilot.direction=southeast; break;

			//	case '.': break;
			//	default :
			//		msg("unrecognised command: %c",input); 
			//		status=RETURN_FAIL;
			//		break;

			//}

			/// Stuff to do with failing or suceeding inputs
			if(status==RETURN_STATUSA || status==RETURN_STATUSB) status=RETURN_SUCCESS; //walking successes
			if(status!=RETURN_SUCCESS)
			{
				light_local_area();
				display(); //redraw main screen if event failed
			}

		} while(status != RETURN_SUCCESS);
		player->_c.stamina--;
	}

	//bfs(bfs_nodes,XMAX*YMAX,player->pos.y*XMAX+player->pos.x);

	dijk_reset(dijk_nodes);
	if(!(player->_c.flags&ISINVIS)) dijk_addsrc(dijk_nodes, player->pos.y*XMAX+player->pos.x,0);
	for(int i=0; i<(MAX_PATHMEM-1); i++)
	{
		memcpy(path_memory[i+1],path_memory[i],sizeof(path_memory[0]));
	}
	memcpy(path_memory[0],dijk_nodes, sizeof(dijk_nodes));

	if(!(player->_c.flags&ISINVIS))
	{
		dijk_reset(dijk_flee);
		dijk_addsrc(dijk_flee, player->pos.y*XMAX+player->pos.x,0);
		dijk_scale(dijk_flee, -1.2);
		dijk_scan(dijk_flee);
	}

	light_room( inroom(player));
	light_local_area();

	//if(!strcmp(_tmp_msg, message_queue[1])) msg("\0");
	return 0;
}

void light_local_area()
{
	int x=player->pos.x;
	int y=player->pos.y;
	seen(x,y);
	if(!(player->_c.flags & ISBLIND))
	{
		seen(x-1,y+1);
		seen(x-1,y);
		seen(x-1,y-1);
		seen(x,y+1);
		seen(x,y-1);
		seen(x+1,y+1);
		seen(x+1,y+0);
		seen(x+1,y-1);
	}
}
