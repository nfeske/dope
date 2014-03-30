/*
 * \brief   Interface of input event layer of DOpE
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

#ifndef _DOPE_INPUT_H_
#define _DOPE_INPUT_H_

#include "widget.h"
#include "event.h"

struct input_services {
	int     (*get_event)    (EVENT *e);
};


#endif /* _DOPE_INPUT_H_ */
