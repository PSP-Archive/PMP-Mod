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
color space conversion / tables generator
*/




/*
R = (Y - 16) * (255 / 219) + (V - 128) * (1.40200 * 255 / 224)
G = (Y - 16) * (255 / 219) - (U - 128) * (0.34414 * 255 / 224) - (V - 128) * (0.71414 * 255 / 224)
B = (Y - 16) * (255 / 219) + (U - 128) * (1.77200 * 255 / 224)


R = Y * (255 / 219) - 16 * (255 / 219) + V * (1.40200 * 255 / 224) - 128 * (1.40200 * 255 / 224)
G = Y * (255 / 219) - 16 * (255 / 219) - U * (0.34414 * 255 / 224) + 128 * (0.34414 * 255 / 224) - V * (0.71414 * 255 / 224) + 128 * (0.71414 * 255 / 224)
B = Y * (255 / 219) - 16 * (255 / 219) + U * (1.77200 * 255 / 224) - 128 * (1.77200 * 255 / 224)


R = Y * (255 / 219) + V * (1.40200 * 255 / 224) - 16 * (255 / 219) - 128 * (1.40200 * 255 / 224)
G = Y * (255 / 219) - U * (0.34414 * 255 / 224) - V * (0.71414 * 255 / 224) - 16 * (255 / 219) + 128 * (0.34414 * 255 / 224) + 128 * (0.71414 * 255 / 224)
B = Y * (255 / 219) + U * (1.77200 * 255 / 224) - 16 * (255 / 219) - 128 * (1.77200 * 255 / 224)


my   = (255.0 / 219.0)
mu1  = (1.77200 * 255.0 / 224.0)
mv1  = (1.40200 * 255.0 / 224.0)
mu2  = - (0.34414 * 255.0 / 224.0)
mv2  = - (0.71414 * 255.0 / 224.0)
addb = - 16.0 * (255.0 / 219.0) - 128.0 * (1.77200 * 255.0 / 224.0)
addr = - 16.0 * (255.0 / 219.0) - 128.0 * (1.40200 * 255.0 / 224.0)
addg = - 16.0 * (255.0 / 219.0) + 128.0 * (0.34414 * 255.0 / 224.0) + 128.0 * (0.71414 * 255.0 / 224.0)


B = Y * my + U * mu1 + addb
R = Y * my + V * mv1 + addr
G = Y * my + U * mu2 + V * mv2 + addg


Y = + R * (0.29900 * 219.0 / 255.0) + G * (0.58700 * 219.0 / 255.0) + B * (0.11400 * 219.0 / 255.0) + 16.0
U = - R * (0.16874 * 224.0 / 255.0) - G * (0.33126 * 224.0 / 255.0) + B * (0.50000 * 224.0 / 255.0) + 128.0
V = + R * (0.50000 * 224.0 / 255.0) - G * (0.41869 * 224.0 / 255.0) - B * (0.08131 * 224.0 / 255.0) + 128.0


Y -> 16 .. 235
U -> 16 .. 240
V -> 16 .. 240
*/


#include<stdio.h>
#include<stdlib.h>


#define y_levels 16
#define y_step   4


double my;
double mu1;
double mv1;
double mu2;
double mv2;
double addb;
double addr;
double addg;


int y_constant [256];
int u1_constant[256];
int v1_constant[256];
int u2_constant[256];
int v2_constant[256];


// this reduce the error on green calculations
#define g_offset (50.0)


int color_space_conversion_y_put[1024 * y_levels];
int color_space_conversion_u_add[1024];
int color_space_conversion_u_sub[1024];
int color_space_conversion_v_add[1024];
int color_space_conversion_v_sub[1024];


struct yuv_int_struct
	{
	int y;
	int u;
	int v;
	};


struct rgb_int_struct
	{
	int r;
	int g;
	int b;
	};


struct rgb_double_struct
	{
	double r;
	double g;
	double b;
	};


struct minimum_maximum_struct
	{
	int first;
	int minimum;
	int maximum;
	};


struct rgb_int_error_struct
	{
	struct minimum_maximum_struct r_error;
	struct minimum_maximum_struct g_error;
	struct minimum_maximum_struct b_error;
	long long number_of_tests;
	long long sum_of_absolute_r_error;
	long long sum_of_absolute_g_error;
	long long sum_of_absolute_b_error;
	};


