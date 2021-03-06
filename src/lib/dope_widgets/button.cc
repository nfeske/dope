/*
 * \brief   DOpE Button widget module
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

struct button;
#define WIDGET struct button

#include <input/keycodes.h>

/* local includes */
#include "dopestd.h"
#include "button.h"
#include "gfx_macros.h"
#include "widget_data.h"
#include "widget_help.h"
#include "fontman.h"
#include "script.h"
#include "widman.h"
#include "userstate.h"
#include "messenger.h"

static struct widman_services    *widman;
static struct gfx_services       *gfx;
static struct fontman_services   *font;
static struct script_services    *script;
static struct userstate_services *userstate;
static struct messenger_services *msg;

struct button_data {
	char const *text;
	s16         style;
	s16         font_id;
	s16         tx, ty;                    /* text position inside the button */
	s16         pad_x, pad_y;
	u16         flags;
	void      (*click)  (WIDGET *);
	void      (*release)(WIDGET *);
};

enum {
	BUTTON_FLAGS_FREE_W = 0x01,    /* allow all widths  */
	BUTTON_FLAGS_FREE_H = 0x02,    /* allow all heights */
};

static GFX_CONTAINER *normal_img;
static GFX_CONTAINER *focus_img;
static GFX_CONTAINER *actwin_img;

extern int config_clackcommit;

int init_button(struct dope_services *d);

static const color_t BLACK_SOLID = GFX_RGBA(0, 0, 0, 255);
static const color_t BLACK_MIXED = GFX_RGBA(0, 0, 0, 127);
static const color_t WHITE_SOLID = GFX_RGBA(255, 255, 255, 255);
static const color_t WHITE_MIXED = GFX_RGBA(255, 255, 255, 127);
static const color_t DARK_GREY   = GFX_RGBA(80, 80, 80, 255);


/********************************
 ** Functions for internal use **
 ********************************/

static void update_text_pos(BUTTON *b)
{
	if (!b->bd->text) return;
	b->bd->tx = (b->wd->w - 2*b->bd->pad_x - font->calc_str_width (b->bd->font_id, b->bd->text))>>1;
	b->bd->ty = (b->wd->h - 2*b->bd->pad_y - font->calc_str_height(b->bd->font_id, b->bd->text))>>1;
}


static inline void draw_focus_frame(GFX_CONTAINER *d, s32 x, s32 y, s32 w, s32 h)
{
	gfx->draw_hline(d, x, y, w, WHITE_MIXED);
	gfx->draw_vline(d, x, y, h, WHITE_MIXED);
	gfx->draw_hline(d, x, y + h - 1, w, BLACK_SOLID);
	gfx->draw_vline(d, x + w - 1, y, h, BLACK_SOLID);
}


static inline void draw_raised_frame(GFX_CONTAINER *d, s32 x, s32 y, s32 w, s32 h)
{
	/* outer frame */
	gfx->draw_hline(d, x, y, w, WHITE_MIXED);
	gfx->draw_vline(d, x, y, h, WHITE_MIXED);
	gfx->draw_hline(d, x, y + h - 1, w, BLACK_SOLID);
	gfx->draw_vline(d, x + w - 1, y, h, BLACK_SOLID);

	/* inner frame */
	gfx->draw_hline(d, x + 1, y + 1, w - 2, WHITE_MIXED);
	gfx->draw_vline(d, x + 1, y + 1, h - 2, WHITE_MIXED);
	gfx->draw_hline(d, x + 1, y + 1, w - 2, WHITE_MIXED);
	gfx->draw_vline(d, x + 1, y + 1, h - 2, WHITE_MIXED);
	gfx->draw_hline(d, x + 1, y + h - 2, w - 2, BLACK_MIXED);
	gfx->draw_vline(d, x + w - 2, y + 1, h - 2, BLACK_MIXED);

	/* spot */
	gfx->draw_hline(d, x + 1, y + 1, 1, WHITE_SOLID);
	gfx->draw_vline(d, x + 1, y + 1, 1, WHITE_SOLID);
}


static inline void draw_pressed_frame(GFX_CONTAINER *d, s32 x, s32 y, s32 w, s32 h)
{
	/* outer frame */
	gfx->draw_hline(d, x, y, w, BLACK_SOLID);
	gfx->draw_vline(d, x, y, h, BLACK_SOLID);
	gfx->draw_hline(d, x, y + h - 1, w, WHITE_MIXED);
	gfx->draw_vline(d, x + w - 1, y, h, WHITE_MIXED);

	/* inner frame */
	gfx->draw_hline(d, x + 1, y + 1, w - 2, BLACK_SOLID);
	gfx->draw_vline(d, x + 1, y + 1, h - 2, BLACK_SOLID);
	gfx->draw_hline(d, x + 1, y + h - 2, w - 2, WHITE_SOLID);
	gfx->draw_vline(d, x + w - 2, y + 1, h - 2, WHITE_SOLID);
}


