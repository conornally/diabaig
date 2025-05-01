#include "extern.h"

//externs 
int messaged=0;
int verbose=0;
char message[MSGSZ];
char message_queue[2][MSGSZ];
char message_history[26][MSGSZ];

char prefix[4];
char suffix[4];
char thing_name[64]; //just a standard place for temporary name storage
char death_message[64];
char playername[12];
//#ifdef WINDOWS
//char scorefile[1028]="diabaig-scr.score";//{'d','i','a','b','a','i','g','.','s','c','r'};
//char savefile[1028]= "diabaig-save.save";//{'d','i','a','b','a','i','g','.','s','a','v','e'};
//char loadfile[1028]= "diabaig-save.save";//{'d','i','a','b','a','i','g','.','s','a','v','e'};
//#else
char scorefile[1028]="diabaig.scr";//{'d','i','a','b','a','i','g','.','s','c','r'};
char savefile[1028]= "diabaig.save";//{'d','i','a','b','a','i','g','.','s','a','v','e'};
char loadfile[1028]= "diabaig.save";//{'d','i','a','b','a','i','g','.','s','a','v','e'};

//#endif //WINDOWS
	   
//int  nspellscrolls=0;
char dragonname[DRAGONNAMESIZE]="dragonname";
char dragon_mod[DRAGONNAMESIZE]="the mod";
int game_won=0;
struct conf conf_diabaig;

WINDOW *win;
int seed;
bool wizardmode;
bool running;
char scroll_names[MAXSCROLL][16];
int testarena;

struct _database db;
nav_node bfs_nodes[XMAX*YMAX];
nav_node dijk_nodes[XMAX*YMAX];
nav_node path_memory[MAX_PATHMEM][XMAX*YMAX];
//nav_node path_nodes[PATHFINDMAX][XMAX*YMAX];
float dijk_map[XMAX*YMAX];
struct _rip_data rip_data;

obj_info type_info[MAXOBJTYPES]={
	{"potion", 30,"\0",true},
	{"scroll", 15,"\0",true},
	{"food",   6 ,"\0",true},
	{"gold",   55,"\0",true},
	{"weapon", 9 ,"\0",true},
	{"armour", 5 ,"\0",true},
	{"ring",   2 ,"\0",true},
	{"trinket", 0,"\0",true},
};

obj_info potion_info[MAXPOTION]={
	{"minor healing",	30,  "\0", false},
	{"confusion", 		15,  "\0", false},
	{"poison", 			10,  "\0", false},
	{"healing",			15,  "\0", false},
	{"invisibility",	6,   "\0", false},
	{"strength",		5,   "\0", false},
	{"dexterity",		5,   "\0", false},
	{"elixir",			1,   "\0", false},
	{"swiftness",		8,   "\0", false},
	{"slowness",		7,   "\0", false},
	{"sleeping",		10,  "\0", false},
	{"blindness",		10,  "\0", false},
	{"dragonbreath",	10,  "\0", false},
	{"ink",				10,  "\0", false},
	{"smokebomb",		20,  "\0", false},
};

obj_info scroll_info[MAXSCROLL]={
	{"identification", 	40, "\0", false},
	{"enchant weapon", 	8, 	"\0", false},
	{"enchant armour", 	7,  "\0", false},
	{"teleportation",  	15, "\0", false},
	{"cleanse spirit", 	5,  "\0", false},
	{"reveal map", 	   	5,  "\0", false},
	{"sense creatures",	5,  "\0", false},
	{"detect items",   	5,  "\0", false},
	{"alert monster",  	5,  "\0", false},
	{"learn spell",  	4,  "\0", false},
	{"nyctophobia",  	5,  "\0", false},
	{"knowledge",		10, "\0", false},
	{"amnesia", 		5,  "\0", false},
};

obj_info food_info[MAXFOOD]={
	{"slime-mold",			10, "\0", true},
	{"mushroom",			10, "\0", true},
	{"ration", 				0,  "\0",true},
};

//just to keep getinfo happy
obj_info gold_info[2]={
	{"gold",		100,"\0",true},
	{"dragon scale",1,	"\0",true}
};

obj_info trinket_info[MAXTRINKET]={
	{"dragon tooth", 0,"\0",true},
};

obj_info weapon_info[MAXWEAPON]={
	{"club",		10, "\0", true},
	{"dagger", 		20, "\0", true},
	{"sword", 		10, "\0", true},
	{"mace", 		5,  "\0", true},
	{"longsword", 	8,  "\0", true},
	{"broadsword", 	3,  "\0", true},
	{"battleaxe", 	10, "\0", true},
	{"warhammer", 	2,  "\0", true},
	{"spear", 		10, "\0", true},
	{"shield", 		15, "\0", true},
	{"shortbow",	5,  "\0", true},
	{"longbow", 	5,  "\0", true},
	{"recurve bow",	2,  "\0", true},
	{"arrow", 		35, "\0", true},
	{"towersheild",	5,  "\0", true},
	{"torch",		10, "\0", true},
};

