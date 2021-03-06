/*
 * \brief   Interface of DOpE Scrollbar widget module
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

#ifndef _DOPE_SCROLLBAR_H_
#define _DOPE_SCROLLBAR_H_

#include "widget.h"

struct scrollbar_methods;
struct scrollbar_data;

#define SCROLLBAR struct scrollbar

struct scrollbar {
	struct widget_methods    *gen;
	struct scrollbar_methods *scroll;
	struct widget_data       *wd;
	struct scrollbar_data    *sd;
};

struct scrollbar_methods {
	void        (*set_orient)        (SCROLLBAR *, char const *orient);
	char const *(*get_orient)        (SCROLLBAR *);
	void        (*set_autoview)      (SCROLLBAR *, long av);
	long        (*get_autoview)      (SCROLLBAR *);
	void        (*set_slider_x)      (SCROLLBAR *, s32 new_sx);
	u32         (*get_slider_x)      (SCROLLBAR *);
	void        (*set_slider_y)      (SCROLLBAR *, s32 new_sy);
	u32         (*get_slider_y)      (SCROLLBAR *);
	void        (*set_real_size)     (SCROLLBAR *, u32 new_real_size);
	u32         (*get_real_size)     (SCROLLBAR *);
	void        (*set_view_size)     (SCROLLBAR *, u32 new_view_size);
	u32         (*get_view_size)     (SCROLLBAR *);
	void        (*set_view_offset)   (SCROLLBAR *, s32 new_view_offset);
	u32         (*get_view_offset)   (SCROLLBAR *);
	s32         (*get_arrow_size)    (SCROLLBAR *);
	void        (*reg_scroll_update) (SCROLLBAR *, void (*callback)(void *), void *arg);
};

struct scrollbar_services {
	SCROLLBAR *(*create) (void);
};


#endif /* _DOPE_SCROLLBAR_H_ */