void init_constants()
	{
	my   = (255.0 / 219.0);
	mu1  = (1.77200 * 255.0 / 224.0);
	mv1  = (1.40200 * 255.0 / 224.0);
	mu2  = - (0.34414 * 255.0 / 224.0);
	mv2  = - (0.71414 * 255.0 / 224.0);
	addb = - 16.0 * (255.0 / 219.0) - 128.0 * (1.77200 * 255.0 / 224.0);
	addr = - 16.0 * (255.0 / 219.0) - 128.0 * (1.40200 * 255.0 / 224.0);
	addg = - 16.0 * (255.0 / 219.0) + 128.0 * (0.34414 * 255.0 / 224.0) + 128.0 * (0.71414 * 255.0 / 224.0);
	}


void init_minimum_maximum(struct minimum_maximum_struct *minimum_maximum)
	{
	minimum_maximum->first = 1;
	}


void find_minimum_maximum(int value, struct minimum_maximum_struct *minimum_maximum)
	{
	if (minimum_maximum->first)
		{
		minimum_maximum->first = 0;

		minimum_maximum->minimum = value;
		minimum_maximum->maximum = value;
		}
	else
		{
		if (value < minimum_maximum->minimum) minimum_maximum->minimum = value;
		if (value > minimum_maximum->maximum) minimum_maximum->maximum = value;
		}
	}


void init_rgb_int_error(struct rgb_int_error_struct *e)
	{
	init_minimum_maximum(&e->r_error);
	init_minimum_maximum(&e->g_error);
	init_minimum_maximum(&e->b_error);

	e->number_of_tests         = 0;
	e->sum_of_absolute_r_error = 0;
	e->sum_of_absolute_g_error = 0;
	e->sum_of_absolute_b_error = 0;
	}


void find_rgb_int_error(struct rgb_int_error_struct *e, struct rgb_int_struct *rgb_int_1, struct rgb_int_struct *rgb_int_2)
	{
	find_minimum_maximum(rgb_int_1->r - rgb_int_2->r, &e->r_error);
	find_minimum_maximum(rgb_int_1->g - rgb_int_2->g, &e->g_error);
	find_minimum_maximum(rgb_int_1->b - rgb_int_2->b, &e->b_error);

	e->number_of_tests ++;

	long long r = abs(rgb_int_1->r - rgb_int_2->r);
	long long g = abs(rgb_int_1->g - rgb_int_2->g);
	long long b = abs(rgb_int_1->b - rgb_int_2->b);

	e->sum_of_absolute_r_error += r;
	e->sum_of_absolute_g_error += g;
	e->sum_of_absolute_b_error += b;
	}


void show_minimum_maximum(char *txt, struct minimum_maximum_struct *minimum_maximum)
	{
	printf("%s: minimum=%i maximum=%i\n", txt, minimum_maximum->minimum, minimum_maximum->maximum);
	}


void show_rgb_int_error(char *txt, struct rgb_int_error_struct *e)
	{
	printf("%s:\n", txt);

	show_minimum_maximum(" r_error", &e->r_error);
	show_minimum_maximum(" g_error", &e->g_error);
	show_minimum_maximum(" b_error", &e->b_error);

	long long average_r_error = 1000 * e->sum_of_absolute_r_error / e->number_of_tests;
	long long average_g_error = 1000 * e->sum_of_absolute_g_error / e->number_of_tests;
	long long average_b_error = 1000 * e->sum_of_absolute_b_error / e->number_of_tests;

	printf(" avg_r_error: %i.%03i\n", (int) (average_r_error / 1000), (int) (average_r_error % 1000));
	printf(" avg_g_error: %i.%03i\n", (int) (average_g_error / 1000), (int) (average_g_error % 1000));
	printf(" avg_b_error: %i.%03i\n", (int) (average_b_error / 1000), (int) (average_b_error % 1000));
	}


int round_double(double value)
	{
	if (value >= 0)
		{
		return(value + 0.5);
		}
	else
		{
		return(-(-value + 0.5));
		}
	}


