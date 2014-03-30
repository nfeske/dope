/*
 * \brief   DOpE VScreen widget module
 * \date    2002-11-13
 * \author  Norman Feske
 */

/*
 * Copyright (C) 2002-2007 Norman Feske
 * Copyright (C) 2008-2014 Genode Labs GmbH
 *
 * This file is part of the DOpE package, which is distributed under
 * the terms of the GNU General Public Licence 2.
 */

struct vscreen;
#define WIDGET struct vscreen

#include <input/keycodes.h>

#include "dopestd.h"
#include "appman.h"
#include "scheduler.h"
#include "widget_data.h"
#include "widget_help.h"
#include "gfx.h"
#include "redraw.h"
#include "vscreen.h"
#include "fontman.h"
#include "script.h"
#include "widman.h"
#include "userstate.h"
#include "messenger.h"
#include "window.h"

enum {
	MAX_IDENTS = 40,
	
	VSCR_MOUSEMODE_FREE    = 0,
	VSCR_MOUSEMODE_GRAB    = 1,
	VSCR_MOUSEMODE_GRABBED = 2,
	
	VSCR_UPDATE_MOUSE_X = 0x1,
	VSCR_UPDATE_MOUSE_Y = 0x2,
};

static struct scheduler_services   *sched;
static struct widman_services      *widman;
static struct script_services      *script;
static struct redraw_services      *redraw;
static struct appman_services      *appman;
static struct gfx_services         *gfx;
static struct messenger_services   *msg;
static struct userstate_services   *userstate;
static struct fontman_services     *font;

struct vscreen_data {
	long    update_flags;
	s8      bpp;                 /* bits per pixel                               */
	s32     xres, yres;          /* virtual screen dimensions                    */
	s16     fps;                 /* frames per second                            */
	char    smb_ident[64];       /* shared memory block identifier               */
	void   *pixels;              /* pointer to page aligned pixel buffer         */
	GFX_CONTAINER *image;        /* image representation (for drawing)           */
	s16     grabmouse;           /* mouse grab mode flag 0=free 1=grab 2=grabbed */
	VSCREEN *share_next;         /* next vscreen widget with shared buffer       */
	s32     vw, vh;              /* view size                                    */
	s32     curr_vx, curr_vy;    /* current view position                        */
	s32     next_vx, next_vy;    /* next view position                           */
};

static int msg_cnt, msg_fid;    /* fade cnt and font of on-screen msg */
static int msg_x, msg_y;        /* position of onscreen message       */
static int msg_w, msg_h;        /* size of onscreen message area      */
static char const *msg_txt;     /* text of the onscreen message       */

static int vs_mx, vs_my;        /* mouse position of grabbed mouse */

int init_vscreen(struct dope_services *d);


/***********************
 ** Utility functions **
 ***********************/

/**
 * Exclude vscreen widget from ring list of vscreens with a shared buffer
 */
static void vscr_share_exclude(VSCREEN *vs)
{
	VSCREEN *prev = vs->vd->share_next;
	while ((prev) && (prev->vd->share_next!=vs)) prev = prev->vd->share_next;
	if (prev) prev->vd->share_next = vs->vd->share_next;
	vs->vd->share_next = NULL;

	if (vs->vd->image) {
		gfx->dec_ref(vs->vd->image);
		vs->vd->image = NULL;
	}
}


/**
 * Include new vscreen in ring list of vscreens with a shared buffer
 *
 * \param vs   member of the new desired ring list
 * \param new  new vscreen widget to join the list
 */
static void vscr_share_join(VSCREEN *vs, VSCREEN *new_vs)
{
	if (new_vs->vd->share_next) vscr_share_exclude(new_vs);
	new_vs->vd->share_next = vs->vd->share_next;
	vs->vd->share_next = new_vs;
	if (!new_vs->vd->share_next) new_vs->vd->share_next = vs;
}


/****************************
 ** General widget methods **
 ****************************/

