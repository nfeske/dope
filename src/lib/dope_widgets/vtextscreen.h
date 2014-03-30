/*
 * \brief   Interface of DOpE VTextScreen widget module
 * \date    2004-03-02
 * \author  Norman Feske
 */

/*
 * Copyright (C) 2002-2007 Norman Feske
 * Copyright (C) 2008-2014 Genode Labs GmbH
 *
 * This file is part of the DOpE package, which is distributed under
 * the terms of the GNU General Public Licence 2.
 */

#ifndef _DOPE_VTEXTSCREEN_H_
#define _DOPE_VTEXTSCREEN_H_

#include "widget.h"

struct vtextscreen_methods;
struct vtextscreen_data;

#define VTEXTSCREEN struct vtextscreen

#include "gfx.h"

struct vtextscreen {
	struct widget_methods      *gen;
	struct vtextscreen_methods *vts;
	struct widget_data         *wd;
	struct vtextscreen_data    *vd;
};

struct vtextscreen_methods {
	s32         (*probe_mode)   (VTEXTSCREEN *, s32 width, s32 height, char *mode);
	s32         (*set_mode)     (VTEXTSCREEN *, s32 width, s32 height, char *mode);
	char const *(*map)          (VTEXTSCREEN *, char *dst_thread_ident);
	void        (*refresh)      (VTEXTSCREEN *, s32 x, s32 y, s32 w, s32 h);
	GFX_CONTAINER *(*get_image) (VTEXTSCREEN *);
};

struct vtextscreen_services {
	VTEXTSCREEN *(*create) (void);
};

#endif /* _DOPE_VTEXTSCREEN_H_ */