static inline void draw_kfocus_frame(GFX_CONTAINER *d, s32 x, s32 y, s32 w, s32 h)
{
	gfx->draw_hline(d, x + 1,     y,         w - 2, BLACK_SOLID);
	gfx->draw_vline(d, x,         y,         h,     BLACK_SOLID);
	gfx->draw_hline(d, x + 1,     y + h - 1, w - 2, BLACK_SOLID);
	gfx->draw_vline(d, x + w - 1, y,         h,     BLACK_SOLID);
}


/****************************
 ** General widget methods **
 ****************************/

static int but_draw(BUTTON *b, struct gfx_ds *ds, long x, long y, WIDGET *origin)
{
	int tx = b->bd->tx, ty = b->bd->ty;
	int w  = b->wd->w,  h  = b->wd->h;

	/* we hit the origin, acknowledge visibility request */
	if (origin == b) return 1;

	/* only draw if no origin is specified */
	if (origin) return 0;

	x += b->wd->x;
	y += b->wd->y;

	gfx->push_clipping(ds, x, y, w, h);

	x += b->bd->pad_x;
	y += b->bd->pad_y;
	w -= b->bd->pad_x*2;
	h -= b->bd->pad_y*2;

	/* draw keyboard focus frame if button is currently selected */
	if (b->wd->flags & WID_FLAGS_KFOCUS)
		draw_kfocus_frame(ds, x - 1, y - 1, w + 2, h + 2);

	if (b->wd->flags & WID_FLAGS_MFOCUS) {
		gfx->draw_img(ds, x, y, w, h, focus_img, 255);

		if (!(b->wd->flags & WID_FLAGS_STATE)) {
			gfx->draw_img(ds, x, y, w, h, focus_img, 255);
			draw_focus_frame(ds, x, y, w, h);
		}
	} else {
		if (b->bd->style == 2) {
			gfx->draw_img(ds, x, y, w, h, actwin_img, 255);
		} else {
			gfx->draw_img(ds, x, y, w, h, normal_img, 255);
		}
	}

	if (b->wd->flags & WID_FLAGS_STATE) {
		draw_pressed_frame(ds, x, y, w, h);
	} else {
		if (!(b->wd->flags & WID_FLAGS_MFOCUS)) {
			draw_raised_frame(ds, x, y, w, h);
		}
	}

	tx += x; ty += y;
	if (b->wd->flags & WID_FLAGS_MFOCUS) {
		tx += 1; ty += 1;
	}
	if (b->wd->flags & WID_FLAGS_STATE) {
		tx += 2; ty += 2;
	}
	gfx->push_clipping(ds, x + 2, y + 2, w - 4, h - 4);

	if (b->bd->text) {
		switch (b->bd->style) {
			case 1:
				gfx->draw_string(ds, tx, ty, BLACK_SOLID, 0, b->bd->font_id, b->bd->text);
				break;
			case 2:
				gfx->draw_string(ds, tx, ty, DARK_GREY, 0, b->bd->font_id, b->bd->text);
				break;
			default:
				gfx->draw_string(ds, tx, ty, DARK_GREY, 0, b->bd->font_id, b->bd->text);
				break;
		}
	}

	gfx->pop_clipping(ds);
	gfx->pop_clipping(ds);

	return 1;
}


static void but_untouch_callback(BUTTON *b, int dx, int dy)
{
	char const *clack_msg, *commit_msg = NULL;
	
	if (!b->gen->get_state(b)) return;
	
	clack_msg = b->gen->get_bind_msg(b, "clack");
	if (clack_msg) msg->send_action_event(b->gen->get_app_id(b), "clack", clack_msg);

	commit_msg = b->gen->get_bind_msg(b, "commit");
	if (commit_msg && config_clackcommit)
		msg->send_action_event(b->gen->get_app_id(b), "commit", commit_msg);
}


