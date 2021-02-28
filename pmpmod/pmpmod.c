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


#include <pspdebug.h>
#include <psppower.h>
#include "avformat.h"
#include "me.h"
#include "pmp.h"
#include "opendir.h"


PSP_MODULE_INFO("pmpmod", 0x1000, 1, 1);
PSP_MAIN_THREAD_ATTR(0);


#define video_directory "ms0:/PSP/VIDEO/"


void pmp_pause()
	{
	while (1)
		{
		SceCtrlData controller;
		sceCtrlReadBufferPositive(&controller, 1);

		if (controller.Buttons & PSP_CTRL_CROSS || controller.Buttons & PSP_CTRL_TRIANGLE)
			{
			break;
			}
		}
	}


void screen_init()
	{
	pspDebugScreenSetTextColor(0xffffff);
	pspDebugScreenSetBackColor(0x000000);
	pspDebugScreenInit();
	}


void print_error(char *s)
	{
	screen_init();
	pspDebugScreenSetXY(0, 0);
	pspDebugScreenPrintf("Error:\n");
	pspDebugScreenPrintf("\n");
	pspDebugScreenPrintf("\"%s\"\n", s);
	pspDebugScreenPrintf("\n");
	pspDebugScreenPrintf("Press X");
	pmp_pause();
	}


char *static_init()
	{
	scePowerSetClockFrequency(333, 333, 166);
	scePowerLock(0);


	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);


	av_register_all();


	if (me_struct_init() == 0)
		{
		return("static_init: me_struct_init failed");
		}


	return(0);
	}


int init()
	{
	char *result = static_init();
	if (result != 0)
		{
        print_error(result);

		return(0);
		}


	return(1);
	}


void print_pmp(int licence)
	{
	screen_init();
	pspDebugScreenSetXY(0, 0);
	pspDebugScreenPrintf("PMP Mod v2.01 by jonny\n");
	pspDebugScreenPrintf("\n");
	pspDebugScreenPrintf("Press TRIANGLE to exit");
	
	
	if (licence == 0)
		{
		pspDebugScreenPrintf(", X or O to play, SQUARE to see the licence");
		}


	pspDebugScreenSetXY(0, 23);
	pspDebugScreenPrintf("PMP Mod uses libavcodec from FFMPEG (http://ffmpeg.sourceforge.net/)\n");
	pspDebugScreenPrintf("Many thanks goes to:\n");
	pspDebugScreenPrintf("\n");
	pspDebugScreenPrintf("- FFMPEG developers\n");
	pspDebugScreenPrintf("- JiniCho (PMP Mod is a modified version of his original FFMPEG and\n");
	pspDebugScreenPrintf("  FFPLAY psp port)\n");
	pspDebugScreenPrintf("- ps2dev.org developers and users\n");
	pspDebugScreenPrintf("\n");
	pspDebugScreenPrintf("                                   http://jonny.leffe.dnsalias.com/");
	}


