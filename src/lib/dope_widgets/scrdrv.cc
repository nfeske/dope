/*
 * \brief   Genode version of DOpE screen driver module
 * \date    2007-07-26
 * \author  Norman Feske
 */

/*
 * Copyright (C) 2002-2007 Norman Feske
 * Copyright (C) 2006-2013 Genode Labs GmbH
 *
 * This file is part of the DOpE package, which is distributed under
 * the  terms  of the  GNU General Public Licence 2.
 */

/* local includes */
#include "dopestd.h"
#include "scrdrv.h"

/* Genode includes */
#include <base/printf.h>
#include <base/env.h>
#include <base/sleep.h>
#include <nitpicker_session/connection.h>
#include <nitpicker_view/client.h>
#include <framebuffer_session/client.h>
#include <input_session/client.h>


Nitpicker::Session   *nitpicker_session;
Input::Session       *input_session;
Framebuffer::Session *framebuffer_session;

static int    scr_width, scr_height;    /* screen dimensions                         */
static int    scr_depth;                /* color depth                               */
static int    scr_linelength;           /* bytes per scanline                        */
static void  *buf_adr;                  /* adress of screen buffer (doublebuffering) */

extern int config_adapt_redraw;

int init_scrdrv(struct dope_services *d);


/***********************
 ** Service functions **
 ***********************/

/**
 * Set up screen
 */
static long set_screen(long width, long height, long depth)
{
	Framebuffer::Mode scr_mode = framebuffer_session->mode();

	scr_width      = scr_mode.width();
	scr_height     = scr_mode.height();
	scr_linelength = scr_width;

	switch (scr_mode.format()) {

	case Framebuffer::Mode::RGB565:
		scr_depth = 16;
		break;

	default:
		PERR("color mode is not supported");
	}

	printf("screen is %dx%d\n", scr_width, scr_height);
	if (!scr_width || !scr_height) {
		PERR("got invalid screen - spinning");
		Genode::sleep_forever();
	}

	buf_adr = Genode::env()->rm_session()->attach(framebuffer_session->dataspace());
	return 1;
}


/**
 * Deinitialisation
 */
static void restore_screen(void) { }


/**
 * Provide information about the screen
 */
static long  get_scr_width  (void) {return scr_width;}
static long  get_scr_height (void) {return scr_height;}
static long  get_scr_depth  (void) {return scr_depth;}
static void *get_scr_adr    (void) {return buf_adr;}
static void *get_buf_adr    (void) {return buf_adr;}


/**
 * Propagate buffer update to nitpicker
 */
static void update_area(long x1,long y1,long x2,long y2) {

	if ((x1 > x2) || (y1 > y2) || (x1 > scr_width)  || (x2 < 0)
	                           || (y1 > scr_height) || (y2 < 0)) return;

	framebuffer_session->refresh(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
}


/**
 * Set new mouse shape
 */
static void set_mouse_shape(void *new_shape) { }


/**
 * Set mouse position
 */
static void set_mouse_pos(long mx, long my) { }


/**************************************
 ** Service structure of this module **
 **************************************/

static struct scrdrv_services services = {
	set_screen:         set_screen,
	restore_screen:     restore_screen,
	get_scr_width:      get_scr_width,
	get_scr_height:     get_scr_height,
	get_scr_depth:      get_scr_depth,
	get_scr_adr:        get_scr_adr,
	get_buf_adr:        get_buf_adr,
	update_area:        update_area,
	set_mouse_pos:      set_mouse_pos,
	set_mouse_shape:    set_mouse_shape,
};


/************************
 ** Module entry point **
 ************************/

int init_scrdrv(struct dope_services *d)
{
	config_adapt_redraw = 0;

	using namespace Genode;

	/*
	 * Init sessions to the required external services
	 */
	static Nitpicker::Connection nitpicker;
	nitpicker.buffer(nitpicker.mode(), false);
	nitpicker_session = &nitpicker;

	static Framebuffer::Session_client
		framebuffer(nitpicker.framebuffer_session());
	framebuffer_session = &framebuffer;

	static Input::Session_client
		input(nitpicker.input_session());
	input_session = &input;

	d->register_module("ScreenDriver 1.0",&services);
	return 1;
}
