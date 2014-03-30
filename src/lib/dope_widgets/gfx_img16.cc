/*
 * \brief   DOpE gfx 16bit image handler module
 * \date    2003-04-05
 * \author  Norman Feske
 */

/*
 * Copyright (C) 2002-2007 Norman Feske
 * Copyright (C) 2008-2014 Genode Labs GmbH
 *
 * This file is part of the DOpE package, which is distributed under
 * the terms of the GNU General Public Licence 2.
 */

#include "dopestd.h"
#include "sharedmem.h"
#include "gfx_handler.h"
#include "gfx.h"

typedef u16 pixel_t;

static struct sharedmem_services *shmem;

struct gfx_ds_data {
	int        w,h;     /* width and height of the image */
	SHAREDMEM *smb;     /* shared memory block */
	pixel_t   *pixels;  /* 16bit color values of the pixels */
};

int init_gfximg16(struct dope_services *d);


/***************************
 ** Gfx handler functions **
 ***************************/

static int img_get_width(struct gfx_ds_data *img)
{
	return img->w;
}


static int img_get_height(struct gfx_ds_data *img)
{
	return img->h;
}


static enum img_type img_get_type(struct gfx_ds_data *img)
{
	return GFX_IMG_TYPE_RGB16;
}


static void img_destroy(struct gfx_ds_data *img)
{
	shmem->destroy(img->smb);
	free(img);
}


static void *img_map(struct gfx_ds_data *img)
{
	return img->pixels;
}


static int img_share(struct gfx_ds_data *img, void *dst_thread)
{
	return shmem->share(img->smb, dst_thread);
}


static int img_get_ident(struct gfx_ds_data *img, char *dst_ident)
{
	shmem->get_ident(img->smb, dst_ident);
	return 0;
}


/***********************
 ** Service functions **
 ***********************/

static struct gfx_ds_data *create(int width, int height, struct gfx_ds_handler **handler)
{
	struct gfx_ds_data *data = (struct gfx_ds_data *)zalloc(sizeof(struct gfx_ds_data));
	if (!data) return NULL;

	data->w      = width;
	data->h      = height;
	data->smb    = shmem->alloc(width*height*sizeof(pixel_t));
	data->pixels = (pixel_t *)(shmem->get_address(data->smb));

	if (data->pixels)
		memset(data->pixels, 0, width*height*sizeof(pixel_t));

	return data;
}


static int register_gfx_handler(struct gfx_ds_handler *handler)
{
	handler->get_width  = img_get_width;
	handler->get_height = img_get_height;
	handler->get_type   = img_get_type;
	handler->destroy    = img_destroy;
	handler->map        = img_map;
	handler->share      = img_share;
	handler->get_ident  = img_get_ident;
	return 0;
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct gfx_handler_services services = {
	create,
	register_gfx_handler,
};


/************************
 ** Module entry point **
 ************************/

int init_gfximg16(struct dope_services *d)
{
	shmem = (sharedmem_services *)(d->get_module("SharedMemory 1.0"));
	d->register_module("GfxImage16 1.0",&services);
	return 1;
}
