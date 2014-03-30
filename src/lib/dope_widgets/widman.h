/*
 * \brief   Interface of the widget manager module of DOpE
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

#ifndef _DOPE_WIDMAN_H_
#define _DOPE_WIDMAN_H_

#include "widget.h"

struct widtype;

struct widman_services {
	void (*default_widget_data)    (struct widget_data *);
	void (*default_widget_methods) (struct widget_methods *);
	void (*build_script_lang)      (widtype *widtype, struct widget_methods *);
};

#endif /* _DOPE_WIDMAN_H_ */