/**
 * Draw vscreen widget
 */
static int vscr_draw(VSCREEN *vs, struct gfx_ds *ds, long x, long y, WIDGET *origin)
{
	x += vs->wd->x;
	y += vs->wd->y;

	if (origin == vs) return 1;
	if (origin) return 0;

	gfx->push_clipping(ds, x, y, vs->wd->w, vs->wd->h);
	if (vs->vd->image) {
		float xratio = (float)vs->wd->w / (float)vs->vd->vw;
		float yratio = (float)vs->wd->h / (float)vs->vd->vh;
		int sx = xratio * vs->vd->curr_vx;
		int sy = yratio * vs->vd->curr_vy;
		int sw = xratio * vs->vd->xres;
		int sh = yratio * vs->vd->yres;

		gfx->draw_img(ds, x - sx, y - sy, sw, sh, vs->vd->image, 255);
	}
	if ((vs->vd->grabmouse == VSCR_MOUSEMODE_GRABBED) && (msg_cnt)) {
		int v = msg_cnt;
		gfx->draw_string(ds, x+msg_x-1, y+msg_y, GFX_RGB(0, 0, 0), 0, msg_fid, msg_txt);
		gfx->draw_string(ds, x+msg_x+1, y+msg_y, GFX_RGB(0, 0, 0), 0, msg_fid, msg_txt);
		gfx->draw_string(ds, x+msg_x, y+msg_y-1, GFX_RGB(0, 0, 0), 0, msg_fid, msg_txt);
		gfx->draw_string(ds, x+msg_x, y+msg_y+1, GFX_RGB(0, 0, 0), 0, msg_fid, msg_txt);
		gfx->draw_string(ds, x+msg_x, y+msg_y,   GFX_RGB(v, v, v), 0, msg_fid, msg_txt);
	}
	gfx->pop_clipping(ds);

	return 1;
}


static void (*orig_update) (VSCREEN *vs);

/**
 * Update widget after changes of its attributes
 */
static void vscr_update(VSCREEN *vs)
{
	EVENT event;

	if (vs->vd->update_flags & (VSCR_UPDATE_MOUSE_X | VSCR_UPDATE_MOUSE_Y)) {
		if (!(vs->vd->update_flags & VSCR_UPDATE_MOUSE_X)) vs_mx = userstate->get_mx();
		if (!(vs->vd->update_flags & VSCR_UPDATE_MOUSE_Y)) vs_my = userstate->get_my();

		event.type  = EVENT_MOTION;
		event.abs_x = vs_mx - vs->gen->get_abs_x(vs);
		event.abs_y = vs_my - vs->gen->get_abs_y(vs);
		event.rel_x = vs_mx - userstate->get_mx();
		event.rel_y = vs_my - userstate->get_my();
		vs->gen->handle_event(vs, &event, NULL);

		/* warp mouse to new position and handle the synthetic event */
		userstate->set_pos(vs_mx, vs_my);
	}
	orig_update(vs);
	vs->vd->update_flags = 0;
}


/**
 * Timer tick callback for grab userstate
 *
 * This callback is used to fade out the mouse-release-message.
 * The variable msg_cnt is set to 255 when the mouse is grabbed and faded
 * down to zero. For each step the part of the widget which displays the
 * message is redrawn.
 */
static void grab_tick_callback(WIDGET *w, int dx, int dy)
{
	if (msg_cnt > 70) {
		msg_cnt -= 2;
		if (w->vd->fps == 0) redraw->draw_widgetarea(w, msg_x-1, msg_y-1, msg_x+msg_w+1, msg_y+msg_h+1);
	} else if (msg_cnt > 0) {
		msg_cnt = 0;
		if (w->vd->fps == 0) redraw->draw_widgetarea(w, msg_x-1, msg_y-1, msg_x+msg_w+1, msg_y+msg_h+1);
	} else {
		msg_cnt = 0;
	}
}