obj_info armour_info[MAXARMOUR]={
	{"cloth robe", 		30, "\0", true},
	{"leather cloak", 	20, "\0", true},
	{"ring mail", 		13, "\0", true},
	{"plate mail", 		6,  "\0", true},
	{"mithril coat", 	3,  "\0", true}
};

obj_info ring_info[MAXRINGS]={
	{"satiation",		 5,	 "\0", false},
	{"vitality",		 10,  "\0", false},
	{"critical eye",	 10,  "\0", false},
	{"regeneration",	 5,   "\0", false},
	{"fire resistance",  10,  "\0", false},
	{"frost resistance", 10, "\0", false},
	{"poison resistance",10, "\0", false},
	{"earendil", 		 5,   "\0", false},
	{"the mighty", 		 5,   "\0", false},
	{"the steadfast",	 5,   "\0", false},
	{"concentration",	 5,   "\0", false},
	{"waking",	         5,   "\0", false}
};

obj_info trap_info[MAXTRAP]={
	{"bear trap", 10, "\0", true},
	{"confusion", 10, "\0", true},
	{"pitfall",   10, "\0", true}
};

dmg_info init_weapon_info[MAXWEAPON]={
	{"1d2", "1d1", W_ONEHAND                 }, //stick
	{"1d5", "3d4", W_ONEHAND|CANSTACK|ISMISSILE}, //dagger
	{"2d3", "1d2", W_ONEHAND                 }, //sword
	{"3d3", "1d2", W_ONEHAND                 }, //mace
	{"4d3", "1d2", W_TWOHAND                 }, //longsword
	{"4d4", "1d2", W_TWOHAND                 }, //broadsword
	{"3d2", "1d2", W_ONEHAND                 }, //battleaxe
	{"5d4", "1d2", W_TWOHAND                 }, //warhammer
	{"2d2", "3d3", W_ONEHAND|ISMISSILE       },  //spear
	{"1d1", "0d0", W_ONEHAND                 }, //shield

	{"0d1", "0d0", W_TWOHAND                 }, //shortbow
	{"1d1", "0d0", W_TWOHAND                 }, //longbow
	{"2d1", "0d0", W_TWOHAND                 }, //recurvebow

	{"0d0", "1d5", W_ARROW|CANSTACK|ISMISSILE}, //arrow
	{"2d2", "0d0", W_ONEHAND}, 					//towersheild
	{"1d1", "0d0", W_ONEHAND|CANSTACK},			//torch
};

int def_info[MAXARMOUR][2]={
	{2,5}, //cloth
	{4,1}, //leather
	{6,1}, //ringmail
	{10,2}, //platemail
	{15,5}, //mithril
};

int level_values[MAX_XPLEVELS]={
	10,
	50,
	100,
	250,
	500,
	1500,
	3000,
	5000,
	10000,
	50000,
};

_spell_info spell_info[MAXSPELL]={
	// name			prob, cooldown, known, nuses, mastery
	{"fire bolt", 		10, 200, false,0,20},
	{"heal", 			10, 125, false,0,20},
	{"bandage wounds", 	10, 100, true, 0,20},
	{"frostbite", 		10, 200, false,0,20},
	{"pacify", 			10, 200, false,0,20},
	{"polymorph", 		10, 200, false,0,20},
	{"disarm", 			10, 200, false,0,20},
	{"grappling vines", 10, 100, false,0,20},
	{"hypnotise", 		10, 200, false,0,20},
	{"bewilderment",	10, 200, false,0,20},

	{"firestorm",		 0, 300, false,0,-1},
	{"blizzard",		 0, 300, false,0,-1},
	{"tangling mangrove",0, 300, false,0,-1},
	{"holy healing"     ,0, 200, false,0,-1},

	{"repel"     ,		10, 50, false,0,25},
	{"shockwave" ,		0, 100, false,0,-1},

	{"slice" 		   ,10, 50,false,0,25},
	{"twirling strike" ,0,  80,false,0,25},
	{"whirlwind"       ,0, 100,false,0,-1},

	{"arcane barrier"  ,10, 100, false, 0, 20},
	{"arcane absorption",0, 150, false, 0, -1},
};

monster_info player_info = {"@", 0,0,0,ISPLAYER, 10,"4d1",10,1,	0,0,"the player"};

