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
idct done with the psp me
*/


#include <stdio.h>
#include <stdlib.h>
#include <pspkernel.h>
#include "me.h"
#include "dsputil.h"
#include "me_idct.h"
#include "simple_idct.h"
#include "mem64.h"
#include "h263dec.h"


#define idct_put saved_simple_idct_put
#define idct_add saved_simple_idct_add


// 480 / 16  -  272 / 16
int max_width  = 30;
int max_height = 17;


static short          *me_idct_p_macroblocks;
static unsigned int    me_idct_p_macroblocks_index;


static unsigned char **me_idct_p_dest[32];
static unsigned int   *me_idct_p_data[32];
static unsigned int    me_idct_count [32];
unsigned int           me_idct_start_row;




void me_idct_safe_constructor()
	{
	me_idct_p_macroblocks = 0;

	int i = 0;
	for (; i < 32; i++)
		{
		me_idct_p_dest[i] = 0;
		me_idct_p_data[i] = 0;
		}
	}


static void safe_free_64(void *p)
	{
	if (p != 0) free_64(p);
	}


void me_idct_p_free()
	{
	safe_free_64(me_idct_p_macroblocks);

	int i = 0;
	for (; i < 32; i++)
		{
		safe_free_64(me_idct_p_dest[i]);
		safe_free_64(me_idct_p_data[i]);
		}


	me_idct_safe_constructor();
	}


char *me_idct_p_malloc(unsigned int width, unsigned int height)
	{
	me_idct_safe_constructor();


	max_width  = (width  + 15) >> 4;
	max_height = (height + 15) >> 4;


	me_idct_p_macroblocks = malloc_64(64 * sizeof(short) * 6 * max_width * max_height);
	if (me_idct_p_macroblocks == 0)
		{
		me_idct_p_free();
		return("me_idct_p_malloc: malloc_64 failed on me_idct_p_macroblocks");
		}


	int i = 0;
	for (; i < max_height; i++)
		{
		me_idct_p_dest[i] = malloc_64(sizeof(unsigned char *) * 6 * max_width);
		if (me_idct_p_dest[i] == 0)
			{
			me_idct_p_free();
			return("me_idct_p_malloc: malloc_64 failed on me_idct_p_dest");
			}


		me_idct_p_data[i] = malloc_64(sizeof(unsigned int)    * 6 * max_width);
		if (me_idct_p_data[i] == 0)
			{
			me_idct_p_free();
			return("me_idct_p_malloc: malloc_64 failed on me_idct_p_data");
			}
		}


	return(0);
	}


void me_idct_start()
	{
	me_idct_p_macroblocks_index = 0;
	me_idct_start_row           = 0;

	int i = 0;
	for (; i < max_height; i++)
		{
		me_idct_count[i] = 0;
		}
	}


short *me_idct_new_macroblock()
	{
	short *new_macroblock = &me_idct_p_macroblocks[me_idct_p_macroblocks_index];

	me_idct_p_macroblocks_index += 6 * 64;

	return(new_macroblock);
	}


unsigned int me_idct_block_index(short *block)
	{
	return((block - me_idct_p_macroblocks) >> 6);
	}


/*
|--|---|--------------|-----|
|  |put|line_size >> 6|block|
|--|---|--------------|-----|
|11|  1|             4|   16|
|--|---|--------------|-----|
*/

void me_idct_add(unsigned char put, unsigned char *dest, unsigned short line_size, unsigned short block)
	{
	me_idct_p_dest[me_idct_mb_y][me_idct_count[me_idct_mb_y]] = dest;
	me_idct_p_data[me_idct_mb_y][me_idct_count[me_idct_mb_y]] = (put << 20) | (line_size << 10) | block;

	me_idct_count[me_idct_mb_y]++;
	}


void me_idct_row_run(int row)
	{
	unsigned int i = 0;

	for (; i < me_idct_count[row]; i++)
		{
		if (me_idct_p_data[row][i] & 0x100000)
			{
			idct_put(me_idct_p_dest[row][i], (me_idct_p_data[row][i] & 0xf0000) >> 10, me_idct_p_macroblocks + ((me_idct_p_data[row][i] & 0xffff) << 6));
			}
		else
			{
			idct_add(me_idct_p_dest[row][i], (me_idct_p_data[row][i] & 0xf0000) >> 10, me_idct_p_macroblocks + ((me_idct_p_data[row][i] & 0xffff) << 6));
			}
		}
	}


static void me_idct_alternate_rows(int start)
	{
	int i = start;
	for (; i < max_height; i += 2)
		{
		me_idct_row_run(i);
		}
	}


static void me_idct_alternate_rows_with_me(int start)
	{
	me_idct_alternate_rows(start);
	me_sceKernelDcacheWritebackInvalidateAll();
	}


static void me_idct_row_run_with_me(int start)
	{
	me_idct_row_run(start);
	me_sceKernelDcacheWritebackInvalidateAll();
	}


void me_idct_row_run_in_background()
	{
	sceKernelDcacheWritebackInvalidateAll();

	me_start((int) &me_idct_row_run_with_me, me_idct_start_row);
	me_idct_start_row++;
	}


static void me_idct_flush()
	{
	if (me_idct_start_row < max_height)
		{
		sceKernelDcacheWritebackInvalidateAll();

		me_start((int) &me_idct_alternate_rows_with_me, me_idct_start_row + 1);
		me_idct_alternate_rows(me_idct_start_row);
		me_wait();
		}
	}


void me_idct_run()
	{
	while (!me_unused())
		{
		if (me_idct_start_row < max_height)
			{
			me_idct_row_run(me_idct_start_row);
			me_idct_start_row++;
			}
		}

	me_idct_flush();
	}
