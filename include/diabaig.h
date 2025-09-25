#ifndef DIABAIG_H
#define DIABAIG_H

#ifndef VERSION
#define VERSION "unknown"
#endif

#include <math.h>
#include <limits.h>

#include "platform.h"
#include "generic.h"
#include "entity.h"
#include "extern.h"
#include "profiles.h"
#include "data_embedded.h"

//tile* tileat(int x, int y);
//Entity* moat(int x, int y);
//Entity* objat(int x, int y);

int pick_one(obj_info *info, int nitems);
int weighted_pick(int list[], int length);
//obj_info *getinfo(int type);
char *getname(Entity *e);
char *getbasic_name(Entity *e);
char *get_prefix(const char *name);
int wrapline(char *src, char *dst, size_t len);

Entity *_new_obj(int type);
Entity *new_obj();
Entity *_new_monster(int type);
Entity *new_monster();
Entity *new_player();
void _set_weapon(Entity *e, int which);
void _set_armour(Entity *e, int which);
void _set_potion(Entity *e, int which);
void _set_scroll(Entity *e, int which);
void _set_ring(Entity *e, int which);
void _set_trinket(Entity *e, int which);
void _set_food(Entity *e, int which);
void set_item(Entity *e, int which);
Entity *new_weapon(int which);
Entity *new_armour(int which);
Entity *split_stack(Entity *e);
void clear_entity(Entity *e);
void item_info(Entity *e);

void init();
void reset_world();
void init_db();
void init_world();
void init_guesses();
void init_pathfinding();
int  init_player();
void init_playername();
void revert();
void display_tutorial();
void display_credits();
void display_scores();
void display_dijkstra();
int simple_menu(char *lst[], int len, int x, int y);
void set_class(classes cls);

void find_neighbours();
void construct_level(level *lvl);
void spawnroom();
void treasureroom(room *r);
void scholarroom(room *r);
void warriorroom(room* r);
void dragonnest(room* r);
void primaryboss_room(room* r);
void shoproom(room *r);
void regen_shop(int num); 

//void build_level_testingarena();

bool islegal(coord p);
int has_ring(int which);
int has_wep(int which);

int update();

void display();
void boss_bar(Entity *e);
void display_file(const char *fname);
void display_dathead(const char dat[], const int size);
void display_frameheader(char *s);
void msg(const char *fmt,...);
void player_msg(Entity *e,const char *fmt,...);
int get_efflevel();

void show_performance();
void show_inventory();
void draw_wee_guy(int x, int y);
Entity *menuselect(int type, const char *header);
void show_help();
void show_message_history();
void identify_screen();
void colour_check();
int pick_direction();

int home();
void mainloop();

//system
int walk(Entity *e, int direction);
int stumble(Entity *e);
int descend(Entity *e);
int ascend(Entity *e);
int melee(Entity *e, Entity *target);
int knockback(Entity *e, Entity *target, float strength, int direction);
int interact(Entity* e, Entity* target);
int pickup();
void _drop(Entity *item);
int drop();
int equip();
int toggle_equip();
int drink();
int read_scroll();
int throw_item();
int fire_bow();
void show_discovered();
int eat();
int search();
int apply_potion();
int wizard_console();
int quit();

int continue_screen();
int save();
int load(const char *fname);
void autosave();
void delete_autosave();

//spell stuff
int pick_spell();
int learn_spell(int type);
int cast_spell(int slot);
int modify_damage(Entity *target, int damage, int type);

void faint();
void starve(); 

int getdirection(coord a, coord b);
int get_first_thing_direction(coord pos, int direction);
int get_wall_direction(coord pos, int direction);
tile *get_tile_direct(Entity *src, int direction);
Entity *get_target_direct(Entity *src, int direciton);
Entity **get_target_radius(Entity *src, int radius);
Entity **get_target_room(Entity *src);
Entity **get_target_adjacent(Entity *src);
Entity **get_target_type(int type);
Entity ** get_target_visible();

