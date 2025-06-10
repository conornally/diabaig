#ifndef ENTITY_H
#define ENTITY_H

//OBJ TYPES
#define POTION '!'
#define SCROLL '?'
#define FOOD   ':'
#define GOLD   '*'
#define WEAPON '/' 
#define ARMOUR ']'
#define RING   '='
#define TRINKET   '^'
#define MAXOBJTYPES 8

#define STAFF  '/'
#define CORPSE '~'

#define WALL   '~'
#define PASSAGE '#'
#define HWALL  '-'
#define VWALL  '|'
#define FLOOR  '.'
#define DOOR   '+'
#define STONE  ' '
#define TRAP   '^'
#define UPSTAIRS '<'
#define DOWNSTAIRS '>'
#define GUARDIAN 'O'

#define MAXMONSTERS 58

//POTION TYPES
#define P_HEALING   0
#define P_CONFUSION 1
#define P_POISON   2
#define P_HEALINGPLUS 3
#define P_INVISIBILITY 4 //make spell
#define P_STR 5
#define P_DEX 6
#define P_ELIXIR 7
#define P_HASTE 8
#define P_SLOWNESS 9
#define P_SLEEP 10
#define P_BLINDNESS 11
#define P_BURNING 12
#define P_INK	  13
#define P_SMOKEBOMB	  14
#define MAXPOTION 15

//SCROLL TYPES
#define S_ID 0
#define S_ENCHANTWEAPON 1
#define S_ENCHANTARMOUR 2
#define S_TELEPORT  3
#define S_CLEANSE 4
#define S_MAP 5
#define S_SENSECREATURES 6
#define S_SENSEITEMS	 7
#define S_ALERTMONSTER 8
#define S_LEARNSPELL 9
#define S_NYCTOPHOBIA 10
#define S_LORE	11
#define S_AMNESIA 12
#define MAXSCROLL 13
#define S_SUMMONMONSTER
#define S_LEVELUP

//FOOD TYPES
#define F_SLIMEMOLD 0
#define F_MUSHROOM 1
#define F_RATION 2
#define MAXFOOD 3
//#define F_CAVEMUSHROOM 1
//#define F_GRIMCAP 2
//#define F_MAGICMUSHRROM 3

//TRAP TYPES
#define T_BEARTRAP 0
#define T_CONFUSETRAP 1
#define T_PITFALL 2
#define MAXTRAP 3

//WEAPON TYPES
enum WEAPONS{
	STICK,
	DAGGER,
	SWORD,
	MACE,
	LONGSWORD,
	BROADSWORD,
	BATTLEAXE,
	WARHAMMER,
	SPEAR,
	SHIELD,
	SHORTBOW,
	LONGBOW,
	RECURVEBOW,
	ARROW,
	TOWERSHIELD,
	TORCH,
	MAXWEAPON
};

//ARMOUR TYPES
enum ARMOURS{
	CLOTH,
	LEATHER,
	RING_MAIL,
	PLATE_MAIL,
	MITHRIL,
	SRC_CLOAK,
	MAXARMOUR
};

//RING TYPES
#define R_NORING    -1
#define R_SLOWHUNGER 0
#define R_VITALITY  1
//#define R_BERSERKER 2  //hp++ str-- (REMOVED)
#define R_CRITICALEYE 2 //dex++
#define R_REGEN 3		  //add another regen daemon (OP?)
//#define R_REJUVINATION 5  (REMOVED)
#define R_FIRERESIST   4
#define R_FROSTRESIST  5
#define R_POISONRESIST 6
#define R_ELENDIL 7
#define R_STRENGTH 8
#define R_DEFENCE 9
#define R_CONCENTRATION 10
#define R_WAKING 11
#define MAXRINGS 12


//SPELL TYPES
#define SP_NOTLEARNT -1
#define SP_FIREBOLT 0 
#define SP_HEAL 1
#define SP_BANDAGEWOUNDS 2
#define SP_FROSTBITE 3
#define SP_CALM  4
#define SP_POLYMORPH 5
#define SP_DISARMING 6
#define SP_BINDINGVINES 7
#define SP_HYPNOTISE 8
#define SP_SONICBOOM 9
#define SP_FIRESTORM 10	//MASTER OF FIREBOLT
#define SP_BLIZZARD  11 //MASTER OF FROSTBITE
#define SP_TANGLINGMANGROVE 12
#define SP_MAXHEAL 13
#define SP_REPEL 14
#define SP_SHOCKWAVE 15
#define SP_SLICE 16
#define SP_TWIRLSTRIKE 17
#define SP_WHIRLWIND 18
#define SP_ARCANEBARRIER 19
#define SP_ARCANEABSORPTION 20
#define SP_DISTRACT //fight each other
#define SP_INVISIBILITY //not potion
#define MAXSPELL 21
//can also do buffs?

