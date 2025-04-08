#include "extern.h"
#include "generic.h"

static char* _getname_obj(Entity *e);
static char* _getname_mon(Entity *e);

char *get_prefix(const char *name)
{
	if(name[0]=='a'||name[0]=='e'||name[0]=='i'||name[0]=='o'||name[0]=='u' ||
	   name[0]=='A'||name[0]=='E'||name[0]=='I'||name[0]=='O'||name[0]=='U') sprintf(prefix,"an");
	else sprintf(prefix,"a");

	if(!strcmp(name,dragonname))
		sprintf(prefix," ");
	return prefix;
}


char *getname(Entity *e)
{
	char *name=NULL;
	if(e)
	{
		if(e->flags & ISOBJ) name=_getname_obj(e);
		else if(e->flags & ISCREATURE) name=_getname_mon(e);
	}
	return name;
}

static char* _getname_mon(Entity *e)
{
	int getpref=1;
	char *name=malloc(64);
	if(e->_c.flags & ISPLAYER) sprintf(thing_name,"%s",playername);
	else if(e->_c.flags&ISINVIS || player->_c.flags&(ISBLIND|ISLSD)) sprintf(thing_name,"something");
	else
	{
		switch(e->_c.type)
		{
			case 'g':
				if(e->_c.form==gNORMAL) 	strcpy(name,"goblin");
				if(e->_c.form==gPIKEMAN) 	strcpy(name,"goblin pikeman");
				if(e->_c.form==gSHIELDBEARER) strcpy(name,"goblin shieldbearer");
				if(e->_c.form==gCUTTHROAT) 	strcpy(name,"goblin cut-throat");
				break;

			case 'l': 
				if(e->_c.form==lNORMAL)		strcpy(name,"lizardfolk");
				if(e->_c.form==lMARAUDER)	strcpy(name,"lizardfolk marauder");
				if(e->_c.form==lBERSERKER)	strcpy(name,"lizardfolk berserker");
				if(e->_c.form==lGRUNT)		strcpy(name,"lizardfolk grunt");
				if(e->_c.form==lLIEUTENANT)	strcpy(name,"lizardfolk lieutenant");
				break;
			case 'r':
				snprintf(name,64,"reanimated %s",monsters[e->_c.form-'A'].monster_name);
				break;
			case 't':
				if(e->_c.form==tNORAML) strcpy(name,"troll");
				if(e->_c.form==tFIRE) 	strcpy(name,"flame troll");
				if(e->_c.form==tFROST) 	strcpy(name,"frost troll");
				if(e->_c.form==tROT) 	strcpy(name,"rot troll");
				break;
				
			case 'v': 
				if(e->_c.form==vBAT) strcpy(name,"bat");
				else strcpy(name,"vampire");
				break;
			case 'V': 
				if(e->_c.form==vBAT) strcpy(name,"bat");
				else strcpy(name,"Vampiric Lord");
				break;
			case 'x':
				if(e->_c.form==xPHYLACTERY) strcpy(name,"phylactery");
				else strcpy(name,"x");
				break;

			case 'I':
				strcpy(name, monsters[e->_c.form-'A'].monster_name);
				break;

			case 'D':

				strcpy(name,dragonname);
				getpref=0;
				break;
			case 'R':
				strcpy(name,"Raggle");
				getpref=0;
				break;
			default:
				strcpy(name,monsters[e->_c.type-'A'].monster_name);
				break;
		}

		if(getpref) sprintf(thing_name,"%s %s",get_prefix(name),name);
		else strcpy(thing_name,name);
	}
	free(name);
	return thing_name;
}

