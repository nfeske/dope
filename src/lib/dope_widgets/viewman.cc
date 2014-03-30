/*
 * \brief   Genode version of DOpE's view manager
 * \date    2007-07-26
 * \author  Norman Feske
 */

/*
 * Copyright (C) 2002-2007 Norman Feske
 * Copyright (C) 2008-2014 Genode Labs GmbH
 *
 * This file is part of the DOpE package, which is distributed under
 * the terms of the GNU General Public Licence 2.
 */

/* local inlcudes */
#include "dopestd.h"
#include "viewman.h"

/* Genode includes */
#include <base/env.h>
#include <nitpicker_session/nitpicker_session.h>
#include <nitpicker_view/client.h>

extern Nitpicker::Session *nitpicker_session;

int init_viewman(struct dope_services *d);

struct view {
	Nitpicker::View_capability cap;

	/**
	 * Constructor
	 */
	view() {
		cap = nitpicker_session->create_view();
		Nitpicker::View_client(cap).stack(Nitpicker::View_capability(), true, true);
	}

	/**
	 * Destructor
	 */
	~view() {
		nitpicker_session->destroy_view(cap);
	}
};


/***********************
 ** Service functions **
 ***********************/

/**
 * Create a new view
 *
 * \return  view id of the new view or a negative error code
 */
static struct view *view_create(void)
{
	struct view *v = new (Genode::env()->heap()) view;

	Nitpicker::View_client(v->cap).title("DOpE");
	return v;
}


/**
 * Destroy view
 */
static void view_destroy(struct view *v)
{
	using namespace Genode;
	destroy(env()->heap(), v);
}


/**
 * Position view
 *
 * \return  0 on success or a negative error code
 */
static int view_place(struct view *v, int x, int y, int w, int h)
{
	if (!v->cap.valid()) return -1;
	return Nitpicker::View_client(v->cap).viewport(x, y, w, h, -x, -y, false);
}


/**
 * Bring view on top
 *
 * \return  0 on success or a negative error code
 */
static int view_top(struct view *v)
{
	if (!v->cap.valid()) return -1;
	return Nitpicker::View_client(v->cap).stack(Nitpicker::View_capability(), true, false);
}


/**
 * Bring view to back
 *
 * \return  0 on success or a negative error code
 */
static int view_back(struct view *v)
{
	if (!v->cap.valid()) return -1;
	return Nitpicker::View_client(v->cap).stack(Nitpicker::View_capability(), false, false);
}


/**
 * Set title of a view
 *
 * \return  0 on success or a negative error code
 */
static int view_set_title(struct view *v, const char *title)
{
	if (!v->cap.valid()) return -1;
	return Nitpicker::View_client(v->cap).title(title);
}


/**
 * Define background view
 *
 * \return  0 on success or a negative error code
 */
static int view_set_bg(struct view *v)
{
	nitpicker_session->background(v->cap);
	return 0;
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct viewman_services services = {
	view_create,
	view_destroy,
	view_place,
	view_top,
	view_back,
	view_set_title,
	view_set_bg,
};


/************************
 ** Module entry point **
 ************************/

int init_viewman(struct dope_services *d) {
	d->register_module("ViewManager 1.0", &services);
	return 1;
}
