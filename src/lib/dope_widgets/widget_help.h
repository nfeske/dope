/*
 * \brief   Widget type default includes
 * \date    2004-02-01
 * \author  Norman Feske
 */

/*
 * Copyright (C) 2002-2007 Norman Feske
 * Copyright (C) 2008-2014 Genode Labs GmbH
 *
 * This file is part of the DOpE package, which is distributed under
 * the terms of the GNU General Public Licence 2.
 */

#ifndef _DOPE_WIDGET_HELP_H_
#define _DOPE_WIDGET_HELP_H_

#include "widget.h"


/**
 * Allocate widget structure of specified type
 */
#define ALLOC_WIDGET(widtype)                          \
	(widtype *)zalloc(sizeof(widtype)                   \
	                + sizeof(struct widget_data)         \
	                + sizeof(widtype ## _data));


/**
 * Initialize widget structure
 *
 * \param wid           name of widget struct to initialize
 * \param widtype       type of widget struct
 * \param spec_methods  widget specific function table
 */
#define SET_WIDGET_DEFAULTS(wid, widtype, spec_methods) {                         \
	if (!wid) {                                                                    \
		ERROR(printf("Error: out of memory. Widget allocation failed.\n"));         \
		return NULL;                                                                 \
	}                                                                                 \
	((struct widget *)wid)->gen  = &gen_methods;                                       \
	((struct widget *)wid)->spec = spec_methods;                                        \
	((struct widget *)wid)->wd   = (struct widget_data *)((adr)wid + sizeof(widtype));   \
	((struct widget *)wid)->sd   = (widtype  ##  _data *)((adr)wid->wd                    \
	                                                   + sizeof(struct widget_data));      \
	widman->default_widget_data(wid->wd);                                                   \
}

#endif /* _DOPE_WIDGET_HELP_H_ */
