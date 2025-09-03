#include "unistd.h"
#include "diabaig.h"

void usage()
{
	fprintf(stderr,"diabaig [-htvw] [-L loadfile] [-s seed] [-S file.scr] \n");
	if(verbose)
	{
		fprintf(stderr,"  -h : print help screen\n");
		fprintf(stderr,"  -L : load from file \"loadfile\"\n");
		//fprintf(stderr,"  -r : display highscore list\n");
		fprintf(stderr,"  -S : set a custom seed\n");
		fprintf(stderr,"  -s : set a custom highscore file\n");
		fprintf(stderr,"  -t : enter the testing arena\n");
		fprintf(stderr,"  -v : verbose output mode\n");
		fprintf(stderr,"  -w : wizard mode\n");
	}
	exit(1);
}

int main(int argc, char *argv[])
{
	int c;
	seed=time(NULL);
	wizardmode=0;
	testarena=0;

	//memset(loadfile, '\0', sizeof(char)*1028);

	while((c=getopt(argc,argv,"htvwl:S:s:"))!=EOF)
	{
		switch(c)
		{
			case 'h': usage(); break;
			case 's': seed=atoi(optarg); break;
			case 'S': snprintf(scorefile, 1028, "%s", optarg); break;
			case 't': testarena=1; break;
			case 'v': verbose=1; break;
			case 'w': wizardmode=1; break;
			case 'l': strncpy(loadfile,optarg,1028);
		}
	}
	argc--;
	argv++;

	srand(seed);
	init();

	int mode=-1;

	while(mode!=MENU_QUIT) // SYSTEM LOOP
	{
		init_world();
		mode=home();
		switch(mode)
		{
			case MENU_NEWGAME:
				if(init_player()) mode=-1;
				else running=true;
				break;

			case MENU_CONTINUE:
				if(!continue_screen())
				{
					running=true;
					mode=0;
				}
				else mode=-1;

				break;
				/*
				if(!load(loadfile))
				{
					running=true;
					mode=0;
				}
				else
				{
					mode=-1;

				}
				break;
				*/
			
			case MENU_HIGHSCORES:
				display_scores();
				break;

			case MENU_CREDITS:
				display_credits();
				break;

			case MENU_SETTINGS:
				conf_set();
				break;

			case MENU_QUIT:
				break;

			case 6:
				seed=time(NULL);
				init_world();
				break;
			default: break;
		}

		wclear(win);
		if( (mode==0) || (mode==1)) mainloop();
		wclear(win);
	}

	revert();

	return 0;
}