static void (*orig_handle_event) (BUTTON *b, EVENT *e, WIDGET *from);
static void but_handle_event(BUTTON *b, EVENT *e, WIDGET *from)
{
	char const *click_msg, *clack_msg, *commit_msg = NULL;
	switch (e->type) {

	case EVENT_PRESS:

		/* check for mouse button event */
		if ((e->code != Input::BTN_MOUSE) && (e->code != Input::KEY_SPACE)) break;

		if (b->bd->click) b->bd->click(b);

		click_msg  = b->gen->get_bind_msg(b, "click");
		clack_msg  = b->gen->get_bind_msg(b, "clack");
		commit_msg = b->gen->get_bind_msg(b, "commit");

		if (click_msg || clack_msg || commit_msg)
			userstate->touch(b, NULL, but_untouch_callback);

		if (click_msg)
			msg->send_action_event(b->gen->get_app_id(b), "click", click_msg);

		if (commit_msg && !config_clackcommit)
			msg->send_action_event(b->gen->get_app_id(b), "commit", commit_msg);

		if (click_msg || clack_msg || commit_msg || b->bd->click) return;
		break;

	case EVENT_RELEASE:
		/* check for mouse button event */
		if (e->code == Input::BTN_MOUSE) {
			if (b->bd->release) b->bd->release(b);
		}
		break;
	}

	if (b->bd->click) return;
	orig_handle_event(b, e, from);
}


/**
 * Determine min/max size of a button
 *
 * The height and the min width of a Button depend on the used font and
 * the Button text. For the DOpE-internal use of Buttons for the window
 * controls there exist the FREE_W and FREE_H flags. When set, the size
 * of a Button is completely free.
 */
static void but_calc_minmax(BUTTON *b)
{
	/* ensure that there is enough padding to draw the focus frame */
	if (b->wd->flags & WID_FLAGS_TAKEFOCUS) {
		if (b->bd->pad_x < 1) b->bd->pad_x++;
		if (b->bd->pad_y < 1) b->bd->pad_y++;
	}

	/* the min/max height of the button depends on its text */
	if (b->bd->text) {
		b->wd->min_w = font->calc_str_width (b->bd->font_id, b->bd->text)
		             + b->bd->pad_x * 2 + 6;
		b->wd->min_h = font->calc_str_height(b->bd->font_id, b->bd->text)
		             + b->bd->pad_y * 2 + 6;
		b->wd->max_w = 9999;
		b->wd->max_h = b->wd->min_h;
	} else {
		b->wd->min_w = b->wd->min_h = 0;
		b->wd->max_w = b->wd->max_h = 9999;
	}

	/* do not constrain the min/max properties for free buttons */
	if (b->bd->flags & BUTTON_FLAGS_FREE_W) {
		b->wd->min_w = 0;
		b->wd->max_w = 9999;
	}
	if (b->bd->flags & BUTTON_FLAGS_FREE_H) {
		b->wd->min_h = 0;
		b->wd->max_h = 9999;
	}
}


/**
 * Update position and size of button
 *
 * When size changes, the text position must be updated.
 */
static void (*orig_updatepos) (BUTTON *b);
static void but_updatepos(BUTTON *b)
{
	update_text_pos(b);
	orig_updatepos(b);
}


/**
 * Free button widget data
 */
static void but_free_data(BUTTON *b)
{
	if (b->bd->text) free(b->bd->text);
}


/**
 * Catch bind calls to enable the keyboard focus
 */
static void (*orig_bind) (BUTTON *b, char const *bind_ident, char const *message);
static void but_bind(BUTTON *b, char const *bind_ident, char const *message)
{
	b->wd->flags |= WID_FLAGS_TAKEFOCUS;
	orig_bind(b, bind_ident, message);
}


/**
 * Return widget type identifier
 */
static char const *but_get_type(BUTTON *b)
{
	return "Button";
}


/*****************************
 ** Button specific methods **
 *****************************/

static void but_set_text(BUTTON *b, char const *new_txt)
{
	if (b->bd->text) {
		free(b->bd->text);
	}
	b->bd->text = strdup(new_txt);

	/*
	 * If a button's size is completely free, a change
	 * of the text has no effect on the layout of the
	 * parent. In this case, it is enough to do a
	 * refresh instead of giving a MINMAX notification.
	 */
	if (!(b->bd->flags & BUTTON_FLAGS_FREE_W)
	 || !(b->bd->flags & BUTTON_FLAGS_FREE_H))
		b->wd->update |= WID_UPDATE_MINMAX;
	else
		b->wd->update |= WID_UPDATE_REFRESH;
}


static char const *but_get_text(BUTTON *b)
{
	return b->bd->text;
}


static void but_set_font(BUTTON *b, s32 font_id)
{
	b->bd->font_id = font_id;
	b->wd->update |= WID_UPDATE_MINMAX;
}


static s32 but_get_font(BUTTON *b)
{
	return b->bd->font_id;
}


static void but_set_style(BUTTON *b, s32 style)
{
	b->bd->style = style;
	b->wd->update |= WID_UPDATE_REFRESH;
}


static s32 but_get_style(BUTTON *b)
{
	return b->bd->font_id;
}


