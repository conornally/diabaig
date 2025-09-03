#ifndef PLATFORM_H
#define PLATFORM_H

#include <sys/stat.h>
#include <sys/types.h>

#ifdef WINDOWS
	#include <windows.h>
	#include <pdcurses.h>
	#define strdup _strdup
	#define getmouse nc_getmouse
	
	#include <direct.h>
	#define MKDIR(path,mode) _mkdir(path)
	#define STAT _stat

#else // NOT WINDOWS
	#include <unistd.h>
	#include <ncurses.h>
	
	#define MKDIR(path,mode) mkdir(path, mode)
	#define STAT stat

#endif

void platform_sleep(int t);

#endif