enum TRINKS
{
	TK_DRAGONTOOTH,
	MAXTRINKET
};

enum DRAGTYPES
{
	DRG_FIRE,
	DRG_ICE,
	DRG_POISON,
	NDRAG_TYPE
};
//#define DRG_MIGHTY  3
//#define DRG_HYPNO  	4

enum FLUID_TYPES
{
	AIR,
	COMBUST,
	MIST,
	MIASMA,
	SMOKE,
	NFLUID_TYPES
};

enum DAMAGE_TYPES
{
	DMG_PHYSICAL,
	DMG_MAGIC,
	DMG_FIRE,
	DMG_ICE,
	DMG_POISON
};


//ENTITY FLAGS
#define ISACTIVE   0x01
#define ISTILE     0x02
#define ISCREATURE 0x04
#define ISOBJ      0x08
#define ISONLEVEL  0x10
#define ISILLUSION 0x20
#define ALTCHARSET 0x40
#define ISMARKED   0x80

//OBJ FLAGS
#define OBSTRUCTS 0x01
#define CANSTACK  0x02
#define ISMISSILE 0x04
#define ISCURSED  0x08
#define ISIDENTIF 0x10

#define W_ONEHAND 0x100
#define W_TWOHAND 0x200
#define W_ARROW	  0x800

//CREATURE FLAGS
#define ISPLAYER 	0x01
#define ISAGRO   	0x02
#define ISWANDER 	0x04
#define ISFLY    	0x08
#define CANTRACK 	0x10 // this was a mistake
#define ISCONFUSED 	0x20
#define ISFRIEND 	0x40
#define ISINVIS  	0x80
#define ISPOISON 	0x100
#define ISSLEEP  	0x200
#define SEENPLAYER  0x400 
#define ISBURN	 	0x800
#define ISFREEZE 	0x1000
#define ISREGEN  	0x2000
#define ISBOUND  	0x4000
#define ISSPEED  	0x8000
#define ISSLOW   	0x10000
#define ISINKED  	0x20000
#define ISLSD	 	0x40000
#define ISSCARED 	0x80000
#define ISBLIND		0x100000
#define ISVULN		0x200000
#define ISBREATH	0x400000
#define ISSTUN 		0x800000
#define ISAPEX      0x1000000
#define ISFOLLOW    0x2000000
#define ISREBIRTH   0x4000000
#define ISARCANE    0x8000000

// resistance
#define RESIST_FIRE 	0x01
#define IMMUNE_FIRE 	0x02
#define WEAKTO_FIRE		0x04

#define RESIST_FROST 	0x10
#define IMMUNE_FROST 	0x20
#define WEAKTO_FROST	0x40

#define RESIST_POISON 	0x100
#define IMMUNE_POISON 	0x200
#define WEAKTO_POISON	0x400

#define RESIST_SLEEP	0x1000
#define IMMUNE_SLEEP	0x2000

enum TILE_COLOURS{
STATUS_BURN=1,
STATUS_FREEZE,
STATUS_POISON,
STATUS_REGEN,
STATUS_BIND,
STATUS_INK,
STATUS_ARC,

FLUID_COL_COMBUST,
FLUID_COL_MIST,
FLUID_COL_MIASMA,
FLUID_COL_SMOKE,

C_WHITE,
C_RED,
C_BLUE,
C_GREEN,
C_YELLOW,
C_CYAN,
C_MAGENTA,
C_BLACK,
};

//MONSTER FORM TYPES
#define DEFAULTFORM 0
#define gNORMAL 0
#define gPIKEMAN 1
#define gSHIELDBEARER 2
#define gCUTTHROAT 3

#define lNORMAL 0
#define lMARAUDER 1
#define lBERSERKER 2
#define lGRUNT 3
#define lLIEUTENANT 4

#define tNORAML 0
#define tFIRE 1
#define tFROST 2
#define tROT 3

#define vBAT 1

#define xPHYLACTERY 1

