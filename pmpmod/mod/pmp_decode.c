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
av decoding in a ring buffer
*/


#include "pmp_decode.h"


void pmp_decode_safe_constructor(struct pmp_decode_struct *p)
	{
	pmp_read_safe_constructor(&p->reader);

	p->video_context = 0;
	p->audio_context = 0;

	p->saved_video_frame = 0;

	me_idct_safe_constructor();

	int i = 0;
	for (; i < maximum_frame_buffers; i++)
		{
		p->video_frame_buffers[i] = 0;
		p->audio_frame_buffers[i] = 0;
		}
	}


void pmp_decode_close(struct pmp_decode_struct *p)
	{
	pmp_read_close(&p->reader);

	if (p->video_context != 0)
		{
		avcodec_close(p->video_context);
		av_free(p->video_context);
		}

	if (p->audio_context != 0)
		{
		avcodec_close(p->audio_context);
		av_free(p->audio_context);
		}

	if (p->saved_video_frame != 0) av_free(p->saved_video_frame);

	me_idct_p_free();

	int i = 0;
	for (; i < maximum_frame_buffers; i++)
		{
		if (p->video_frame_buffers[i] != 0) free_64(p->video_frame_buffers[i]);
		if (p->audio_frame_buffers[i] != 0) free_64(p->audio_frame_buffers[i]);
		}


	pmp_decode_safe_constructor(p);
	}


char *pmp_decode_open(struct pmp_decode_struct *p, char *s)
	{
	pmp_decode_safe_constructor(p);




	char *result = pmp_read_open(&p->reader, FF_INPUT_BUFFER_PADDING_SIZE, s);
	if (result != 0)
		{
		pmp_decode_close(p);
		return(result);
		}

	p->audio_frame_size = (p->reader.file.header.audio.scale << 1) << p->reader.file.header.audio.stereo;




	aspect_ratio_struct_init(p->reader.file.header.video.width, p->reader.file.header.video.height);




	p->video_decoder = avcodec_find_decoder(CODEC_ID_MPEG4);
	if (p->video_decoder == 0)
		{
		pmp_decode_close(p);
		return("pmp_decode_open: avcodec_find_decoder failed on CODEC_ID_MPEG4");
		}


	p->audio_decoder = avcodec_find_decoder(CODEC_ID_MP3);
	if (p->audio_decoder == 0)
		{
		pmp_decode_close(p);
		return("pmp_decode_open: avcodec_find_decoder failed on CODEC_ID_MP3");
		}




	p->video_context = avcodec_alloc_context();
	if (p->video_context == 0)
		{
		pmp_decode_close(p);
		return("pmp_decode_open: avcodec_alloc_context failed");
		}


    p->video_context->width  = p->reader.file.header.video.width;
    p->video_context->height = p->reader.file.header.video.height;

    if (avcodec_open(p->video_context, p->video_decoder) < 0)
    	{
    	av_free(p->video_context);
    	p->video_context = 0;

		pmp_decode_close(p);
		return("pmp_decode_open: avcodec_open failed on video_context");
    	}




	p->audio_context = avcodec_alloc_context();
	if (p->audio_context == 0)
		{
		pmp_decode_close(p);
		return("pmp_decode_open: avcodec_alloc_context failed");
		}


	if (avcodec_open(p->audio_context, p->audio_decoder) < 0)
		{
    	av_free(p->audio_context);
    	p->audio_context = 0;

		pmp_decode_close(p);
		return("pmp_decode_open: avcodec_open failed on audio_context");
		}




	p->saved_video_frame = avcodec_alloc_frame();
	if (p->saved_video_frame == 0)
		{
		pmp_decode_close(p);
		return("pmp_decode_open: avcodec_alloc_frame failed");
		}




	result = me_idct_p_malloc(p->reader.file.header.video.width, p->reader.file.header.video.height);
	if (result != 0)
		{
		pmp_decode_close(p);
		return(result);
		}




	p->number_of_frame_buffers = 0;

	int i = 0;
	for (; i < maximum_frame_buffers; i++)
		{
		p->video_frame_buffers[i] = malloc_64(557056 + 2048 * interface_height);

		if (p->video_frame_buffers[i] == 0)
			{
			break;
			}

		memset(p->video_frame_buffers[i], 0, 557056 + 2048 * interface_height);

		p->number_of_frame_buffers ++;
		}


	if (p->number_of_frame_buffers < (number_of_free_video_frame_buffers + 4))
		{
		pmp_decode_close(p);
		return("pmp_decode_open: number_of_frame_buffers < 4");
		}


	p->number_of_frame_buffers -= number_of_free_video_frame_buffers;

	i = 0;
	for (; i < number_of_free_video_frame_buffers; i++)
		{
		free_64(p->video_frame_buffers[p->number_of_frame_buffers + i]);

		p->video_frame_buffers[p->number_of_frame_buffers + i] = 0;
		}




	i = 0;
	for (; i <= p->number_of_frame_buffers; i++)
		{
		// p->audio_frame_buffers[p->number_of_frame_buffers] is a null buffer

		p->audio_frame_buffers[i] = malloc_64(p->audio_frame_size * p->reader.file.header.audio.maximum_number_of_frames);

		if (p->audio_frame_buffers[i] == 0)
			{
			pmp_decode_close(p);
			return("pmp_decode_open: malloc_64 failed on audio_frame_buffers");
			}

		memset(p->audio_frame_buffers[i], 0, p->audio_frame_size * p->reader.file.header.audio.maximum_number_of_frames);
		}




	sceDisplayWaitVblankStart();
	sceDisplaySetFrameBuf(p->video_frame_buffers[0], 512, PSP_DISPLAY_PIXEL_FORMAT_8888, PSP_DISPLAY_SETBUF_IMMEDIATE);




	p->current_buffer_number = 0;


	return(0);
	}


