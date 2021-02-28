/*
PMP Mod
Copyright (C) 2006 jonny

Homepage: http://jonny.leffe.dnsalias.com
E-mail:   jonny@leffe.dnsalias.com

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
"opendir" c code (~3 lines of perl -> ~100 lines of c :s)
*/


#include "opendir.h"


static SceIoDirent directory_entry;


void opendir_safe_constructor(struct opendir_struct *p)
	{
	p->directory = -1;

	p->directory_entry = 0;
	}


void opendir_close(struct opendir_struct *p)
	{
	if (!(p->directory < 0)) sceIoDclose(p->directory);

	if (p->directory_entry != 0) free_64(p->directory_entry);


	opendir_safe_constructor(p);
	}


char *opendir_open(struct opendir_struct *p, char *directory)
	{
	opendir_safe_constructor(p);


	if (chdir(directory) < 0)
		{
		opendir_close(p);
		return("opendir_open: chdir failed");
		}




	p->directory = sceIoDopen(directory);
	if (p->directory < 0)
		{
		opendir_close(p);
		return("opendir_open: sceIoDopen failed");
		}




	unsigned int number_of_directory_entries = 0;


	while (1)
		{
		memset(&directory_entry, 0, sizeof(SceIoDirent));
		int result = sceIoDread(p->directory, &directory_entry);

		if (result == 0)
			{
			break;
			}
		else if (result > 0)
			{
			number_of_directory_entries++;
			}
		else
			{
			opendir_close(p);
			return("opendir_open: sceIoDread failed");
			}
		}




	sceIoDclose(p->directory);
	p->directory = -1;




	p->directory = sceIoDopen(directory);
	if (p->directory < 0)
		{
		opendir_close(p);
		return("opendir_open: sceIoDopen failed");
		}


	p->directory_entry = malloc_64(sizeof(SceIoDirent) * number_of_directory_entries);
	if (p->directory_entry == 0)
		{
		opendir_close(p);
		return("opendir_open: malloc_64 failed on directory_entry");
		}




	p->number_of_directory_entries = 0;


	int i = 0;

	for (; i < number_of_directory_entries; i++)
		{
		memset(&directory_entry, 0, sizeof(SceIoDirent));
		int result = sceIoDread(p->directory, &directory_entry);

		if (result == 0)
			{
			break;
			}
		else if (result > 0)
			{
			p->directory_entry[p->number_of_directory_entries] = directory_entry;

			if (FIO_SO_ISREG(p->directory_entry[p->number_of_directory_entries].d_stat.st_attr))
				{
				p->number_of_directory_entries++;
				}
			}
		else
			{
			opendir_close(p);
			return("opendir_open: sceIoDread failed");
			}
		}


	sceIoDclose(p->directory);
	p->directory = -1;




	if (p->number_of_directory_entries == 0)
		{
		opendir_close(p);
		return("opendir_open: number_of_directory_entries == 0");
		}


	return(0);
	}
