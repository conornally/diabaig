#include "diabaig.h"

/*
 * I kind of messed this up from the start by making the 
 * architecture really awful. Rather than fixing it and 
 * potentially breaking things, i am just going to try and
 * deal with it in the save/load stuff here.
 * Realistically though, it wont be perfect
 */

int _save(const char *fname)
{
	int status=0;
	FILE *fp=NULL;
	if(!(fp=fopen(fname,"wb")))
	{
		perror(fname);
		status=1;
	}
	else
	{
		// Do I actually still need to do this?
		for(int id=0; id<(XMAX*YMAX);id++) 
		{
			db.tiles[id].creature=NULL;
			db.tiles[id].obj=NULL;
		}

		fwrite(&db,sizeof(struct _database),1,fp);
		fwrite(&verbose,sizeof(verbose),1,fp);
		fwrite(&message,sizeof(message),1,fp);
		fwrite(&message_queue,sizeof(message_queue),1,fp);
		fwrite(&message_history,sizeof(message_history),1,fp);
		fwrite(&playername,sizeof(playername),1,fp);
		
		fwrite(&wizardmode,sizeof(wizardmode),1,fp);
		fwrite(&testarena,sizeof(testarena),1,fp);
		fwrite(&bfs_nodes,sizeof(bfs_nodes),1,fp);
		fwrite(&path_memory,sizeof(bfs_nodes),1,fp);


		fwrite(&scroll_info,sizeof(scroll_info),1,fp);
		fwrite(&potion_info,sizeof(potion_info),1,fp);
		fwrite(&ring_info,sizeof(ring_info),1,fp);
		fwrite(&spell_info,sizeof(spell_info),1,fp);
		
		fwrite(&potion_colours,sizeof(potion_colours),1,fp);
		fwrite(&metal_types,sizeof(potion_colours),1,fp);

		fclose(fp);
	}
	return status;
}

int save(const char *fname)
{
	int status=RETURN_SUCCESS;
	int input='n';
	msg("saving to %s: y/n?",fname);
	display();
	if((input=getch())=='y')
	{
		status=_save(fname);
		if(status) msg("save failed");
		else
		{
			msg("saved to diabaig.save");
			running=0;
		}
	}
	else
	{
		status=RETURN_UNDEF;
		msg("not saved");
	}

	return status;
}

void backup()
{
	_save("/tmp/diabaig.backup");
}


int _load(const char *fname)
{
	int status=0;
	FILE* fp=NULL;
	if(!(fp=fopen(fname,"rb")))
	{
		perror(fname);
		status=1;
	}
	else
	{
		fread(&db,sizeof(struct _database),1,fp);
		fread(&verbose,sizeof(verbose),1,fp);
		fread(&message,sizeof(message),1,fp);
		fread(&message_queue,sizeof(message_queue),1,fp);
		fread(&message_history,sizeof(message_history),1,fp);
		fread(&playername,sizeof(playername),1,fp);

		fread(&wizardmode,sizeof(wizardmode),1,fp);
		fread(&testarena,sizeof(testarena),1,fp);
		fread(&bfs_nodes,sizeof(bfs_nodes),1,fp);
		fread(&path_memory,sizeof(bfs_nodes),1,fp);

		fread(&scroll_info,sizeof(scroll_info),1,fp);
		fread(&potion_info,sizeof(potion_info),1,fp);
		fread(&ring_info,sizeof(ring_info),1,fp);
		fread(&spell_info,sizeof(spell_info),1,fp);

		fread(&potion_colours,sizeof(potion_colours),1,fp);
		fread(&metal_types,sizeof(potion_colours),1,fp);


		// HACK the map back into shape
		Entity* e;
		int id;
		for(id=0; id<DBSIZE_CREATURES; id++)
		{
			e=&db.creatures[id];
			if(e && (e->flags&ISACTIVE))
			{
				db.tiles[e->pos.y*XMAX+e->pos.x].creature=e;
				inroom(e);
			}
		}
		for(id=0; id<DBSIZE_OBJECTS; id++)
		{
			e=&db.objects[id];
			if(e && (e->flags&ISACTIVE))
			{
				int ininv=0;
				for(int iid=0;iid<26;iid++)
				{
					if(db.inventory[iid]==id) ininv=1;
				}
				if(!ininv) objat(e->pos.x, e->pos.y)=e;
			}
		}
		sprintf(message_queue[1], "successfully loaded: ");
		strncat(message_queue[1], loadfile, MSGSZ-strlen(message_queue[1])-1);
		//snprintf(message_queue[1], MSGSZ, "successfully loaded from %s",loadfile);
		fclose(fp);
	}


	return status;
}

int load(const char *fname)
{
	int status=0;
	//_log("loaded %s: %d",fname, status);
	status=_load(fname);
	if(!status)
	{
		remove(fname);
	}
	return status;
}
