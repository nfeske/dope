/*
 * \brief   DOpE entry function
 * \date    2002-11-13
 * \author  Norman Feske
 *
 * This file describes the startup of DOpE. It
 * initialises all needed modules and calls the
 * eventloop (see eventloop.c) of DOpE.
 */

/*
 * Copyright (C) 2002-2007 Norman Feske
 * Copyright (C) 2008-2014 Genode Labs GmbH
 *
 * This file is part of the DOpE package, which is distributed under
 * the terms of the GNU General Public Licence 2.
 */

#include "dopestd.h"
#include "scheduler.h"
#include "gfx.h"
#include "userstate.h"
#include "screen.h"

static struct gfx_services       *gfx;
static struct screen_services    *screen;
static struct userstate_services *userstate;

extern SCREEN *curr_scr;

/**
 * Prototypes from startup.c (in system dependent directory)
 */
extern void native_startup(int argc, char **argv);

/**
 * Prototypes from pool.c
 */
extern long  pool_add(char const *name,void *structure);
extern void *pool_get(char const *name);

/**
 * Prototypes from 'modules'
 */
extern int init_keymap           (struct dope_services *);
extern int init_clipping         (struct dope_services *);
extern int init_scrdrv           (struct dope_services *);
extern int init_input            (struct dope_services *);
extern int init_viewman          (struct dope_services *);
extern int init_widman           (struct dope_services *);
extern int init_screen           (struct dope_services *);
extern int init_timer            (struct dope_services *);
extern int init_tick             (struct dope_services *);
extern int init_relax            (struct dope_services *);
extern int init_button           (struct dope_services *);
extern int init_entry            (struct dope_services *);
extern int init_loaddisplay      (struct dope_services *);
extern int init_variable         (struct dope_services *);
extern int init_label            (struct dope_services *);
extern int init_background       (struct dope_services *);
extern int init_container        (struct dope_services *);
extern int init_window           (struct dope_services *);
extern int init_userstate        (struct dope_services *);
extern int init_conv_fnt         (struct dope_services *);
extern int init_conv_tff         (struct dope_services *);
extern int init_fontman          (struct dope_services *);
extern int init_gfx              (struct dope_services *);
extern int init_gfxscr16         (struct dope_services *);
extern int init_gfximg16         (struct dope_services *);
extern int init_gfximg32         (struct dope_services *);
extern int init_cache            (struct dope_services *);
extern int init_scale            (struct dope_services *);
extern int init_scrollbar        (struct dope_services *);
extern int init_frame            (struct dope_services *);
extern int init_grid             (struct dope_services *);
extern int init_redraw           (struct dope_services *);
extern int init_simple_scheduler (struct dope_services *);
extern int init_hashtable        (struct dope_services *);
extern int init_tokenizer        (struct dope_services *);
extern int init_scope            (struct dope_services *);
extern int init_script           (struct dope_services *);
extern int init_appman           (struct dope_services *);
extern int init_winlayout        (struct dope_services *);
extern int init_messenger        (struct dope_services *);
extern int init_vscreen          (struct dope_services *);
extern int init_vtextscreen      (struct dope_services *);
extern int init_sharedmem        (struct dope_services *);

/**
 * Prototypes from eventloop.c
 */
extern void eventloop(struct dope_services *);

struct dope_services dope = {
	pool_get,
	pool_add,
};


int config_transparency  = 0;   /* use translucent effects                */
int config_don_scheduler = 0;   /* use donation scheduler                 */
int config_clackcommit   = 0;   /* deliver commit events on mouse release */
int config_winborder     = 5;   /* size of window resize border           */
int config_menubar       = 0;   /* menubar visibility                     */
int config_dropshadows   = 1;   /* draw dropshadows behind windows        */


extern "C" void wait_for_continue();


long dope_init()
{
	init_sharedmem(&dope);
	init_timer(&dope);
	init_tick(&dope);
	init_relax(&dope);
	init_keymap(&dope);
	init_cache(&dope);
	init_hashtable(&dope);
	init_appman(&dope);
	init_tokenizer(&dope);
	init_messenger(&dope);
	init_script(&dope);
	init_clipping(&dope);
	init_scrdrv(&dope);
	init_input(&dope);
	init_viewman(&dope);
	init_conv_fnt(&dope);
	init_conv_tff(&dope);
	init_fontman(&dope);
	init_gfxscr16(&dope);
	init_gfximg16(&dope);
	init_gfximg32(&dope);
	init_gfx(&dope);
	init_redraw(&dope);
	init_userstate(&dope);
	init_widman(&dope);
	init_scope(&dope);
	init_button(&dope);
	init_entry(&dope);
	init_variable(&dope);
	init_label(&dope);
	init_loaddisplay(&dope);
	init_background(&dope);
	init_scrollbar(&dope);
	init_scale(&dope);
	init_frame(&dope);
	init_container(&dope);
	init_grid(&dope);
	init_winlayout(&dope);
	init_window(&dope);
	init_screen(&dope);
	init_simple_scheduler(&dope);
	init_vscreen(&dope);
	init_vtextscreen(&dope);

	{
		static GFX_CONTAINER *scr_ds;
		gfx       = (gfx_services       *)(pool_get("Gfx 1.0"));
		screen    = (screen_services    *)(pool_get("Screen 1.0"));
		userstate = (userstate_services *)(pool_get("UserState 1.0"));

		scr_ds = gfx->alloc_scr("default");
		curr_scr = screen->create();
		curr_scr->scr->set_gfx(curr_scr, scr_ds);
		userstate->set_max_mx(gfx->get_width(scr_ds));
		userstate->set_max_my(gfx->get_height(scr_ds));
	}

	return 0;
}