/**
 * Handle events
 *
 * We have to take care about the mouse grab mode of the VScreen widget.
 * In grab-mode the mouse is grabbed by any press event onto the widget.
 * When grabbed, the mouse can be discharged by pressing [pause]. All
 * other events go through the normal way.
 */
static void (*orig_handle_event)(WIDGET *w, EVENT *, WIDGET *from);
static void vscr_handle_event(VSCREEN *vs, EVENT *e, WIDGET *from)
{
	char const *m;
	s32 app_id;
	if (e->type == EVENT_PRESS) {
		/* transition from grabbed to grab */
		if (vs->vd->grabmouse == VSCR_MOUSEMODE_GRABBED) {
			if (e->code == Input::KEY_PAUSE) {
				vs->vd->grabmouse = VSCR_MOUSEMODE_GRAB;
				m = vs->gen->get_bind_msg(vs, "discharge");
				app_id = vs->gen->get_app_id(vs);
				if (m) msg->send_action_event(app_id, "discharge", m);
				msg_cnt = 0;
				redraw->draw_widgetarea(vs, msg_x, msg_y, msg_x + msg_w, msg_y + msg_h);
				userstate->idle();
				return;
			}
		/* transition to grabbed mouse */
		} else if (vs->vd->grabmouse == VSCR_MOUSEMODE_GRAB) {
			WINDOW *w;
			
			/* top associated window */
			w = (WINDOW *)vs->gen->get_window(vs);
			if (w) w->win->top(w);

			vs->vd->grabmouse = VSCR_MOUSEMODE_GRABBED;
			m = vs->gen->get_bind_msg(vs, "catch");
			app_id = vs->gen->get_app_id(vs);
			if (m) msg->send_action_event(app_id, "catch", m);
			msg_x = (vs->wd->w - msg_w)/2;
			msg_y = (vs->wd->h - msg_h)/2;
			userstate->grab(vs, grab_tick_callback);
			msg_cnt = 255;
			return;
		}
	}
	/* scale values of motion event */
	if (e->type == EVENT_MOTION) {
		float xratio  = (float)vs->vd->xres / (float)vs->wd->w;
		float yratio  = (float)vs->vd->yres / (float)vs->wd->h;
		int old_abs_x = e->abs_x - e->rel_x;
		int old_abs_y = e->abs_y - e->rel_y;

		/* scale - we do not scale rel_x and rel_y directly to prevent aliasing */
		e->abs_x  = (float)e->abs_x  * xratio;
		e->abs_y  = (float)e->abs_y  * yratio;
		old_abs_x = (float)old_abs_x * xratio;
		old_abs_y = (float)old_abs_y * yratio;
		e->rel_x  = e->abs_x - old_abs_x;
		e->rel_y  = e->abs_y - old_abs_y;
	}
	orig_handle_event(vs, e, from);
}


/**
 * Release vscreen data
 *
 * Eventually, we have to exclude the vscreen from
 * the ring list of vscreens which share one buffer.
 */
static void vscr_free_data(VSCREEN *vs)
{
	/* free the mouse if it is currently grabbed inside the vscreen widget */
	if ((userstate->get() == USERSTATE_GRAB) && (userstate->get_selected() == vs))
		userstate->idle();
	
	if (vs->wd->ref_cnt == 0) vscr_share_exclude(vs);
}


/**
 * Return widget type identifier
 */
static char const *vscr_get_type(VSCREEN *vs)
{
	return "VScreen";
}


/******************************
 ** Vscreen specific methods **
 ******************************/

/**
 * Set vscreen mouse x position
 */
static void vscr_set_mx(VSCREEN *vs, s32 new_mx)
{
	if (vs->vd->grabmouse != VSCR_MOUSEMODE_GRABBED) return;
	vs_mx = new_mx + vs->gen->get_abs_x(vs);
	vs->vd->update_flags |= VSCR_UPDATE_MOUSE_X;
}


/**
 * Request vscreen mouse x position
 */