int _do_applypotion(Entity *item, Entity *potion);
int _dodrink(Entity *target, Entity *item);
int _doread(Entity *target, Entity *item);
int _doeat(Entity *target, Entity *item);
void _throw(Entity *e, Entity *item, int dir);
int _equip(int id);
int _do_dequip(int id);
int _do_equip_ring(Entity *e);
int _do_unequip_ring(Entity *e);
void _do_weapon_effect(Entity *weapon, Entity *target, int melee);
void _wizard_profiles(struct _profiler *p);
int enchant(Entity* e);
int disenchant(Entity* e);

void murder(Entity *e, Entity *target);
void player_die();
void player_rebirth();
void do_addxp(Entity *e);
void levelup();

int use(Entity *e);
int add_daemon(Entity *e, int type, int duration);
void update_daemon(_daemon *d);
void end_daemon(_daemon *d);
void stop_daemon(_daemon *d);
_daemon *search_daemon(Entity *e, int type);
int status_system(Entity *e);

void light_room(room *r);
void unlight_room(room *r);
void light_local_area();
void placeat(Entity *e, int x, int y);
void placeinroom(room *r, Entity *e);
void seen(int x, int y);
room *inroom(Entity *e);
void lock_doors(room *r);
void unlock_doors(room *r);
void spawn_monster_inroom(room *r);

//ai
bool tile_lineofsight(Entity *e, int tile_id);
bool lineofsight(Entity *e, Entity *target);
void bfs(nav_node nodemap[], int nitems, int root);
void bfs_flood(nav_node nodemap[], int nitems, int root);
int getpath(nav_node nodemap[], int from);
int wander(Entity *e);
int track_player(Entity *e);
int do_agro(Entity *e);
int flee(Entity *e, Entity *target);
int hunt(Entity *e, Entity *target);

nav_node *dijk_new();
void dijk_reset(nav_node map[]);
void dijk_addsrc(nav_node map[], int id, float weight);
void dijk_scan(nav_node map[]);
void dijk_combine(float *dest, float *add);
int  dijk_getpath(Entity *e, nav_node *map);
void dijk_scale(nav_node nodemap[], float factor);
void dijk_place_entity(nav_node map[], Entity *e);

void air_diffuse(float decay, float noise);

void do_first_sight(Entity *e);
Entity *_spawn_adds(Entity *e, int type, int number);
void _ai_alchemist(Entity *e);
void _ai_cat(Entity *e);
void _ai_cat_offscreen(Entity *e);
void _ai_druid(Entity *e);
void _ai_illusionist(Entity *e);
void _ai_necromancer(Entity *e);
void _ai_vampire(Entity *e);
void _ai_witch(Entity *e);
void _ai_yeti(Entity *e);
void _ai_Lich(Entity *e);
void _ai_MOD(Entity *e);
void _ai_Banshee(Entity *e);
void _ai_ElderDragon(Entity *e);
void _ai_FellBeast(Entity *e);
void _ai_Lindworm(Entity *e);
void _ai_Nightmare(Entity *e);
void _ai_ObsidianLizard(Entity *e);
void _ai_Tezcatlipoca(Entity *e);
void _ai_Quetzalcoatl(Entity *e);
void _ai_Jaguar(Entity *e);
void _ai_VenusFT(Entity *e);
void _ai_Zealot(Entity *e);

void gen_dragonname(char* name, int form);
void _ai_dragon(Entity* e);
void _ai_youngdragon(Entity* e);
void _ai_dragon_offscreen(Entity *e);
int dragon_breath(Entity* e, Entity* target);
void gen_dragon();
Entity* getdragon();
int checkfortooth();

struct queue new_queue();
void push_queue(struct queue *q, int id);
int pull_queue(struct queue *q);

void reset_node(nav_node *node);

int animate(animation* a);
//void add_animation(animation a);
//int run_animate();

void display_scores();

struct conf conf_default();
int conf_save(const char *fname);
int conf_load(const char *fname);
void conf_set();

struct _autopilot
{
	int active; 
	int direction;
	int target;
	int ignore;
	int mode;
	int step;

	int c_adjacent; //is there a creature adjacent
	int c_inroom;   //is there a creature in the room
	int o_adjacent; //is there an object adjacent

	nav_node map[XMAX*YMAX]; //the dijk map to follow
};
extern struct _autopilot autopilot;

int start_autopilot();
int do_autopilot();
void stop_autopilot();

int autodirection(int direction);
int autoexplore();
int autorest();
int automouse(int x, int y);

#endif
