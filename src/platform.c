#include "platform.h"

void platform_sleep(int t)
{
#ifdef WINDOWS
	Sleep(t/1000);
#else
	usleep(t);
#endif

}