void print_licence()
	{
	pspDebugScreenPrintf("This program is free software; you can redistribute it and/or modify");
	pspDebugScreenPrintf("it under the terms of the GNU General Public License as published by");
	pspDebugScreenPrintf("the Free Software Foundation; either version 2 of the License, or\n");
	pspDebugScreenPrintf("(at your option) any later version.\n");
	pspDebugScreenPrintf("\n");
	pspDebugScreenPrintf("This program is distributed in the hope that it will be useful,\n");
	pspDebugScreenPrintf("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
	pspDebugScreenPrintf("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
	pspDebugScreenPrintf("GNU General Public License for more details.\n");
	pspDebugScreenPrintf("\n");
	pspDebugScreenPrintf("You should have received a copy of the GNU General Public License\n");
	pspDebugScreenPrintf("along with this program; if not, write to the Free Software\n");
	pspDebugScreenPrintf("Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n");
	pspDebugScreenPrintf("\n");
	}


void main_loop()
	{
	struct opendir_struct directory;

	char *result = opendir_open(&directory, video_directory);
	if (result != 0)
		{
		print_pmp(1);
		pspDebugScreenSetXY(0, 4);
		pspDebugScreenPrintf("\"%s\" not found or empty", video_directory);
		pspDebugScreenSetXY(0, 8);
		print_licence();
		pmp_pause();
		return;
		}




	int selected_entry         = 0;
	int top_entry              = 0;
	int maximum_number_of_rows = 16;
	int starting_row           = 4;


	while (1)
		{
		print_pmp(0);




        SceCtrlData controller;

		while (1)
			{
			sceCtrlReadBufferPositive(&controller, 1);

			if (controller.Buttons & PSP_CTRL_CROSS || controller.Buttons & PSP_CTRL_CIRCLE || controller.Buttons & PSP_CTRL_TRIANGLE || controller.Buttons & PSP_CTRL_SQUARE)
				{
				break;
				}


			if (controller.Buttons & PSP_CTRL_DOWN)
				{
				if (selected_entry + 1 < directory.number_of_directory_entries)
					{
					selected_entry++;

					if (selected_entry == top_entry + maximum_number_of_rows)
						{
						top_entry++;
						}
					}
				}


			if (controller.Buttons & PSP_CTRL_UP)
				{
				if (selected_entry != 0)
					{
					selected_entry--;

					if (selected_entry == top_entry - 1)
						{
						top_entry--;
						}
					}
				}




			pspDebugScreenSetXY(1, starting_row);
			pspDebugScreenSetTextColor(0xffffff);
			pspDebugScreenSetBackColor(0xaa4400);
			pspDebugScreenPrintf("%-66.66s", "");


			int i = 0;
			for (; i < maximum_number_of_rows; i++)
				{
				int current_entry = top_entry + i;


				pspDebugScreenSetXY(1, starting_row + i + 1);

				if (current_entry < directory.number_of_directory_entries)
					{
					if (current_entry == selected_entry)
						{
						pspDebugScreenSetTextColor(0xffffff);
						pspDebugScreenSetBackColor(0x882200);
						}
					else
						{
						pspDebugScreenSetTextColor(0xffffff);
						pspDebugScreenSetBackColor(0xcc6600);
						}

					pspDebugScreenPrintf("%-66.66s", directory.directory_entry[current_entry].d_name);
					}
				else
					{
					pspDebugScreenSetTextColor(0xffffff);
					pspDebugScreenSetBackColor(0xaa4400);
					pspDebugScreenPrintf("%-66.66s", "");
					}
				}


			pspDebugScreenSetXY(1, starting_row + maximum_number_of_rows + 1);
			pspDebugScreenSetTextColor(0xffffff);
			pspDebugScreenSetBackColor(0xaa4400);
			pspDebugScreenPrintf("%-66.66s", "");


			sceKernelDelayThread(100000);
			}




		if (controller.Buttons & PSP_CTRL_TRIANGLE)
			{
			break;
			}


		if (controller.Buttons & PSP_CTRL_SQUARE)
			{
			screen_init();
			pspDebugScreenSetXY(0, 0);
			print_licence();
			pspDebugScreenPrintf("Press X");
			pmp_pause();
			screen_init();
			sceKernelDelayThread(1000000);
			continue;
			}




		screen_init();
		pspDebugScreenSetXY(0, 0);
		pspDebugScreenPrintf("Loading ...");


		char *result = pmp_play(directory.directory_entry[selected_entry].d_name);
		if (result != 0)
			{
			sceKernelDelayThread(1000000);
			print_error(result);
			screen_init();
			sceKernelDelayThread(1000000);
			}
		}




	opendir_close(&directory);
	}


int main(int unused0, char *unused1[])
	{
	if (init())
		{
		main_loop();
		}


	sceKernelExitGame();
	return(0);
	}