monster_info monsters[MAXMONSTERS]={
	//name 				prob xp level 				flags 				{hp, 	str,   dex, def, res, res_flags}
	{"Acidic Blob",		10, 20, 19,					ISAGRO,					60, 	"10d1",	 8,   8,   0,  IMMUNE_POISON|WEAKTO_FIRE,	"an amorphous body of jelly-like melting fluid"},
	{"Banshee", 		20, 30, 3+SECONDARYBOSS,	ISAGRO,					400, 	"12d3",	 8,  15,  10,  IMMUNE_SLEEP,				"a haunting grim gaunt figure with wide hollow eyes"},
	{"Chimera",			10, 100,15+SECONDARYBOSS,	ISAGRO|ISAPEX,			600, 	"10d7",	12,  15,  10,  IMMUNE_FIRE,					"its half lion, half goat and half snake!"},
	{"Dragon",			0,  400,999, 				ISAGRO,     			701,	"7d10",	10,  15,  10,  0,								"a huge cunning winged reptile that hoards gold"},
	{"Elder Dragon",   	10, 100,20+SECONDARYBOSS,	ISAGRO|ISAPEX|ISSLOW,	800, 	"5d20",	10,  15,  10,  WEAKTO_FIRE|WEAKTO_FROST|WEAKTO_POISON,	"a huge dragon, with brutal malice in its ancient eyes"},
	{"Fell Beast",		10, 30, 21,					ISAGRO|ISFLY,			50, 	"8d7", 	10,   0,   0,  WEAKTO_FIRE|WEAKTO_FROST,    "a hideous winged creature that oozes a sickening liquid"},
	{"Gate Keeper",		20, 35, 20,  				ISAGRO|ISSLOW,			250,	"7d10",	10,  15,   15, IMMUNE_SLEEP|IMMUNE_POISON,  "a heavy walking statue, wielding a massive sword and shield"},
	{"Hydra",			20, 1,  26,   				ISAGRO,					500,	"5d8", 	10,  15,   10, 0,                           "a large reptile with many heads, full of teeth"},
	{"Illusion",		0,  1,  999, 				0, 						1,  	"1d1", 	 0,   0,   0,  0,                           "is this really there, you can almost see through it"},
	{"Jaguar", 			0, 1,   6+SECONDARYBOSS,	ISAGRO,					300, 	"10d5",	 5,   3,   0,  WEAKTO_POISON|WEAKTO_FIRE,	"a big cat with camouflage spots, this one is huge"},
	{"K",				20, 1,  999, 				0, 						10, 	"1d1", 	 0,   0,   0,  0,							""},
	{"Lindworm",		10, 100,3+SECONDARYBOSS, 	ISAGRO|ISAPEX,			400, 	"12d3",	 9,  15,  10,  IMMUNE_SLEEP|IMMUNE_FROST,	"a massive reptilian creature with a terrifying gaze"},
	{"Mother of Dragons",0, 200,999,				ISAGRO,					300,	"20d2",	 8,  10,   5,  0,							"she is the mother of all Diabaigs dragons"},
	{"Nightmare",  		20, 100,15+SECONDARYBOSS, 	0, 						100, 	"1d1", 	15,  10,  20,  IMMUNE_SLEEP|WEAKTO_FIRE,	"a warped figment of pure terror, do not blink"},
	{"Obsidian Lizard",	20, 30, 27,					ISAGRO,					300, 	"4d7", 	 10, 20,  10,  IMMUNE_FIRE|IMMUNE_POISON|IMMUNE_FROST, "a reptile with dazzling glassy scales"},
	{"Phoenix",			5,  35, 5+SECONDARYBOSS, 	ISAGRO|ISFLY|ISSPEED,	40, 	"10d4",	13,   5,  20,  IMMUNE_FIRE|WEAKTO_FROST|WEAKTO_POISON, "a bird born in fire will never truly die"},
	{"Quetzalcoatl",   	20, 30, 11+SECONDARYBOSS,	ISAGRO, 				400,	"1d1",  10,  10,  10,  IMMUNE_FIRE|IMMUNE_FROST|WEAKTO_POISON, "this large feathered serpent can channel the wind"},
	{"Raggle",			0,  1,  999, 				ISFRIEND,				100, 	"2d2", 	10,  10,   0,  0, 							"a hooded creature with a large bag of trinkets to trade"},
	{"Sidhe",           20, 20,  28, 				ISAGRO|ISFLY|ISSPEED|CANTRACK,40,"2d6",	16,   0,   5,  RESIST_SLEEP|WEAKTO_FIRE, 	"a long clawed fae-like creature that haunts lost adventurers"},
	{"Tezcatlipoca",	20, 30, 10+SECONDARYBOSS, 	ISAGRO, 				300, 	"10d3",	12,  20,  10,  RESIST_POISON|RESIST_FIRE|RESIST_FROST, "a powerful sorcerer of darkness, half warrior half jaguar"},
	{"Uruk Captain",	20, 1,  25, 				ISAGRO,					10, 	"8d7", 	10,  10,   0,  0, 							"the leader of an orcish militia"},
	{"Vampiric Lord",	20, 1,  999, 				ISAGRO,					100, 	"1d1", 	10,   0,   0,  0, 							""},
	{"Wyvern",			35, 20,  23, 				ISAGRO|ISFLY,			150,	"5d10",	10,  10,   5,  IMMUNE_FIRE|RESIST_POISON, 	"a ferocious winged lizard",},
	{"Xololt",			20, 1,  999, 				ISFRIEND, 				10, 	"1d1", 	10,   0,   0,  0, 							""},
	{"Young Dragon",	35, 20,  22, 				ISAGRO|ISFLY,			30, 	"2d25",	15,   5,   20, RESIST_FIRE,					"a newly hatched dragon, still trying to control its fire"},
	{"Zealot",			20, 1,  999, 				0, 						10, 	"1d1", 	 0,   0,   0,  0,							""},

	{"", 0, 0, 999, 0, 0, "\0", 0, 0,0,0,""}, //[
	{"", 0, 0, 999, 0, 0, "\0", 0, 0,0,0,""}, // backslash 
	{"", 0, 0, 999, 0, 0, "\0", 0, 0,0,0,""}, //]
	{"", 0, 0, 999, 0, 0, "\0", 0, 0,0,0,""}, //^
	{"", 0, 0, 999, 0, 0, "\0", 0, 0,0,0,""}, //_
	{"", 0, 0, 999, 0, 0, "\0", 0, 0,0,0,""}, //`

	//name 			prob xp level flags 			{hp, str, dex, def, res, res_flags}
	{"alchemist",	20, 10, 10, 0, 					30,	"1d20",	10, 5,	5,	0,			"a human with a particular affinity for potions"},
	{"bat", 		20, 1,  0, 	ISWANDER|ISFLY, 	2, 	"1d2",	15,	0,	0,	0,			"a small winged mammal that moves erratically"},
	{"cat",			 0, 1, 10,  CANTRACK|ISFRIEND,	10,	"0d0",	25, 0,	0,	0,          "curiosity has got the better of this guy"}, //rare but will find and follow you
	{"druid",		15, 5,  9,  ISREGEN,			40,	"1d20",	9, 	10,	5,	0,          "a human that can commune with nature"},
	{"emu",			20, 2,  1, 	ISAGRO,				13, "2d2",	7, 	0,	0,	0,          "a flightless bird with a painful kick"},
	{"fae",			9,  4,  4, 	ISSPEED,			10, "1d6",	16, 4,	5,	0,			"a tiny winged creature with a sharp pin-like sword"},
	{"goblin", 		20, 2,  0, 	ISAGRO,				8, 	"1d4",	7, 	0,	0,	0,			"a small wicked creature with pointed ears and deep pockets"},
	{"harpy",		20, 10, 10, ISAGRO|ISFLY,		30,"10d2",	12, 0,	0,	0,			"half person half bird, as hideous as it is cruel"},
	{"illusionist",	20, 8,  7,  0,					30,	"1d1",	16,	0,	5,	0,			"a conjurer of images to dazzle and confuse you"},
	{"jackal",		20, 3,  2, 	ISAGRO, 			20,	"2d5",	8, 	0,	0,	0,			"a large canine animal with a keen sense of smell"},
	{"kestrel",		20, 3,  1,	ISAGRO|ISFLY|ISSPEED,9,	"1d2",	11,	0,	0,	0,			"a small bird of prey that will hover just out of reach"},
	{"lizardfolk",	30, 15, 15,	ISAGRO,				50,	"6d7",	10,	10,	10,	RESIST_FIRE,"a reptilian humanoid known for its strength in battle"},
	{"mimic",		20, 5,  5, 	0, 					30,	"2d8",	5, 	2,	0,	0,			"this creature can take on any form it chooses"},
	{"necromancer",	20, 17, 18, 0,	 				50,	"2d15",	10, 0,	10,	0,			"a sorcerer of dark deathly magic"},
	{"orc",			20, 8,  7, 	ISAGRO, 			35,	"6d2",	8, 	0,	0,	0,			"a large powerful brutish and twisted humanoid"},
	{"phantom",		20, 14, 17, ISAGRO,				45,	"5d5",	16, 0,	5,	IMMUNE_POISON,"a ghostly figment of something that died here, long ago"},
	{"quaggoth",	20, 12, 9, 	ISAGRO,				55,	"6d5",	8, 	0,	0,	0,			"a bear adapted for cave dwelling"},
	{"reanimation", 20, 5,  17, ISAGRO|ISSLOW,		0,  "0d0",	0, 	0,	0,	IMMUNE_POISON|IMMUNE_FROST|IMMUNE_FIRE|IMMUNE_SLEEP,"the corpse of some poor creature brought back"}, 
	{"spider", 		20, 3,  1,  ISAGRO,				10,	"2d2",	8, 	2,	0,	0,			"it has eight creepy legs and two large poisonous fangs"}, //poison bite
	{"troll",		18, 14, 14, ISAGRO|ISREGEN,		100,"6d6",	4, 	10,	0,	0,			"a massive marauding creature with a large club"},
	{"uruk",		20, 10, 8,  ISAGRO, 			25,	"4d4",	8, 	8,	0,	0,			"an orcish humanoid trained for blood thirsty merciless battle"},
	{"vampire",		20, 12, 12, ISAGRO,				50,	"4d5",	9, 	0,	0,	0,			"a blood sucking humanoid that looks almost bat-like"},
	{"witch",		20, 10, 6,  0,					15,	"1d10",	10, 0,	0,	RESIST_POISON,"a dark cloaked and hunched figure with a pointy hat"}, //spawn spiders
	{"xoloitzcuintli",0,0,  999,ISSCARED,			10,	"1d1",	10, 	0,	0,	0,		""},
	{"yeti",		20, 8,  12, ISAGRO,				50,	"7d4",	8, 	3,	0,	IMMUNE_FROST|WEAKTO_FIRE,"a large hairy creature with big horns"},
	{"zombie",		20, 5,  5, 	ISAGRO|ISSLOW,		55,	"5d4",	5, 	5,	0,	IMMUNE_POISON, "a fallen adventurer, brought back to life for some grim purpose"},

};