static void but_set_click(BUTTON *b, void (*callback)(BUTTON *b))
{
	b->bd->click = callback;
}


static void but_set_release(BUTTON *b, void (*callback)(BUTTON *b))
{
	b->bd->release = callback;
}


static void but_set_free_w(BUTTON *b, int free_w_flag)
{
	if (free_w_flag) b->bd->flags |= BUTTON_FLAGS_FREE_W;
	else b->bd->flags &= ~BUTTON_FLAGS_FREE_W;
	b->wd->update |= WID_UPDATE_MINMAX;
}


static void but_set_free_h(BUTTON *b, int free_h_flag)
{
	if (free_h_flag) b->bd->flags |= BUTTON_FLAGS_FREE_H;
	else b->bd->flags &= ~BUTTON_FLAGS_FREE_H;
	b->wd->update |= WID_UPDATE_MINMAX;
}


static void but_set_pad_x(BUTTON *b, int pad_x)
{
	if (pad_x < 0) return;
	b->bd->pad_x = pad_x;
	b->wd->update |= WID_UPDATE_MINMAX;
}


static int but_get_pad_x(BUTTON *b)
{
	return b->bd->pad_x;
}


static void but_set_pad_y(BUTTON *b, int pad_y)
{
	if (pad_y < 0) return;
	b->bd->pad_y = pad_y;
	b->wd->update |= WID_UPDATE_MINMAX;
}


static int but_get_pad_y(BUTTON *b)
{
	return b->bd->pad_y;
}


static struct widget_methods gen_methods;
static struct button_methods but_methods = {
	but_set_text,
	but_get_text,
	but_set_font,
	but_get_font,
	but_set_style,
	but_get_style,
	but_set_click,
	but_set_release,
	but_set_free_w,
	but_set_free_h,
	but_set_pad_x,
	but_set_pad_y,
};


/***********************
 ** Service functions **
 ***********************/

static BUTTON *create(void)
{
	BUTTON *button = ALLOC_WIDGET(struct button);
	SET_WIDGET_DEFAULTS(button, struct button, &but_methods);

	/* set button specific attributes */
	button->bd->style  = 1;
	button->bd->pad_x  = button->bd->pad_y = 2;
	button->wd->flags |= WID_FLAGS_HIGHLIGHT | WID_FLAGS_SELECTABLE;

	update_text_pos(button);
	but_calc_minmax(button);
	return button;
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct button_services services = {
	create
};


/************************
 ** Module entry point **
 ************************/

static void build_script_lang(void)
{
	struct widtype *widtype;

	widtype = script->reg_widget_type("Button", (void *(*)(void))create);
	script->reg_widget_attrib(widtype, "string text", (void *)but_get_text, (void *)but_set_text, (void *)gen_methods.update);
	script->reg_widget_attrib(widtype, "long padx", (void *)but_get_pad_x, (void *)but_set_pad_x, (void *)gen_methods.update);
	script->reg_widget_attrib(widtype, "long pady", (void *)but_get_pad_y, (void *)but_set_pad_y, (void *)gen_methods.update);
	widman->build_script_lang(widtype, &gen_methods);
}


int init_button(struct dope_services *d)
{
	widman    = (widman_services    *)(d->get_module("WidgetManager 1.0"));
	gfx       = (gfx_services       *)(d->get_module("Gfx 1.0"));
	font      = (fontman_services   *)(d->get_module("FontManager 1.0"));
	script    = (script_services    *)(d->get_module("Script 1.0"));
	userstate = (userstate_services *)(d->get_module("UserState 1.0"));
	msg       = (messenger_services *)(d->get_module("Messenger 1.0"));

	normal_img = gen_range_img(gfx, 128, 128, 128,   90 + 50,  90  + 50, 110 + 50);
	focus_img  = gen_range_img(gfx, 128, 128, 128,   90 + 40,  90  + 40, 110 + 40);
	actwin_img = gen_range_img(gfx, 128, 128, 128,  110 + 50,  110 + 50,  90 + 50);

	/* define general widget functions */
	widman->default_widget_methods(&gen_methods);

	orig_updatepos    = gen_methods.updatepos;
	orig_handle_event = gen_methods.handle_event;
	orig_bind         = gen_methods.bind;

	gen_methods.get_type     = but_get_type;
	gen_methods.draw         = but_draw;
	gen_methods.updatepos    = but_updatepos;
	gen_methods.handle_event = but_handle_event;
	gen_methods.calc_minmax  = but_calc_minmax;
	gen_methods.free_data    = but_free_data;
	gen_methods.bind         = but_bind;

	build_script_lang();

	d->register_module("Button 1.0", &services);
	return 1;
}
