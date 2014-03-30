/*
 * \brief   DOpE messenger module
 * \date    2004-05-31
 * \author  Norman Feske
 */

/*
 * Copyright (C) 2002-2007 Norman Feske
 * Copyright (C) 2008-2014 Genode Labs GmbH
 *
 * This file is part of the DOpE package, which is distributed under
 * the terms of the GNU General Public Licence 2.
 */

#include <dope/dopelib.h>

/* local includes */
#include "dopestd.h"
#include "event.h"
#include "messenger.h"

int init_messenger(struct dope_services *d);


/********************************
 ** Functions for internal use **
 ********************************/

static unsigned long hex2u32(const char *s)
{
	int i;
	unsigned long result=0;
	for (i=0;i<8;i++,s++) {
		if (!(*s)) return result;
		result = result*16 + (*s & 0xf);
		if (*s > '9') result += 9;
	}
	return result;
}


/***********************
 ** Service functions **
 ***********************/

static void send_input_event(s32 app_id, EVENT *e, char const *bindarg)
{
	void (*callback) (Event_union *, void *);
	void *arg;
	Event_union de;

	callback = (void (*)(Event_union *,void*))hex2u32(bindarg);
	arg = (void *)hex2u32(bindarg+10);

	switch (e->type) {

	case EVENT_MOUSE_ENTER:
		de.type = EVENT_TYPE_COMMAND;
		de.command.cmd = "enter";
		break;

	case EVENT_MOUSE_LEAVE:
		de.type = EVENT_TYPE_COMMAND;
		de.command.cmd = "leave";
		break;

	case EVENT_MOTION:
		de.type = EVENT_TYPE_MOTION;
		de.motion.rel_x = e->rel_x;
		de.motion.rel_y = e->rel_y;
		de.motion.abs_x = e->abs_x;
		de.motion.abs_y = e->abs_y;
		break;

	case EVENT_PRESS:
		de.type = EVENT_TYPE_PRESS;
		de.press.code = e->code;
		break;

	case EVENT_RELEASE:
		de.type = EVENT_TYPE_RELEASE;
		de.release.code = e->code;
		break;

	case EVENT_KEY_REPEAT:
		de.type = EVENT_TYPE_KEYREPEAT;
		de.keyrepeat.code = e->code;
		break;

	default:
		return;
	}
	callback(&de, arg);
}


static void send_action_event(s32 app_id, char const *action, char const *bindarg)
{
	void (*callback) (Event_union *, void *);
	void *arg;
	Event_union de;

	callback = (void (*)(Event_union *,void*))hex2u32(bindarg);
	arg = (void *)hex2u32(bindarg+10);

	de.type = EVENT_TYPE_COMMAND;
	de.command.cmd = action;
	callback(&de, arg);
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct messenger_services services = {
	send_input_event,
	send_action_event,
};


/************************
 ** Module entry point **
 ************************/

int init_messenger(struct dope_services *d)
{
	d->register_module("Messenger 1.0",&services);
	return 1;
}
