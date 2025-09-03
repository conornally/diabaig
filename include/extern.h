#ifndef EXTERN_H
#define EXTERN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>

#include "platform.h"
#include "entity.h"

#define RETURN_SUCCESS 0
#define RETURN_FAIL 1
#define RETURN_UNDEF -1

#define RETURN_STATUSA 2
#define RETURN_STATUSB 3

extern WINDOW *win;
#define NCOLS 78
#define NROWS 33

#define DBSIZE_CREATURES 1000
#define DBSIZE_OBJECTS 1000
//#define PATHFINDMAX 12

#define NLEVELS 30
#define XMAX 77
#define YMAX 28
#define NROOMS 20
#define R_YMAX 20
#define R_YMIN 5
#define R_XMAX 30
#define R_XMIN 7
#define MAXREGIONS 14

#define NDAEMONS 50

#define MSGX 1
#define MSGY 27
#define MSGSZ 75

#define PLAYERNAMESZ 10
#define OBJNAMESIZE 32
#define OBJDESCSIZE 1028
#define CREATNAMESIZE 24
#define DRAGONNAMESIZE 16
#define DICESIZE 8

#define ANIM_RATE 10000
//#define ANIM_RATE 100000
#define DIJK_MAX (10000.0f)
#define MAXSHOP 6
#define MAX_PATHMEM 15
#define MAXADDS 20
#define SAVESLOTS 3

//char message[MSGSZ];
extern int messaged;
extern int verbose;
extern char message[MSGSZ];
extern char message_queue[2][MSGSZ];
extern char message_history[26][MSGSZ];

//handy storing places
extern char prefix[4];
extern char suffix[4];
extern char thing_name[64]; //just a standard place for temporary name storage
extern char death_message[64];
extern char playername[12];
extern char scorefile[1028];
extern char savefile[1028];
extern char loadfile[1028];
extern int  nspellscrolls;
extern char dragonname[DRAGONNAMESIZE];
extern char dragon_mod[DRAGONNAMESIZE];
extern int game_won;

extern struct conf conf_diabaig;
extern int FLUID_MATRIX[NFLUID_TYPES][NFLUID_TYPES];


#define MAX_XPLEVELS 10

#define nodir		-1
#define west 		1
#define east 		2
#define north 		3
#define south 		4
#define northwest 	5
#define southwest 	6
#define northeast 	7
#define southeast 	8

enum RIPTYPE
{
	RIP_KILLED,
	RIP_DRANK,
	RIP_STARVE,
	RIP_STATUS,
	RIP_QUIT,
	RIP_UNKOWN,
	RIP_BURN,
	RIP_SAVE,
	RIP_WIN,
	RIP_SUFFOCATE,
};

#define MAXHUNGER 1500
#define CRITMAX 40
#define NSPELLS 3
#define CURSERATE 10
#define ENCHANTRATE 5
#define DISENCHANTRATE 10
#define MAXILLUSIONS 15
#define PRIMARYBOSS 24
#define SECONDARYBOSS 29
#define XOLOTLLAYER 20
#define XOLOTLFOLLOW 200
#define VAMPBITE 10
#define RUSTRATE 3
#define FELLCHANCE 5
#define POTION_EFFECT_TIME 3 // APPLYING POTIONS
#define CATCHANCE 10
#define AGRORATE 750
#define OBSLIZ_HYPNORATE 20
#define APEXRATE 5
#define SHAPESHIFTRATE 20
#define CREATURETRICKLE 500


extern int seed;
extern bool wizardmode;
extern bool running;
extern int testarena;

typedef struct
{
	char obj_name[OBJNAMESIZE];
	int prob;
	char guess[OBJNAMESIZE];
	bool known;
	char desc[OBJDESCSIZE];
} obj_info;

typedef struct
{
	char monster_name[CREATNAMESIZE];
	int prob;
	int xp;
	int level;
	int flags;
	int hp;
	const char str[DICESIZE];
	int dex,def,res;
	int res_flags;
	char description[128];
} monster_info;

typedef struct
{
	char spell_name[OBJNAMESIZE];
	int prob;
	int cooldown;
	bool known;
	int nuses, mastery;
} _spell_info;

extern obj_info type_info[MAXOBJTYPES];
obj_info *getinfo(int type);
int getmax(int type);