void make_yuv_int(struct yuv_int_struct *yuv_int, struct rgb_double_struct *rgb_double)
	{
	double y = + rgb_double->r * (0.29900 * 219.0 / 255.0) + rgb_double->g * (0.58700 * 219.0 / 255.0) + rgb_double->b * (0.11400 * 219.0 / 255.0) + 16.0;
	double u = - rgb_double->r * (0.16874 * 224.0 / 255.0) - rgb_double->g * (0.33126 * 224.0 / 255.0) + rgb_double->b * (0.50000 * 224.0 / 255.0) + 128.0;
	double v = + rgb_double->r * (0.50000 * 224.0 / 255.0) - rgb_double->g * (0.41869 * 224.0 / 255.0) - rgb_double->b * (0.08131 * 224.0 / 255.0) + 128.0;

	yuv_int->y = round_double(y);
	yuv_int->u = round_double(u);
	yuv_int->v = round_double(v);
	}


void make_rgb_int(struct rgb_int_struct *rgb_int, struct yuv_int_struct *yuv_int)
	{
	double y = yuv_int->y;
	double u = yuv_int->u;
	double v = yuv_int->v;

	double r = y * my + v * mv1 + addr;
	double g = y * my + u * mu2 + v * mv2 + addg;
	double b = y * my + u * mu1 + addb;

	rgb_int->r = round_double(r);
	rgb_int->g = round_double(g);
	rgb_int->b = round_double(b);
	}


void clip_int(int *v)
	{
	if      (*v <   0) *v =   0;
	else if (*v > 255) *v = 255;
	}


void make_new_rgb_int(struct rgb_int_struct *rgb_int, struct yuv_int_struct *yuv_int)
	{
	rgb_int->b = y_constant[yuv_int->y] + u1_constant[yuv_int->u];
	clip_int(&rgb_int->b);


	rgb_int->r = y_constant[yuv_int->y] + v1_constant[yuv_int->v];
	clip_int(&rgb_int->r);


	rgb_int->g = y_constant[yuv_int->y];
	clip_int(&rgb_int->g);

	rgb_int->g += u2_constant[yuv_int->u];
	clip_int(&rgb_int->g);

	rgb_int->g += v2_constant[yuv_int->v];
	clip_int(&rgb_int->g);
	}


void clip_rgb_int(struct rgb_int_struct *rgb_int)
	{
	clip_int(&rgb_int->r);
	clip_int(&rgb_int->g);
	clip_int(&rgb_int->b);
	}


void show(char *txt, int *array)
	{
	printf("%s[256] = {", txt);


	int i = 0;
	for (; i < 256; i++)
		{
		if (i) printf(", ");
		printf("%i", array[i]);
		}


	printf("};\n");
	}


void calc_y_constant()
	{
	int i = 0;

	for (; i < 256; i++)
		{
		double y;
		if (i < 16)
			{
			y = 16;
			}
		else if (i > 235)
			{
			y = 235;
			}
		else
			{
			y = i;
			}


		double value = y * my - 16.0 * my;

		y_constant[i] = round_double(value);
		}
	}


void calc_u1_constant()
	{
	int i = 0;

	for (; i < 256; i++)
		{
		double u;
		if (i < 16)
			{
			u = 16;
			}
		else if (i > 240)
			{
			u = 240;
			}
		else
			{
			u = i;
			}


		double value = u * mu1 + addb + 16.0 * my;

		u1_constant[i] = round_double(value);
		}
	}


void calc_v1_constant()
	{
	int i = 0;

	for (; i < 256; i++)
		{
		double v;
		if (i < 16)
			{
			v = 16;
			}
		else if (i > 240)
			{
			v = 240;
			}
		else
			{
			v = i;
			}


		double value = v * mv1 + addr + 16.0 * my;

		v1_constant[i] = round_double(value);
		}
	}


void calc_u2_constant()
	{
	int i = 0;

	for (; i < 256; i++)
		{
		double u;
		if (i < 16)
			{
			u = 16;
			}
		else if (i > 240)
			{
			u = 240;
			}
		else
			{
			u = i;
			}


		double value = u * mu2 + g_offset;

		u2_constant[i] = round_double(value);
		}
	}


