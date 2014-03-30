/*
 * \brief   Genode version of DOpE input driver module
 * \date    2007-07-27
 * \author  Norman Feske
 */

/*
 * Copyright (C) 2002-2007 Norman Feske
 * Copyright (C) 2006-2013 Genode Labs GmbH
 *
 * This file is part of the DOpE package, which is distributed under
 * the  terms  of the  GNU General Public Licence 2.
 */

#include <input/keycodes.h>
#include <input_session/input_session.h>
#include <input/event.h>
#include <base/env.h>

/* local includes */
#include "dopestd.h"
#include "event.h"
#include "input.h"


static int mx, my;                     /* current absolute mouse position */
static int curr_ev, num_ev;            /* current and maximum event index */

extern Input::Session *input_session;  /* initialized by scrdrv.cc  */
static Input::Event *ev_buf;           /* Genode input event buffer */

int init_input(struct dope_services *d);


/************************
 ** Utility functions ***
 ************************/

/**
 * Fill dope event structure
 */
static inline void assign_event(EVENT *e, int type, int code, int ax, int ay)
{
	e->type = type;
	e->code = code;
	e->abs_x = ax;
	e->abs_y = ay;
	e->rel_x = 0;
	e->rel_y = 0;
}


/***********************
 ** Service functions **
 ***********************/

/**
 * Get next event of event queue
 *
 * \return  0 if there is no pending event
 *          1 if there an event was returned in out parameter e
 */
static int get_event(EVENT *e) {

	/* if local event queue is empty, fetch new events from Genode input */
	if (curr_ev == num_ev) {
		num_ev = input_session->flush();
		curr_ev = 0;
	}

	/* return if there is still no event in event queue */
	if (curr_ev == num_ev)
		return 0;

	Input::Event *ev = &ev_buf[curr_ev];

	switch (ev->type()) {
	case Input::Event::PRESS:
		assign_event(e, EVENT_PRESS, ev->keycode(), mx, my);
		break;

	case Input::Event::RELEASE:
		assign_event(e, EVENT_RELEASE, ev->keycode(), mx, my);
		break;

	case Input::Event::MOTION:
		mx = ev->ax();
		my = ev->ay();
		assign_event(e, EVENT_ABSMOTION, 0, mx, my);
		break;

	default:
		break;
	}

	curr_ev++;
	return 1;
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct input_services input = {
	get_event,
};


/************************
 ** Module entry point **
 ************************/

int init_input(struct dope_services *d)
{
	using namespace Genode;

	void *addr = env()->rm_session()->attach(input_session->dataspace());
	ev_buf = (Input::Event *)addr;

	d->register_module("Input 1.0",&input);
	return 1;
}