static void boost_volume(short *audio_buffer, unsigned int number_of_samples, unsigned int volume_boost)
	{
	if (volume_boost != 0)
		{
		while (number_of_samples--)
			{
			int sample = *audio_buffer;
			sample <<= volume_boost;


			if (sample > 32767)
				{
				*audio_buffer++ = 32767;
				}
			else if (sample < -32768)
				{
				*audio_buffer++ = -32768;
				}
			else
				{
				*audio_buffer++ = sample;
				}
			}
		}
	}


char *pmp_decode_get(struct pmp_decode_struct *p, unsigned int frame_number, unsigned int audio_stream, int decode_audio, unsigned int volume_boost, unsigned int aspect_ratio, unsigned int zoom, unsigned int luminosity_boost, unsigned int show_interface)
	{
	char *video_result = 0;


	video_result = pmp_read_get(&p->reader, frame_number, audio_stream, &p->saved_packet);
	if (video_result == 0)
		{
		if (avcodec_decode_video(p->video_context, p->saved_video_frame, &p->saved_got_frame, p->saved_packet.video_buffer, p->saved_packet.video_length) != p->saved_packet.video_length)
			{
			video_result = "pmp_decode_get: avcodec_decode_video failed";
			}
		}

	if (video_result != 0)
		{
		return(video_result);
		}




	if (show_interface == 1)
		{
		draw_interface
			(
			p->reader.file.header.video.scale,
			p->reader.file.header.video.rate,
			p->reader.file.header.video.number_of_frames,
			frame_number,
			aspect_ratio,
			zoom,
			luminosity_boost,
			audio_stream,
			volume_boost
			);
		}




	p->output_frame_buffers[p->current_buffer_number].number_of_audio_frames = p->saved_packet.number_of_audio_frames;
	p->output_frame_buffers[p->current_buffer_number].first_delay            = p->saved_packet.first_delay;
	p->output_frame_buffers[p->current_buffer_number].last_delay             = p->saved_packet.last_delay;
	p->output_frame_buffers[p->current_buffer_number].video_frame            = p->video_frame_buffers[p->current_buffer_number];




	if (p->saved_got_frame == 0)
		{
		if (frame_number == 0)
			{
			memset(p->video_frame_buffers[p->current_buffer_number], 0, 557056 + 2048 * interface_height);
			}
		else
			{
			unsigned int previous_buffer_number;

			if (p->current_buffer_number == 0)
				{
				previous_buffer_number = p->number_of_frame_buffers - 1;
				}
			else
				{
				previous_buffer_number = p->current_buffer_number - 1;
				}


	        void *current_buffer  = p->video_frame_buffers[p->current_buffer_number];
	        void *previous_buffer = p->video_frame_buffers[previous_buffer_number];


			memcpy(current_buffer, previous_buffer, 557056 - 2048 * interface_height);

			if (show_interface == 0)
				{
				memcpy(current_buffer + 557056 - 2048 * interface_height, previous_buffer + 557056, 2048 * interface_height);
				}
			else
				{
				memcpy(current_buffer + 557056 - 2048 * interface_height, background_8888, 2048 * interface_height);
				}

			memcpy(current_buffer + 557056, previous_buffer + 557056, 2048 * interface_height);
			}
		}
	else
		{
		sceKernelDcacheWritebackInvalidateAll();

        pmp_gu_load_yuv
			(
			p->reader.file.header.video.width,
			p->reader.file.header.video.height,
			p->saved_video_frame->data[0],
			p->saved_video_frame->data[1],
			p->saved_video_frame->data[2],
			p->saved_video_frame->linesize[0],
			p->saved_video_frame->linesize[1],
			p->saved_video_frame->linesize[2]
			);

		pmp_gu_draw_yuv(aspect_ratio, zoom, luminosity_boost, show_interface, p->video_frame_buffers[p->current_buffer_number]);
		}




	char *audio_result = 0;


	if (p->saved_packet.audio_buffer == 0 || decode_audio == 0)
		{
		p->output_frame_buffers[p->current_buffer_number].audio_frame = p->audio_frame_buffers[p->number_of_frame_buffers];
		}
	else
		{
		p->output_frame_buffers[p->current_buffer_number].audio_frame = p->audio_frame_buffers[p->current_buffer_number];


		void *audio_buffer = p->saved_packet.audio_buffer;


		int i = 0;
		for (; i < p->saved_packet.number_of_audio_frames; i++)
			{
			int audio_length = p->saved_packet.audio_length[i];
			int audio_output_length;

			if (avcodec_decode_audio(p->audio_context, p->audio_frame_buffers[p->current_buffer_number] + p->audio_frame_size * i, &audio_output_length, audio_buffer, audio_length) != audio_length)
				{
				audio_result = "pmp_decode_get: avcodec_decode_audio failed";
				break;
				}

			audio_buffer += audio_length;


			if (audio_output_length != p->audio_frame_size)
				{
				if (audio_output_length == 0)
					{
					}
				else if (audio_output_length < p->audio_frame_size)
					{
					audio_result = "pmp_decode_get: audio_output_length < audio_frame_size";
					break;
					}
				else if (audio_output_length > p->audio_frame_size)
					{
					audio_result = "pmp_decode_get: audio_output_length > audio_frame_size (severe error)";
					break;
					}
				}
			else
				{
				boost_volume(p->audio_frame_buffers[p->current_buffer_number] + p->audio_frame_size * i, p->reader.file.header.audio.scale << p->reader.file.header.audio.stereo, volume_boost);
				}
			}
		}


	if (p->saved_got_frame != 0)
		{
		pmp_gu_wait();
		}




	p->current_buffer_number = (p->current_buffer_number + 1) % p->number_of_frame_buffers;
	sceKernelDcacheWritebackInvalidateAll();


	return(audio_result);
	}