void calc_v2_constant()
	{
	int i = 0;

	for (; i < 256; i++)
		{
		double v;
		if (i < 16)
			{
			v = 16;
			}
		else if (i > 240)
			{
			v = 240;
			}
		else
			{
			v = i;
			}


		double value = v * mv2 + addg + 16.0 * my - g_offset;

		v2_constant[i] = round_double(value);
		}
	}


void show_unsigned_char(char *txt, int *array, int tot)
	{
	printf("%s[%i] = {", txt, tot);


	int i = 0;
	for (; i < tot; i++)
		{
		if (i) printf(", ");
		printf("%i", array[i]);
		}


	printf("};\n");
	}


void calc_color_space_conversion_y_put()
	{
	int j = 0;

	for (; j < y_levels; j++)
		{
		int i = 0;

		for (; i < 256; i++)
			{
			int v = y_constant[i] + j * y_step;

			clip_int(&v);

			color_space_conversion_y_put[1024 * j + 4 * i + 0] = v;
			color_space_conversion_y_put[1024 * j + 4 * i + 1] = v;
			color_space_conversion_y_put[1024 * j + 4 * i + 2] = v;
			color_space_conversion_y_put[1024 * j + 4 * i + 3] = 0;
			}
		}
	}


void calc_color_space_conversion_u_add()
	{
	// R = Y * my + V * mv1 + addr
	// G = Y * my + U * mu2 + V * mv2 + addg
	// B = Y * my + U * mu1 + addb

	int i = 0;

	for (; i < 256; i++)
		{
		color_space_conversion_u_add[4 * i + 0] = 0;
		color_space_conversion_u_add[4 * i + 1] = u2_constant[i] >= 0 ? u2_constant[i] : 0;
		color_space_conversion_u_add[4 * i + 2] = u1_constant[i] >= 0 ? u1_constant[i] : 0;
		color_space_conversion_u_add[4 * i + 3] = 0;
		}
	}


void calc_color_space_conversion_u_sub()
	{
	// R = Y * my + V * mv1 + addr
	// G = Y * my + U * mu2 + V * mv2 + addg
	// B = Y * my + U * mu1 + addb

	int i = 0;

	for (; i < 256; i++)
		{
		color_space_conversion_u_sub[4 * i + 0] = 0;
		color_space_conversion_u_sub[4 * i + 1] = u2_constant[i] >= 0 ? 0 : -u2_constant[i];
		color_space_conversion_u_sub[4 * i + 2] = u1_constant[i] >= 0 ? 0 : -u1_constant[i];
		color_space_conversion_u_sub[4 * i + 3] = 0;
		}
	}


void calc_color_space_conversion_v_add()
	{
	// R = Y * my + V * mv1 + addr
	// G = Y * my + U * mu2 + V * mv2 + addg
	// B = Y * my + U * mu1 + addb

	int i = 0;

	for (; i < 256; i++)
		{
		color_space_conversion_v_add[4 * i + 0] = v1_constant[i] >= 0 ? v1_constant[i] : 0;
		color_space_conversion_v_add[4 * i + 1] = v2_constant[i] >= 0 ? v2_constant[i] : 0;
		color_space_conversion_v_add[4 * i + 2] = 0;
		color_space_conversion_v_add[4 * i + 3] = 0;
		}
	}


void calc_color_space_conversion_v_sub()
	{
	// R = Y * my + V * mv1 + addr
	// G = Y * my + U * mu2 + V * mv2 + addg
	// B = Y * my + U * mu1 + addb

	int i = 0;

	for (; i < 256; i++)
		{
		color_space_conversion_v_sub[4 * i + 0] = v1_constant[i] >= 0 ? 0 : -v1_constant[i];
		color_space_conversion_v_sub[4 * i + 1] = v2_constant[i] >= 0 ? 0 : -v2_constant[i];
		color_space_conversion_v_sub[4 * i + 2] = 0;
		color_space_conversion_v_sub[4 * i + 3] = 0;
		}
	}


