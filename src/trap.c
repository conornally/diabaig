#include "diabaig.h"

int do_trap(tile *t, Entity *e)
{
	int status=RETURN_UNDEF;
	if(t && e && t->c==TRAP)
	{
		status=do_trap(t,e);
	}
	return status;
}

int _dotrap(tile *t, Entity *e)
{
	int status=RETURN_UNDEF;
	if(t && e)
	{
		status=RETURN_SUCCESS;
	}
	return status;
}
