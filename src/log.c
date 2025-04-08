#include "diabaig.h"

void _log(const char *fmt,...)
{
	FILE *fp;
	char _logmsg[1028];
	va_list ap;
	
	if(verbose)
	{
		va_start(ap,fmt);
		if((fp=fopen("/tmp/diabaig.log","a"))==NULL)
		{
			perror("/tmp/diabaig.log");
		}
		vsnprintf(_logmsg,1028,fmt,ap);
		fprintf(fp,"%s\n",_logmsg);
		fflush(fp);
		fclose(fp);

		va_end(ap);
	}
}
