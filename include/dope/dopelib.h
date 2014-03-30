/*
 * \brief   Interface of DOpE client library
 * \date    2002-11-13
 * \author  Norman Feske <norman.feske@genode-labs.com>
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _INCLUDE__DOPE__DOPELIB_H_
#define _INCLUDE__DOPE__DOPELIB_H_

enum Event_type {
	EVENT_TYPE_UNDEFINED = 0,
	EVENT_TYPE_COMMAND   = 1,
	EVENT_TYPE_MOTION    = 2,
	EVENT_TYPE_PRESS     = 3,
	EVENT_TYPE_RELEASE   = 4,
	EVENT_TYPE_KEYREPEAT = 5,
};


struct Command_event
{
	Event_type  type;               /* must be EVENT_TYPE_COMMAND */
	char const *cmd;                /* command string */
};


struct Motion_event
{
	Event_type type;                /* must be EVENT_TYPE_MOTION */
	long rel_x,rel_y;               /* relative movement in x and y direction */
	long abs_x,abs_y;               /* current position inside the widget */
};


struct Press_event
{
	Event_type type;                /* must be EVENT_TYPE_PRESS */
	long code;                      /* code of key/button that is pressed */
};


struct Release_event
{
	Event_type type;                /* must be EVENT_TYPE_RELEASE */
	long code;                      /* code of key/button that is released */
};


struct Keyrepeat_event
{
	Event_type type;                /* must be EVENT_TYPE_KEYREPEAT */
	long code;                      /* code of key/button that is pressed */
};


union Event_union
{
	Event_type type;
	Command_event   command;
	Motion_event    motion;
	Press_event     press;
	Release_event   release;
	Keyrepeat_event keyrepeat;
};


/**
 * Initialise dope library
 */
long dope_init(void);


/**
 * Deinitialise dope library
 */
void dope_deinit(void);


/**
 * Register dope client application
 *
 * \param appname  name of the DOpE application
 * \return         DOpE application id
 */
long dope_init_app(char const *appname);


/**
 * Unregister dope client application
 *
 * \param app_id  DOpE application to unregister
 * \return        0 on success
 */
long dope_deinit_app(long app_id);


/**
 * Execute dope command
 *
 * \param app_id  DOpE application id
 * \param cmd     command to execute
 * \return        0 on success
 */
int dope_cmd(long app_id, char const *cmd);


/**
 * Execute dope format string command
 *
 * \param app_id  DOpE application id
 * \param cmdf    command to execute specified as format string
 * \return        0 on success
 */
int dope_cmdf(long app_id, char const *cmdf, ...);


/**
 * Execute sequence of DOpE commands
 *
 * \param app_id  DOpE application id
 * \param ...     command sequence
 * \return        0 on success
 *
 * The sequence is a series of NULL-terminated strings. Its end must be
 * marked by a NULL pointer.
 */
int dope_cmd_seq(int app_id, ...);


/**
 * Execute dope command and request result
 *
 * \param app_id    DOpE application id
 * \param dst       destination buffer for storing the result string
 * \param dst_size  size of destination buffer in bytes
 * \param cmd       command to execute
 * \return          0 on success
 */
int dope_req(long app_id, char *dst, int dst_size, char const *cmd);


/**
 * Request result of a dope command specified as format string
 *
 * \param app_id    DOpE application id
 * \param dst       destination buffer for storing the result string
 * \param dst_size  size of destination buffer in bytes
 * \param cmd       command to execute - specified as format string
 * \return          0 on success
 */
int dope_reqf(long app_id, char *dst, int dst_size, char const *cmdf, ...);


/**
 * Bind an event to a dope widget
 *
 * \param app_id      DOpE application id
 * \param var         widget to bind an event to
 * \param event_type  identifier for the event type
 * \param callback    callback function to be called for incoming events
 * \param arg         additional argument for the callback function
 */
void dope_bind(long app_id, char const *var, char const *event_type,
               void (*callback)(Event_union *,void *),void *arg);


/**
 * Bind an event to a dope widget specified as format string
 *
 * \param app_id      DOpE application id
 * \param varfmt      widget to bind an event to (format string)
 * \param event_type  identifier for the event type
 * \param callback    callback function to be called for incoming events
 * \param arg         additional argument for the callback function
 * \param ...         format string arguments
 */
void dope_bindf(long id, char const *varfmt, char const *event_type,
                void (*callback)(Event_union *,void *), void *arg,...);


/**
 * Enter dope eventloop
 *
 * \param app_id  DOpE application id
 */
void dope_eventloop(long app_id);


/**
 * Return number of pending events
 *
 * \param app_id  DOpE application id
 * \return        number of pending events
 */
int dope_events_pending(int app_id);


/**
 * Process one single dope event
 *
 * This function processes exactly one DOpE event. If no event is pending, it
 * blocks until an event is available. Thus, for non-blocking operation, this
 * function should be called only if dope_events_pending was consulted before.
 *
 * \param app_id  DOpE application id
 */
void dope_process_event(long app_id);


/**
 * Request key or button state
 *
 * \param app_id   DOpE application id
 * \param keycode  keycode of the requested key
 * \return         1 if key is currently pressed
 */
long dope_get_keystate(long app_id, long keycode);


/**
 * Request current ascii keyboard state
 *
 * \param app_id   DOpE application id
 * \param keycode  keycode of the requested key
 * \return         ASCII value of the currently pressed key combination
 */
char dope_get_ascii(long app_id, long keycode);


#endif /* _INCLUDE__DOPE__DOPELIB_H_ */
