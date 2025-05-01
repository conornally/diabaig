#ifndef AUTOPILOT_H
#define AUTOPILOT_H

struct _autopilot
{
	int active;
	int direction;
	int target;
};
extern struct _autopilot autopilot;

int start_autopilot();
int do_autopilot();

#endif
