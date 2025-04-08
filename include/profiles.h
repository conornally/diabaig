#ifndef PROFILES_H
#define PROFILES_H

#define PROFILE_CMD_MAX 100

struct _profiler
{
	char *name;
	char *cmds[PROFILE_CMD_MAX];
};

extern struct _profiler _profiles[];

#endif