//#define LIMORTAL 0
//#define LVULNERABLE 1
//#define LMORTAL 2
#define LMAIN 0
#define LFIRST 1
#define LVULNERABLE 2

//TILE FLAGS
//map static
#define MS_EXPLORED 0x01
#define MS_HIDDEN   0x02

//map live
#define ML_VISIBLE  	0x01
#define ML_OBSTRUCTS 	0x02
#define ML_TRANSPARENT 	0x04
#define ML_LOCKED		0x08

//ROOM FLAGS
#define RM_ISACTIVE 0x01
#define RM_ISDARK 	0x02
#define RM_LITBYRING 0x04

//LEVEL FLAGS
#define L_VISITED    0x01
#define L_DARK	     0x02
#define L_RETURN	 0x04 // the same as L_VISITED but for the return journy
#define L_APEXED     0x08 // has an apex creature on it
#define L_SHOP       0x10


//daemon types
enum DAEMON_TYPES{
	D_NONE,
	D_REGEN,
	D_CONFUSE,
	D_INVIS,
	D_POISON,
	D_SLEEP,
	D_BURN,
	D_FASTREGEN,
	D_FREEZE,
	D_BUFSTR,
	D_BUFDEX,
	D_BIND,
	D_HASTE,
	D_SLOW,
	D_STRENGTH,
	D_LSD,
	D_VULNERABILITY,
	D_BLIND,
	D_INK,
	D_LICHRESPAWN,
	D_PACIFY,
	D_SLOWDEATH,
	D_WYVERNPOISON,
	D_DRAGONBREATH,
	D_STUN,
	D_TUTORIAL,
	D_SPAWNPHOENIX,
	D_ARCANEBARRIER,
	D_ABSORPTION
};

#define REGENDURATION 	15
#define CONFUSEDURATION 25
#define INVISDURATION 	25
#define FAINT_LIMIT 	50


enum ANIMS{
	A_NONE,
	A_PROJ,
	A_AOE,
	A_SHOOTDIR,
	A_LASER,
	A_TWIRL,
	A_SLICE
};

enum AIRTYPES{
	AIR_NORMAL,
	AIR_MIASMA,
	AIR_MIST,
	AIR_FLAME
};

typedef struct { int x,y,z; } coord;
typedef int dice_int[2];

typedef struct 
{
	int hp, maxhp;
	dice_int str;
	int dex,def,res;
} stats;

typedef struct
{
	char c;
	int flags; 
	struct entity *creature;
	struct entity *obj;

	int air;
   	float air_pressure;
	int neighbours[8];
} tile;

typedef struct
{
	int x,y;
	int w,h;
	int flags;
} room;

struct entity
{
	int id;
	int flags;
	coord pos;
	union components
	{
		struct
		{
			int flags;      // Status flags etc
			char type;      // what type of creature
			stats stat;     // hp etc
			int res_flags;  // resistant to elements
			int stamina;	// how much energy for more ai
			room *_inroom; 	// are you in a room
			int turn; 		// speed index
			int form; 		// form of creature type
			int parent;		// who spawned you
			int target; //hmmm
			int last_seen_player;
		} _creature;
		struct
		{
			int flags;
			char type; 	//SCROLL,FOOD,POTION
			int which;	//invis,health potion, etc
			int quantity;
			dice_int mod_melee; //dmg when used like sword
			dice_int mod_throw; //dmg when thrown
			int mod_def, mod_res;
			int enchant_level; 
			int potion_effect[2];
		} _object;
	}cmp;
};
typedef struct entity Entity;

typedef struct 
{
	int type;
	int charge, cooldown;
} spell;


//some handy hashdefs
#define _o cmp._object
#define _c cmp._creature
#define player (&db.creatures[0])
//#define dragon (&db.creatures[1])

#define tileat(x,y) (&db.tiles[(y)*XMAX+(x)])
#define moat(x,y) (tileat(x,y)->creature)
#define objat(x,y) (tileat(x,y)->obj)

#define cid(id) (&db.creatures[id])

#define obstructs(x,y)   (tileat(x,y)->flags&ML_OBSTRUCTS)
#define transparent(x,y) (tileat(x,y)->flags&ML_TRANSPARENT)

#define MAX(a,b) (((a)<(b))?(b):(a))
#define MIN(a,b) (((a)>(b))?(b):(a))

#define tflags(x,y) (db.levels[db.cur_level].tile_flags[(y)*XMAX+(x)])

#endif
