/*
 * \brief   General widget data structures
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

#ifndef _DOPE_WIDGET_DATA_H_
#define _DOPE_WIDGET_DATA_H_

/**
 * Widget property and state flags
 */
enum {
	WID_FLAGS_STATE      = 0x0001,  /* widget is selected                  */
	WID_FLAGS_MFOCUS     = 0x0002,  /* widget has mouse focus              */
	WID_FLAGS_KFOCUS     = 0x0004,  /* widget has keyboard focus           */
	WID_FLAGS_EVFORWARD  = 0x0008,  /* pass unhandled events to the parent */
	WID_FLAGS_HIGHLIGHT  = 0x0010,  /* widget reacts on mouse focus        */
	WID_FLAGS_EDITABLE   = 0x0020,  /* widget interprets key strokes       */
	WID_FLAGS_CONCEALING = 0x0040,  /* widget covers its area completely   */
	WID_FLAGS_SELECTABLE = 0x0080,  /* widget is selectable via keyboard   */
	WID_FLAGS_TAKEFOCUS  = 0x0100,  /* widget can receive keyboard focus   */
	WID_FLAGS_GRABFOCUS  = 0x0200,  /* prevent keyboard focus to switch    */
};

/**
 * Widget update flags
 */
enum {
	WID_UPDATE_SIZE     = 0x0001,
	WID_UPDATE_MINMAX   = 0x0002,
	WID_UPDATE_NEWCHILD = 0x0004,
	WID_UPDATE_REFRESH  = 0x0008,
};

struct binding;
struct binding {
	s16             ev_type;     /* event type          */
	char     const *bind_ident;  /* action event string */
	char     const *msg;         /* associated message  */
	struct binding *next;        /* next binding        */
};


struct new_binding;
struct new_binding {
	int         app_id;       /* application to receive the event */
	char const *name;         /* name of binding                  */
	char      **args;         /* arguments to pass with the event */
	char      **cond_names;   /* conditions to be checked         */
	char      **cond_values;  /* desired values of the conditions */
	struct new_binding *next;
};


struct widget_data {
	long    x, y, w, h;         /* current relative position and size  */
	long    min_w, min_h;       /* minimal size                        */
	long    max_w, max_h;       /* maximal size                        */
	long    flags;              /* state flags                         */
	long    update;             /* update flags                        */
	void    *context;           /* associated data                     */
	WIDGET  *parent;            /* parent in widget hierarchy          */
	WIDGET  *next;              /* next widget in a connected list     */
	WIDGET  *prev;              /* previous widget in a connected list */
	void    (*click) (void *);  /* event handle routine                */
	long    ref_cnt;            /* reference counter                   */
	s32     app_id;             /* application that owns the widget    */
	struct binding *bindings;   /* event bindings                      */
	struct new_binding *new_bindings;
};


#endif /* _DOPE_WIDGET_DATA_H_ */
