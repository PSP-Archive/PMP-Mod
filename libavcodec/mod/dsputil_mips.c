/*
 * (was) MMI optimized DSP utils
 * Copyright (c) 2000, 2001 Fabrice Bellard.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * MMI optimization by Leon van Stuivenberg
 * clear_blocks_mmi() by BroadQ
 *
 *
 * i've done only tiny modifications over the JiniCho code
 * the full set should be asm written (put(_no_rnd)_pixels{,_x2,_y2,_xy2} - avg_pixels{,_x2,_y2,_xy2})
 * those functions are a bit boring :)
 *
 *                             (jonny)
 */


#include "avcodec.h"
#include "dsputil.h"


static void clear_blocks_mips(DCTELEM *p)
	{
	asm __volatile__
		(
		".set push\n"
		".set noreorder\n"

		"li $2, 12\n"

		"1:\n"

		"  sub $2, $2, 1\n"

		"  sw $0,  0(%0)\n"
		"  sw $0,  4(%0)\n"
		"  sw $0,  8(%0)\n"
		"  sw $0, 12(%0)\n"
		"  sw $0, 16(%0)\n"
		"  sw $0, 20(%0)\n"
		"  sw $0, 24(%0)\n"
		"  sw $0, 28(%0)\n"
		"  sw $0, 32(%0)\n"
		"  sw $0, 36(%0)\n"
		"  sw $0, 40(%0)\n"
		"  sw $0, 44(%0)\n"
		"  sw $0, 48(%0)\n"
		"  sw $0, 52(%0)\n"
		"  sw $0, 56(%0)\n"
		"  sw $0, 60(%0)\n"

		"  bnez $2, 1b\n"
		"  addiu %0, %0, 64\n"

		".set pop\n"

		:
		: "r" (p)
		: "$2"
		);
	}


static void put_pixels8_mips(uint8_t *block, const uint8_t *pixels, int line_size, int h)
	{
	if (((int) pixels & 3) == 0)
		asm __volatile__
			(
			".set push\n"
			".set noreorder\n"

			// %0 = block
			// %1 = pixels
			// %2 = line_size
			// %3 = h

			"1:\n"

			"  sub %3, %3, 1\n"

			"  lw $2, 0(%1)\n"
			"  sw $2, 0(%0)\n"

			"  lw $3, 4(%1)\n"
			"  sw $3, 4(%0)\n"

			"  addu %1, %1, %2\n"

			"  bnez %3, 1b\n"
			"  addu %0, %0, %2\n"

			".set pop\n"

			:
			: "r" (block), "r" (pixels), "r" (line_size), "r" (h)
			: "$2", "$3"
			);
	else
		asm __volatile__
			(
			".set push\n"
			".set noreorder\n"

			// %0 = block
			// %1 = pixels
			// %2 = line_size
			// %3 = h

			"1:\n"

			"  lwl $2, 0+3(%1)\n"
			"  lwl $3, 4+3(%1)\n"

			"  sub %3, %3, 1\n"

			"  lwr $2, 0(%1)\n"
			"  lwr $3, 4(%1)\n"

			"  addu %1, %1, %2\n"

			"  sw $2, 0(%0)\n"
			"  sw $3, 4(%0)\n"

			"  bnez %3, 1b\n"
			"  addu %0, %0, %2\n"

			".set pop\n"

			:
			: "r" (block), "r" (pixels), "r" (line_size), "r" (h)
			: "$2", "$3"
			);
	}


static void put_pixels16_mips(uint8_t *block, const uint8_t *pixels, int line_size, int h)
	{
	asm __volatile__
		(
		".set push\n"
		".set noreorder\n"

		// %0 = block
		// %1 = pixels
		// %2 = line_size
		// %3 = h

		"1:\n"

		"  lwl $2,  0+3(%1)\n"
		"  lwl $3,  4+3(%1)\n"
		"  lwl $4,  8+3(%1)\n"
		"  lwl $5, 12+3(%1)\n"

		"  sub %3, %3, 1\n"

		"  lwr $2,  0(%1)\n"
		"  lwr $3,  4(%1)\n"
		"  lwr $4,  8(%1)\n"
		"  lwr $5, 12(%1)\n"

		"  addu %1, %1, %2\n"

		"  sw $2,  0(%0)\n"
		"  sw $3,  4(%0)\n"
		"  sw $4,  8(%0)\n"
		"  sw $5, 12(%0)\n"

		"  bnez %3, 1b\n"
		"  addu %0, %0, %2\n"

		".set pop\n"

		:
		: "r" (block), "r" (pixels), "r" (line_size), "r" (h)
		: "$2", "$3", "$4", "$5"
		);
	}


void dsputil_init_mips(DSPContext *c, AVCodecContext *avctx)
	{
    c->clear_blocks = clear_blocks_mips;

    c->put_pixels_tab[0][0] = put_pixels16_mips;
    c->put_pixels_tab[1][0] = put_pixels8_mips;

    c->put_no_rnd_pixels_tab[0][0] = put_pixels16_mips;
    c->put_no_rnd_pixels_tab[1][0] = put_pixels8_mips;
	}
