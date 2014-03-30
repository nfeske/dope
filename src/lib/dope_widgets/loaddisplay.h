/*
 * \brief   Interface of DOpE Load Display widget module
 * \date    2004-01-09
 * \author  Norman Feske
 */

/*
 * Copyright (C) 2002-2007 Norman Feske
 * Copyright (C) 2008-2014 Genode Labs GmbH
 *
 * This file is part of the DOpE package, which is distributed under
 * the terms of the GNU General Public Licence 2.
 */

#ifndef _DOPE_LOADDISPLAY_H_
#define _DOPE_LOADDISPLAY_H_

#include "widget.h"

struct loaddisplay_methods;
struct loaddisplay_data;

#define LOADDISPLAY struct loaddisplay

enum {
	LOADDISPLAY_VERTICAL = 0x1, /* vertical loaddisplay */
	LOADDISPLAY_FIT      = 0x2, /* fit bars in loaddisplay */
	LOADDISPLAY_ABS      = 0x4, /* use absolute values */
};

struct loaddisplay {
	struct widget_methods      *gen;
	struct loaddisplay_methods *ldm;
	struct widget_data         *wd;
	struct loaddisplay_data    *ldd;
};

struct loaddisplay_methods {
	void (*set_orient) (LOADDISPLAY *ld, char const *orientation);
	void (*set_from)   (LOADDISPLAY *ld, float from);
	void (*set_to)     (LOADDISPLAY *ld, float to);
	void (*barconfig)  (LOADDISPLAY *ld, char const *bar_ident,
	                                     char const *value, char const *colname);
};

struct loaddisplay_services {
	LOADDISPLAY *(*create) (void);
};


#endif /* _DOPE_LOADDISPLAY_H_ */
