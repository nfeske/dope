/*
 * \brief   DOpE dummies
 * \date    2004-04-07
 * \author  Norman Feske
 */

/*
 * Copyright (C) 2002-2007 Norman Feske
 * Copyright (C) 2008-2014 Genode Labs GmbH
 *
 * This file is part of the DOpE package, which is distributed under
 * the terms of the GNU General Public Licence 2.
 */

#include "dopestd.h"

int config_oldresize    = 0;  /* use traditional way to resize windows */
int config_adapt_redraw = 0;  /* do not adapt redraw to duration time  */

void init_vscr_server(struct dope_services *d);
void init_vscr_server(struct dope_services *d)
{
	d->register_module("VScreenServer 1.0", NULL);
}