extern obj_info scroll_info[MAXSCROLL];
extern obj_info potion_info[MAXPOTION];
extern obj_info food_info[MAXFOOD];
extern obj_info gold_info[2];
extern obj_info trinket_info[1];
extern obj_info weapon_info[MAXWEAPON];
extern obj_info armour_info[MAXARMOUR];
extern obj_info ring_info[MAXRINGS];
extern obj_info trap_info[MAXTRAP];

extern _spell_info spell_info[MAXSPELL];

extern monster_info player_info;
extern monster_info monsters[MAXMONSTERS];

extern int level_values[MAX_XPLEVELS];

#define NCOLOURS 19
#define NMUSHROOMS 10
#define NMETALS 15
typedef struct { char guess[16]; bool used; } guesses;
extern guesses potion_colours[NCOLOURS];
extern char scroll_names[MAXSCROLL][16];
extern guesses mushroom_types[NMUSHROOMS];
extern guesses metal_types[NMETALS];

typedef struct { char melee_dmg[DICESIZE], throw_dmg[DICESIZE]; int flags; } dmg_info;
extern dmg_info init_weapon_info[MAXWEAPON];
extern int def_info[MAXARMOUR][2];

typedef struct
{
	int c_id;
	int type;
	int time;
} _daemon;


typedef struct
{
	int id;
	int seed;
	int flags;
	char tile_flags[XMAX*YMAX];
	int upstair,downstair;
} level;

typedef struct
{
    int id;
    int price;
    int ident;
}shop_item;

struct _database
{
	int cur_level;
	int gold;
	int hunger;
	int xp, xplvl;
	int cur_mainhand, cur_offhand, cur_armour;
	int toggle_mainhand, toggle_offhand;
	int cur_ringR, cur_ringL;
	int quiver;
	int inventory[26];
	int nspellscrolls;
	int nrooms;
	int defeated_dragon;
	int frame;

	Entity creatures[DBSIZE_CREATURES];
	Entity objects[DBSIZE_OBJECTS];
	tile tiles[XMAX*YMAX];
	level levels[NLEVELS];
	room rooms[NROOMS];
	_daemon daemons[NDAEMONS];

	spell spells[NSPELLS];

	int buy[MAXSHOP][NLEVELS];
    shop_item shop[MAXSHOP];
};
extern struct _database db;

typedef struct 
{
	int vertex;
	int visited;
	int parent;
	int depth;
	float weight;
}nav_node;

extern nav_node bfs_nodes[XMAX*YMAX];
extern nav_node dijk_nodes[XMAX*YMAX];
extern nav_node dijk_flee[XMAX*YMAX];
extern nav_node path_memory[MAX_PATHMEM][XMAX*YMAX];
//extern nav_node path_nodes[PATHFINDMAX][XMAX*YMAX];

struct _rip_data
{
	int by;
	char cause[24];
   	char name[24];
};
extern struct _rip_data rip_data;
void set_ripdata(int by, char *name);

struct _score
{
	int gold;
	char message[128];
};

typedef struct
{
	int type;
	char symbol;
	long long unsigned int colour;
	coord pos, target_pos;
	int _step;
}animation;

typedef enum
{
	ROGUE,
	WARRIOR,
	MONK,
	STK_RAV_MAD,
	WIZARD,
	CLASSMAX

}classes;
extern char classnames[CLASSMAX][16];

struct queue
{
	int items[XMAX*YMAX];
	int front;
	int rear;
};

struct conf
{
	int move_north;
	int move_south;
	int move_east;
	int move_west;
	int move_northeast;
	int move_southeast;
	int move_northwest;
	int move_southwest;
	//int descend;
	//int ascend;

	//int drop;
	int drink;
	int read;
	int equip;
	int throw;
	int eat;
	int apply;
	//int toggle;
	int inventory;
	int search;
	int fire;
};

enum MENU_MODES
{ 
	MENU_NEWGAME,
	MENU_CONTINUE,
	MENU_HIGHSCORES,
	MENU_SETTINGS,
	MENU_CREDITS,
	MENU_QUIT
};


//RANDOM NUMBER STUFF
int rng(int range);
void seed_time();
int diceroll(dice_int dice);
int dicemax(dice_int dice);
float dicemean(dice_int dice);
void splitdice(const char *dice, dice_int dest);

void _log(const char *fmt,...);

#endif