static s32 vscr_get_mx(VSCREEN *vs)
{
	if (vs->vd->grabmouse != VSCR_MOUSEMODE_GRABBED) return 0;
	return vs_mx - vs->gen->get_abs_x(vs);
}


/**
 * Set vscreen mouse y position
 */
static void vscr_set_my(VSCREEN *vs, s32 new_my)
{
	if (vs->vd->grabmouse != VSCR_MOUSEMODE_GRABBED) return;
	vs_my = new_my + vs->gen->get_abs_y(vs);
	vs->vd->update_flags |= VSCR_UPDATE_MOUSE_Y;
}


/**
 * Set width of a vscreen to a fixed value
 */
static void vscr_set_fixw(VSCREEN *vs, char *new_fixw)
{
	if (!new_fixw) return;
	if (streq("none", new_fixw, 4)) {
		vs->wd->min_w = 0;
		vs->wd->max_w = 99999;
	} else {
		vs->wd->min_w = vs->wd->max_w = atol(new_fixw);
	}
	vs->wd->update |= WID_UPDATE_MINMAX;
}


/**
 * Set height of a vscreen to a fixed value
 */
static void vscr_set_fixh(VSCREEN *vs, char *new_fixh)
{
	if (!new_fixh) return;
	if (streq("none", new_fixh, 4)) {
		vs->wd->min_h = 0;
		vs->wd->max_h = 99999;
	} else {
		vs->wd->min_h = vs->wd->max_h = atol(new_fixh);
	}
	vs->wd->update |= WID_UPDATE_MINMAX;
}


/**
 * Request vscreen mouse y position
 */
static s32 vscr_get_my(VSCREEN *vs)
{
	if (vs->vd->grabmouse != VSCR_MOUSEMODE_GRABBED) return 0;
	return vs_my - vs->gen->get_abs_y(vs);
}


/**
 * Set mouse grab mode of the vscreen widget
 */
static void vscr_set_grabmouse(VSCREEN *vs, s32 grab_flag)
{
	vs->vd->grabmouse = grab_flag ? VSCR_MOUSEMODE_GRAB : VSCR_MOUSEMODE_FREE;
}


/**
 * Request mouse grab mode
 */
static s32 vscr_get_grabmouse(VSCREEN *vs)
{
	if (vs->vd->grabmouse == VSCR_MOUSEMODE_FREE) return 0;
	return 1;
}


/**
 * Test if a given graphics mode is valid
 */
static s32 vscr_probe_mode(VSCREEN *vs, s32 width, s32 height, char *mode)
{
	if ((!streq(mode, "RGB16",  6))
	 && (!streq(mode, "YUV420", 7))
	 && (!streq(mode, "RGBA32", 7))) return 0;
	if (width*height <= 0) return 0;
	return 1;
}


/**
 * Set graphics mode
 */
static s32 vscr_set_mode(VSCREEN *vs, s32 width, s32 height, char *mode)
{
	int type = 0;

	if (!vscr_probe_mode(vs, width, height, mode)) return 0;

	/* destroy old image buffer and reset values */
	switch (vs->vd->bpp) {
	case 16:
	case 32:
		if (vs->vd->image) gfx->dec_ref(vs->vd->image);
	}

	vs->vd->bpp     = 0;
	vs->vd->xres    = 0;
	vs->vd->yres    = 0;
	vs->vd->pixels  = NULL;
	vs->vd->image   = NULL;
	vs->vd->vw      = vs->vd->vh      = 0;
	vs->vd->curr_vx = vs->vd->next_vx = 0;
	vs->vd->curr_vy = vs->vd->next_vy = 0;

	/* create new frame buffer image */
	if (streq("RGB16",  mode, 6)) type = GFX_IMG_TYPE_RGB16;
	if (streq("RGBA32", mode, 7)) type = GFX_IMG_TYPE_RGBA32;

	if (!type) {
		ERROR(printf("VScreen(set_mode): mode %s not supported!\n", mode);)
		return 0;
	}

	if ((vs->vd->image = gfx->alloc_img(width, height, (img_type)type))) {
		vs->vd->xres   = width;
		vs->vd->yres   = height;
		vs->vd->vw     = width;
		vs->vd->vh     = height;
		vs->vd->pixels = gfx->map(vs->vd->image);
		gfx->get_ident(vs->vd->image, &vs->vd->smb_ident[0]);
	} else {
		ERROR(printf("VScreen(set_mode): out of memory!\n");)
		return 0;
	}

	if (type == GFX_IMG_TYPE_RGBA32)
		vs->wd->flags &= ~WID_FLAGS_CONCEALING;
	else
		vs->wd->flags |= WID_FLAGS_CONCEALING;

	vs->gen->update(vs);
	return 1;
}