static char *_getname_obj(Entity *e)
{
	int elevel=0;
	char *name=malloc(32);
	memset(name,'\0',32*sizeof(char));
	obj_info info, *tmp=getinfo(e->_o.type);
	if(!tmp) return NULL;

	if((e->_o.flags & ISCURSED) && (e->_o.flags & ISIDENTIF))
	{
		//strcat(name,"cursed ");
	}

	if( (elevel=e->_o.enchant_level) )
	{
		char* tmp=strdup(name);
		sprintf(name,"%senchanted %s%d %s",elevel>0?"":"dis", elevel>0?"+":"",e->_o.enchant_level, tmp);
		free(tmp);
	}

	info=tmp[e->_o.which];
	//if(info.known) name=info.obj_name;
	//else name=info.guess;
	if(info.known) strcat(name,info.obj_name);
	else strcat(name,info.guess);


	if(e->_o.quantity>1)
	{
		sprintf(prefix,"%d",e->_o.quantity);
		sprintf(suffix,"s");
	}
	else
	{
		get_prefix(name);
		memset(suffix,'\0',4);
	}

	switch(e->_o.type)
	{
		case POTION: 
			if(info.known)
			{
				if(e->_o.which==P_ELIXIR) 
				{
					if(e->_o.quantity==1) sprintf(prefix,"an");
					sprintf(thing_name, "%s elixir%s", prefix, suffix);
				}
				else if(e->_o.which==P_SMOKEBOMB) 
				{
					sprintf(thing_name, "%s smokebomb%s", prefix, suffix);
				}
				else sprintf(thing_name, "%s potion%s of %s", prefix, suffix, info.obj_name);
			}
			else
			{
				//get_prefix(info.guess);
				sprintf(thing_name, "%s %s potion%s", prefix, info.guess, suffix);
			}
			break;

		case SCROLL:
			snprintf(thing_name,64,"%s scroll%s titled \"%s\"", (e->_o.quantity>1)?prefix:"a", suffix, name); 
			break;

		case FOOD:
			if(info.known) sprintf(thing_name,"%s %s%s",prefix,info.obj_name,suffix);
			else
			{
				sprintf(thing_name,"%s %s mushroom%s",prefix,info.guess,suffix);
			}
			break;
		case GOLD: 
			if(e->_o.which==0) sprintf(thing_name,"some gold");
			if(e->_o.which==1) sprintf(thing_name,"a dragon scale");
			break;
		case WEAPON:
			if(e->_o.which==TORCH && e->_o.quantity>1) sprintf(suffix,"es");
			sprintf(thing_name,"%s %s%s",prefix,name,suffix);
			break;
		case ARMOUR:
			//sprintf(thing_name,"%s %s%s", prefix, info.known ? info.obj_name:info.guess, suffix);
			sprintf(thing_name,"%s %s%s",prefix,name,suffix);
			break;

		case RING:
			get_prefix(name);
			sprintf(thing_name,"%s %s ring",prefix,name);
			/*get_prefix(info.guess);
			if(!info.known)
				sprintf(thing_name,"%s %s ring",prefix, info.guess);
			else
				sprintf(thing_name,"%s %s ring of %s", prefix, info.guess, info.obj_name);
			*/
			break;
		case TRINKET:
			//get_prefix(name);
			if(e->_o.which==TK_DRAGONTOOTH && e->_o.quantity>1)
				sprintf(thing_name,"%s %s",prefix, "dragon teeth");
			else sprintf(thing_name,"%s %s%s",prefix,info.obj_name,suffix);
			break;

		default :
			sprintf(thing_name,"%s %s%s",prefix,info.obj_name,suffix);
			break;
	}
	free(name);
	return thing_name;
}



int weighted_pick(int list[], int length)
{
	int total=0, _rng, i;
	for(i=0;i<length;i++) total+=list[i];
	_rng=rng(total);
	total=0;
	for(i=0; i<length;i++)
	{
		if( total<=_rng && _rng<(total+list[i]) ) break;
		total+=list[i];
	}
	return i;
}


char *getbasic_name(Entity *e)
{
	char *name="\0";
	obj_info *info;
	if(e)
	{
		if(e->flags & ISOBJ)
		{
			info=getinfo(e->_o.type);
			name=info[e->_o.which].obj_name;
		}
		if(e->flags & ISCREATURE)
		{
			if(e->_c.type=='@') name=player_info.monster_name;
			else name=monsters[e->_c.type-'A'].monster_name;
		}
	}
	return name;
}

void gen_dragonname(char* name, int form)
{
	char* prefs[]={"Ancala", "Ancolo", "Ung", "Lao", "Zhao","Gor",
				   "Zuco","Ang","Kora","Iro","Tof",
				   "Uno","Dos","Tres","Ein","Vier","Du",
				   "Red","Blak","Bak","Eis","Doth","Uruk","Orak",
				   "Arok","Anok","Onok","Cos","Kos","Kod",
				   "Torak","Ren","Dred",
						0};
	char* sufs[]={"gon", "lung", "tang","magala","mog","mug","mang",
				  "kung","lonng","tung","shaolung","shanlung","shung",
				  "oro-gon","-angon","shad","skad","uno-lung",
				  "koro","kora","korung",
						0};

	char *firemods[]= {"Black","Terrible","Flammable","Inflammable","Mighty",
		"Golden","Copper","Slender","Serpentine","Cruel","Unrelenting", 
		"Wrathful","Angered","Inferno","Infernal","Vengeful",
		0};

	char *icemods[]= { "Grand","Ancient","Godly","Holy","Silver","Slumbering",
		"Armoured","Fortified","Impenetrable","Frozen","Glacial","Restful",
		"Righteous","Slumbering",
		0};

	char *poismods[]={ "Sly","Twisted","Corrupted","Dire","Unholy","Bloated",
		"Horrendous","Untrusted","Hideous","Rotten","Corrupting","Dreadful",
		0};

	char* pre= prefs[ rng(clen(prefs))];
	char* suf= sufs[  rng(clen(sufs))];
	char* mod ;
	switch(form)
	{
		case DRG_FIRE: mod=firemods[ rng(clen(firemods)) ]; break;
		case DRG_ICE: mod=icemods[ rng(clen(icemods)) ]; break;
		case DRG_POISON: mod=poismods[ rng(clen(poismods)) ]; break;
		default: mod="UNKNOWN";
	}
	snprintf(name, DRAGONNAMESIZE, "%s%s",pre,suf);
	snprintf(dragon_mod, DRAGONNAMESIZE, "%s",mod);

}