guesses potion_colours[NCOLOURS]={
	{"red",		false},
	{"blue",	false},
	{"yellow",	false},
	{"pink",	false},
	{"orange",	false},
	{"purple",	false},
	{"green",	false},
	{"black",	false},
	{"white", 	false},
	{"clear",	false},
	{"amber",	false},
	{"fizzy",	false},
	{"cloudy",	false},
	{"bubbling",false},
	{"grey",	false},
	{"cyan",	false},
	{"violet",	false},
	{"crimson", false},
	{"viscous",	false}
};

guesses mushroom_types[NMUSHROOMS]={
	{"red",			false},
	{"white",		false},
	{"black",		false},
	{"brown",		false},
	{"yellow",		false},
	{"seeping",		false},
	{"maggoty",	false},
	{"spotty",		false},
	{"long stemmed",false},
	{"wide capped", false}
};

guesses metal_types[NMETALS]={
	{"gold",	false},
	{"silver",	false},
	{"bronze",	false},
	{"copper",	false},
	{"platinum",false},
	{"rusted",	false},
	{"diamond", false},
	{"bejewelled",false},
	{"meteorite", false},
	{"tungsten",false},
	{"iron", 	false},
	{"brass", 	false},
	{"ruby",	false},
	{"sapphire",false},
	{"amethyst",false},
};

// external info access routines

obj_info *getinfo(int type)
{
	obj_info *info;
	switch(type)
	{
		case POTION: info=potion_info; 	break;
		case SCROLL: info=scroll_info; 	break;
		case FOOD: 	 info=food_info; 	break;
		case GOLD: 	 info=gold_info; 	break;
		case WEAPON: info=weapon_info; 	break;
		case ARMOUR: info=armour_info; 	break;
		case RING:	 info=ring_info;	break;
		case TRINKET:info=trinket_info;	break;
		default: 	 info=NULL; 		break;
	}
	return info;
}
int getmax(int type)
{
	int m=0;
	switch(type)
	{
		case POTION: m=MAXPOTION; 	break;
		case SCROLL: m=MAXSCROLL; 	break;
		case FOOD: 	 m=MAXFOOD; 	break;
		case GOLD: 	 m=2; 	break;
		case WEAPON: m=MAXWEAPON; 	break;
		case ARMOUR: m=MAXARMOUR; 	break;
		case RING:	 m=MAXRINGS;	break;
		case TRINKET:m=MAXTRINKET;	break;
		default: 	 m=0; 		break;
	}
	return m;
}

