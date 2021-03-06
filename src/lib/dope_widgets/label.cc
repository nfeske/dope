/*
 * \brief   DOpE Label widget module
 * \date    2002-05-15
 * \author  Norman Feske
 */

/*
 * Copyright (C) 2002-2007 Norman Feske
 * Copyright (C) 2008-2014 Genode Labs GmbH
 *
 * This file is part of the DOpE package, which is distributed under
 * the terms of the GNU General Public Licence 2.
 */

struct label;
#define WIDGET struct label

#include "dopestd.h"
#include "gfx.h"
#include "widget_data.h"
#include "widget_help.h"
#include "variable.h"
#include "label.h"
#include "fontman.h"
#include "script.h"
#include "widman.h"

static struct widman_services  *widman;
static struct gfx_services     *gfx;
static struct fontman_services *font;
static struct script_services  *script;

struct label_data {
	char const *text;
	s16         font_id;
	s16         tx, ty;       /* text position inside the label cell */
	s16         pad_x, pad_y;
	VARIABLE   *var;
};

int init_label(struct dope_services *d);

#define BLACK_SOLID GFX_RGBA(0, 0, 0, 255)


/********************************
 ** Functions for internal use **
 ********************************/

static void update_text_pos(LABEL *l)
{
	if (!l->ld->text) return;
	l->ld->tx = (l->wd->w - font->calc_str_width (l->ld->font_id, l->ld->text))>>1;
	l->ld->ty = (l->wd->h - font->calc_str_height(l->ld->font_id, l->ld->text))>>1;
}


/****************************
 ** General widget methods **
 ****************************/

static int lab_draw(LABEL *l, struct gfx_ds *ds, long x, long y, WIDGET *origin)
{
	int tx = x + l->wd->x + l->ld->tx;
	int ty = y + l->wd->y + l->ld->ty;

	if (origin == l) return 1;
	if (origin) return 0;

	gfx->push_clipping(ds, x + l->wd->x, y + l->wd->y, l->wd->w, l->wd->h);
	if (l->ld->text) {
		gfx->draw_string(ds, tx, ty, BLACK_SOLID, 0, l->ld->font_id, l->ld->text);
	}
	gfx->pop_clipping(ds);

	return 1;
}


/**
 * Determine min/max size of a label widget
 */
static void lab_calc_minmax(LABEL *l)
{
	if (l->ld->text) {
		l->wd->min_w = l->wd->max_w = font->calc_str_width (l->ld->font_id, l->ld->text)
		                            + l->ld->pad_x * 2;
		l->wd->min_h = l->wd->max_h = font->calc_str_height(l->ld->font_id, l->ld->text)
		                            + l->ld->pad_y * 2;
	} else {
		l->wd->min_w = l->wd->min_h = l->ld->pad_x * 2;
		l->wd->max_w = l->wd->max_h = l->ld->pad_y * 2;
	}
}


static void (*orig_updatepos) (LABEL *l);
static void lab_updatepos(LABEL *l)
{
	update_text_pos(l);
	orig_updatepos(l);
}


/**
 * Free label widget data
 */
static void lab_free_data(LABEL *l)
{
	if (l->ld->text) free(l->ld->text);
}


/**
 * Return widget type identifier
 */
static char const *lab_get_type(LABEL *l)
{
	return "Label";
}


/****************************
 ** Label specific methods **
 ****************************/

static void lab_set_text(LABEL *l, char const *new_txt)
{
	if ((!l) || (!l->ld)) return;
	if (l->ld->text) free(l->ld->text);
	l->ld->text = strdup(new_txt);
	l->wd->update |= WID_UPDATE_MINMAX;
}


static char const *lab_get_text(LABEL *l)
{
	return l->ld->text;
}


/**
 * Set font of label
 *
 * Currently, only the font identifiers 'default' and
 * 'monospaced' are defined.
 */
static void lab_set_font(LABEL *l, char const *fontname)
{
	if (streq(fontname, "default", 255)) {
		l->ld->font_id = 0;
	} else if (streq(fontname, "monospaced", 255)) {
		l->ld->font_id = 1;
	}
	l->wd->update |= WID_UPDATE_MINMAX;
}


/**
 * Request currently used font of a label
 */
static char const *lab_get_font(LABEL *l)
{
	switch (l->ld->font_id) {
		case 1:  return "monospaced";
		default: return "default";
	}
}


/**
 * Callback on variable modifications
 *
 * This routine is called everytime the variable get assigned
 * a new value. We just set the label text to the new value.
 */
static void lab_var_notify(LABEL *l, VARIABLE *v)
{
	lab_set_text(l, v->var->get_string(v));
	l->gen->update(l);
}


/**
 * Connect label to a variable
 *
 * The label will always display the current value of the
 * variable.
 */
static void lab_set_var(LABEL *l, VARIABLE *v)
{
	if (l->ld->var) {
		l->ld->var->gen->dec_ref((WIDGET *)l->ld->var);
		l->ld->var->var->disconnect(l->ld->var, l);
	}

	l->ld->var = v;
	if (v) {
		v->gen->inc_ref((WIDGET *)v);
		v->var->connect(v, l, lab_var_notify);
		lab_set_text(l, v->var->get_string(v));
	} else {
		lab_set_text(l, "");
	}
	l->wd->update |= WID_UPDATE_MINMAX;
}


static VARIABLE *lab_get_var(LABEL *l)
{
	return l->ld->var;
}


static struct widget_methods gen_methods;
static struct label_methods lab_methods = {
	lab_set_text,
	lab_get_text,
	lab_set_font,
	lab_get_font,
};


/***********************
 ** Service functions **
 ***********************/

static LABEL *create(void)
{
	LABEL *label = ALLOC_WIDGET(struct label);
	SET_WIDGET_DEFAULTS(label, struct label, &lab_methods);

	/* set label specific attributes */
	label->ld->pad_x = label->ld->pad_y = 2;
	label->ld->text  = strdup("");
	update_text_pos(label);
	gen_methods.update(label);

	return label;
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct label_services services = {
	create
};


/************************
 ** Module entry point **
 ************************/

static void build_script_lang(void)
{
	widtype *widtype;

	widtype = script->reg_widget_type("Label", (void *(*)(void))create);

	script->reg_widget_attrib(widtype, "string text", (void *)lab_get_text, (void *)lab_set_text, (void *)gen_methods.update);
	script->reg_widget_attrib(widtype, "string font", (void *)lab_get_font, (void *)lab_set_font, (void *)gen_methods.update);
	script->reg_widget_attrib(widtype, "Variable variable", (void *)lab_get_var, (void *)lab_set_var, (void *)gen_methods.update);

	widman->build_script_lang(widtype, &gen_methods);
}


int init_label(struct dope_services *d)
{
	widman  = (widman_services  *)(d->get_module("WidgetManager 1.0"));
	gfx     = (gfx_services     *)(d->get_module("Gfx 1.0"));
	font    = (fontman_services *)(d->get_module("FontManager 1.0"));
	script  = (script_services  *)(d->get_module("Script 1.0"));

	/* define general widget functions */
	widman->default_widget_methods(&gen_methods);

	orig_updatepos = gen_methods.updatepos;

	gen_methods.draw        = lab_draw;
	gen_methods.updatepos   = lab_updatepos;
	gen_methods.get_type    = lab_get_type;
	gen_methods.calc_minmax = lab_calc_minmax;
	gen_methods.free_data   = lab_free_data;

	build_script_lang();

	d->register_module("Label 1.0", &services);
	return 1;
}
