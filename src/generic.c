#include "generic.h"

int len(int *lst)
{
	int *l=lst;
	while(*l) l++;
	return (int)(l-lst);
}
int clen(char *lst[])
{
	int total=0;
	while(lst[total]) total++;
	return total;
}