int main()
	{
	init_constants();


	struct rgb_double_struct rgb_double;
	struct yuv_int_struct    yuv_int;
	struct rgb_int_struct    base_rgb_int;
	struct rgb_int_struct    rgb_int;
	struct rgb_int_struct    new_rgb_int;


	struct minimum_maximum_struct minimum_maximum_y;
	struct minimum_maximum_struct minimum_maximum_u;
	struct minimum_maximum_struct minimum_maximum_v;
	init_minimum_maximum(&minimum_maximum_y);
	init_minimum_maximum(&minimum_maximum_u);
	init_minimum_maximum(&minimum_maximum_v);


	struct minimum_maximum_struct minimum_maximum_r;
	struct minimum_maximum_struct minimum_maximum_g;
	struct minimum_maximum_struct minimum_maximum_b;
	init_minimum_maximum(&minimum_maximum_r);
	init_minimum_maximum(&minimum_maximum_g);
	init_minimum_maximum(&minimum_maximum_b);


	struct rgb_int_error_struct error_1;
	struct rgb_int_error_struct error_2;
	init_rgb_int_error(&error_1);
	init_rgb_int_error(&error_2);


	calc_y_constant();
	calc_u1_constant();
	calc_v1_constant();
	calc_u2_constant();
	calc_v2_constant();


	calc_color_space_conversion_y_put();
	calc_color_space_conversion_u_add();
	calc_color_space_conversion_u_sub();
	calc_color_space_conversion_v_add();
	calc_color_space_conversion_v_sub();


	for (base_rgb_int.r = 0; base_rgb_int.r < 256; base_rgb_int.r++)
		{
		for (base_rgb_int.g = 0; base_rgb_int.g < 256; base_rgb_int.g++)
			{
			for (base_rgb_int.b = 0; base_rgb_int.b < 256; base_rgb_int.b++)
				{
				rgb_double.r = base_rgb_int.r;
				rgb_double.g = base_rgb_int.g;
				rgb_double.b = base_rgb_int.b;


				/*
				Integer YUV

				Y -> 16 .. 235
				U -> 16 .. 240
				V -> 16 .. 240
				*/
				make_yuv_int(&yuv_int, &rgb_double);
				find_minimum_maximum(yuv_int.y, &minimum_maximum_y);
				find_minimum_maximum(yuv_int.u, &minimum_maximum_u);
				find_minimum_maximum(yuv_int.v, &minimum_maximum_v);


				make_rgb_int(&rgb_int, &yuv_int);
				find_minimum_maximum(rgb_int.r, &minimum_maximum_r);
				find_minimum_maximum(rgb_int.g, &minimum_maximum_g);
				find_minimum_maximum(rgb_int.b, &minimum_maximum_b);
				clip_rgb_int(&rgb_int);
				find_rgb_int_error(&error_1, &base_rgb_int, &rgb_int);


				make_new_rgb_int(&new_rgb_int, &yuv_int);
				find_rgb_int_error(&error_2, &rgb_int, &new_rgb_int);
				}
			}
		}


	//show_minimum_maximum("yuv_int.y", &minimum_maximum_y);
	//show_minimum_maximum("yuv_int.u", &minimum_maximum_u);
	//show_minimum_maximum("yuv_int.v", &minimum_maximum_v);


	//show_minimum_maximum("rgb_int.r", &minimum_maximum_r);
	//show_minimum_maximum("rgb_int.g", &minimum_maximum_g);
	//show_minimum_maximum("rgb_int.b", &minimum_maximum_b);


	//show_rgb_int_error("base_rgb_int - rgb_int", &error_1);
	//show_rgb_int_error("rgb_int - new_rgb_int",  &error_2);


	//show("y_constant", y_constant);
	//show("u1_constant", u1_constant);
	//show("v1_constant", v1_constant);
	//show("u2_constant", u2_constant);
	//show("v2_constant", v2_constant);


	show_unsigned_char("unsigned char __attribute__((aligned(16))) color_space_conversion_y_put", color_space_conversion_y_put, 1024 * y_levels);
	show_unsigned_char("unsigned char __attribute__((aligned(16))) color_space_conversion_u_add", color_space_conversion_u_add, 1024);
	show_unsigned_char("unsigned char __attribute__((aligned(16))) color_space_conversion_u_sub", color_space_conversion_u_sub, 1024);
	show_unsigned_char("unsigned char __attribute__((aligned(16))) color_space_conversion_v_add", color_space_conversion_v_add, 1024);
	show_unsigned_char("unsigned char __attribute__((aligned(16))) color_space_conversion_v_sub", color_space_conversion_v_sub, 1024);


	return(0);
	}
