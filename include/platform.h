#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef WINDOWS
#include <windows.h>
#include <pdcurses.h>

#define strdup _strdup

#else
#include <unistd.h>
#include <ncurses.h>
#endif

void platform_sleep(int t);

#endif

//#ifdef WINDOWS
//#include <ncursesw/ncurses.h>
//#else
//#include <ncurses.h>
//#endif
