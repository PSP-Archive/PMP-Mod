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


#ifndef me_idct_h
#define me_idct_h


extern unsigned int me_idct_start_row;


void me_idct_safe_constructor();
char *me_idct_p_malloc(unsigned int width, unsigned int height);
void me_idct_p_free();
void me_idct_start();
short *me_idct_new_macroblock();
unsigned int me_idct_block_index(short *block);
void me_idct_add(unsigned char put, unsigned char *dest, unsigned short line_size, unsigned short block);
void me_idct_row_run(int row);
void me_idct_row_run_in_background();
void me_idct_run();


#endif
