#include "extern.h"

void seed_time()
{
	srand(time(NULL));
}
int rng(int range)
{
	return range ? rand()%(abs(range)):0;
}

void splitdice(const char *dice, dice_int dest)
{
	/*
	char *ncpy=malloc(strlen(dice));
	strcpy(ncpy,dice);
	dest[0]=atoi(strsep(&ncpy,"d"));
	dest[1]=atoi(strsep(&ncpy,"d"));
	free(ncpy);
	*/
	char *tmp=calloc(strlen(dice)+1,sizeof(char));
	strcpy(tmp,dice);

	dest[0]=atoi(strtok(tmp,"d"));
	dest[1]=atoi(strtok(NULL,"d"));
	_log("%s (%d %d)",dice,dest[0],dest[1]);

	free(tmp);
}

int diceroll(dice_int dice)
{
	int val=0;
	for(int d=dice[0];d>0;d--) val+=(1+rng(dice[1]));
	return val;
}

int dicemax(dice_int dice)
{
	int val=dice[0]*dice[1];
	return val;
}