/**
 * Update a specified area of the widget
 */
static void vscr_refresh(VSCREEN *vs, s32 x, s32 y, s32 w, s32 h)
{
	VSCREEN *last = vs;
	int sx1, sy1, sx2, sy2;
	float mx, my;

	int cnt = 1;
	if (vs->vd->share_next) {
		while ((vs = vs->vd->share_next) != last) cnt++;
	}

	if (w == -1) w = vs->vd->xres;
	if (h == -1) h = vs->vd->yres;
	if ((w<1) || (h<1)) return;
	
	/* refresh all vscreens which share the same pixel buffer */
	while (cnt--) {
		mx = (float)vs->wd->w / (float)vs->vd->xres;
		my = (float)vs->wd->h / (float)vs->vd->yres;
		sx1 = x*mx;
		sy1 = y*my;
		sx2 = (int)((x + w)*mx);
		sy2 = (int)((y + h)*my);
		redraw->draw_widgetarea(vs, sx1, sy1, sx2, sy2);
		vs = vs->vd->share_next;
	}
}


/**
 * Map vscreen buffer to another thread's address space
 */
static char const *vscr_map(VSCREEN *vs, char *dst_thread_ident)
{
	char dst_th_buf[16];
	void *dst_th = (void *)(void *)dst_th_buf;
	
	if (!vs->vd->image) return "Error: VScreen mode not initialized.";

	gfx->share(vs->vd->image, dst_th);
	INFO(printf("VScreen(map): return vs->vd->smb_ident = %s\n", &vs->vd->smb_ident[0]));
	return &vs->vd->smb_ident[0];
}


/**
 * Share image buffer with other specified vscreen
 */
static void vscr_share(VSCREEN *vs, VSCREEN *from)
{
	int img_type = 0;
	GFX_CONTAINER *new_image;

	/* exclude widget from its previous ring list of buffer-shared vscreen */
	if (vs->vd->share_next) vscr_share_exclude(vs);

	if (!from || !(new_image = from->vscr->get_image(from))) return;

	/*
	 * Increment reference counter of new image before decrementing
	 * the reference counter of the old one. If both images are the
	 * same, we do not want to risk a reference counter of zero.
	 */
	gfx->inc_ref(new_image);

	/* replace old image with new one */
	if (vs->vd->image) gfx->dec_ref(vs->vd->image);
	vs->vd->image = new_image;

	vs->vd->vw = vs->vd->xres = gfx->get_width(vs->vd->image);
	vs->vd->vh = vs->vd->yres = gfx->get_height(vs->vd->image);
	img_type = gfx->get_type(vs->vd->image);
	switch (img_type) {
	case GFX_IMG_TYPE_RGB16:
		vs->vd->bpp = 16;
		break;
	case GFX_IMG_TYPE_RGBA32:
		vs->vd->bpp = 32;
		break;
	}
	vs->vd->pixels = gfx->map(vs->vd->image);

	vs->gen->set_w(vs, vs->vd->xres);
	vs->gen->set_h(vs, vs->vd->yres);
	vs->gen->updatepos(vs);

	/* join the new ring list of buffer sharing vscreens */
	vscr_share_join(from, vs);
}


