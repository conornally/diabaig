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
	#define SETENV _putenv_s

#else // NOT WINDOWS
	#include <unistd.h>
	#include <ncurses.h>
	
	#define MKDIR(path,mode) mkdir(path, mode)
	#define STAT stat
	#define SETENV setenv

#endif

void platform_sleep(int t);

#endif
