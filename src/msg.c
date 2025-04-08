#include "diabaig.h"

static void _push_message(const char *_msg);


void msg(const char *fmt,...)
{
	va_list ap;
	char *dest=message_queue[1];
	va_start(ap,fmt);
	
	memcpy(message,message_queue[0],sizeof(message));
	memcpy(message_queue[0],message_queue[1],sizeof(message));
	//if(message[0]) dest=message_queue;
	vsnprintf(dest,MSGSZ,fmt,ap);
	_push_message(message_queue[1]);
	va_end(ap);

	messaged++;
}

void player_msg(Entity *e, const char *fmt,...)
{
	if(e==player)
	{
		msg(fmt);
	}
}

static void _push_message(const char *_msg)
{
	if(_msg[0])
	{
		for(int i=25;i>0;i--) strcpy(message_history[i], message_history[i-1]);
		strcpy(message_history[0],message_queue[1]);
	}
}