static GFX_CONTAINER *vscr_get_image(VSCREEN *vs)
{
	return vs->vd->image;
}


static struct widget_methods gen_methods;
static struct vscreen_methods vscreen_methods = {
	vscr_refresh,
	vscr_get_image,
};


/***********************
 ** Service functions **
 ***********************/

static VSCREEN *create(void)
{
	VSCREEN *vs = ALLOC_WIDGET(struct vscreen);
	SET_WIDGET_DEFAULTS(vs, struct vscreen, &vscreen_methods);

	/* set widget type specific data */
	vs->wd->flags |= WID_FLAGS_CONCEALING | WID_FLAGS_EDITABLE;
	return vs;
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct vscreen_services services = {
	create
};


/************************
 ** Module entry point **
 ************************/

static void build_script_lang(void)
{
	widtype *widtype;

	widtype = script->reg_widget_type("VScreen", (void *(*)(void))create);

	script->reg_widget_method(widtype, "long probemode(long width, long height, string mode)", (void *)vscr_probe_mode);
	script->reg_widget_method(widtype, "long setmode(long width, long height, string mode)", (void *)vscr_set_mode);
	script->reg_widget_method(widtype, "string map(string thread=\"caller\")", (void *)vscr_map);
	script->reg_widget_method(widtype, "void refresh(long x=0, long y=0, long w=-1, long h=-1)", (void *)vscr_refresh);
	script->reg_widget_method(widtype, "void share(Widget from)", (void *)vscr_share);

	script->reg_widget_attrib(widtype, "string fixw", NULL, (void *)vscr_set_fixw, (void *)gen_methods.update);
	script->reg_widget_attrib(widtype, "string fixh", NULL, (void *)vscr_set_fixh, (void *)gen_methods.update);
	script->reg_widget_attrib(widtype, "long mousex", (void *)vscr_get_mx, (void *)vscr_set_mx, (void *)gen_methods.update);
	script->reg_widget_attrib(widtype, "long mousey", (void *)vscr_get_my, (void *)vscr_set_my, (void *)gen_methods.update);
	script->reg_widget_attrib(widtype, "boolean grabmouse", (void *)vscr_get_grabmouse, (void *)vscr_set_grabmouse, (void *)gen_methods.update);
	widman->build_script_lang(widtype, &gen_methods);
}


int init_vscreen(struct dope_services *d)
{
	gfx       = (gfx_services       *)(d->get_module("Gfx 1.0"));
	script    = (script_services    *)(d->get_module("Script 1.0"));
	widman    = (widman_services    *)(d->get_module("WidgetManager 1.0"));
	redraw    = (redraw_services    *)(d->get_module("RedrawManager 1.0"));
	appman    = (appman_services    *)(d->get_module("ApplicationManager 1.0"));
	sched     = (scheduler_services *)(d->get_module("Scheduler 1.0"));
	msg       = (messenger_services *)(d->get_module("Messenger 1.0"));
	userstate = (userstate_services *)(d->get_module("UserState 1.0"));
	font      = (fontman_services   *)(d->get_module("FontManager 1.0"));

	/* define general widget functions */
	widman->default_widget_methods(&gen_methods);

	orig_update          = gen_methods.update;
	orig_handle_event    = gen_methods.handle_event;
	
	gen_methods.get_type     = vscr_get_type;
	gen_methods.draw         = vscr_draw;
	gen_methods.update       = vscr_update;
	gen_methods.free_data    = vscr_free_data;
	gen_methods.handle_event = vscr_handle_event;

	build_script_lang();

	msg_fid = 0;
	msg_txt = "press [pause] to release mouse";
	msg_w = font->calc_str_width(msg_fid, msg_txt);
	msg_h = font->calc_str_height(msg_fid, msg_txt);

	d->register_module("VScreen 1.0", &services);
	return 1;
}
