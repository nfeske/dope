/*
 * \brief   Interface of DOpE Scale widget module
 * \date    2003-06-11
 * \author  Norman Feske
 */

/*
 * Copyright (C) 2002-2007 Norman Feske
 * Copyright (C) 2008-2014 Genode Labs GmbH
 *
 * This file is part of the DOpE package, which is distributed under
 * the terms of the GNU General Public Licence 2.
 */

#ifndef _DOPE_SCALE_H_
#define _DOPE_SCALE_H_

#include "variable.h"

struct scale_methods;
struct scale_data;

#define SCALE struct scale

enum { SCALE_VER = 0x1 };

struct scale {
	struct widget_methods *gen;
	struct scale_methods  *scale;
	struct widget_data    *wd;
	struct scale_data     *sd;
};

struct scale_services {
	SCALE *(*create) (void);
};


#endif /* _DOPE_SCALE_H_ */
