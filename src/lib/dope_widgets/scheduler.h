/*
 * \brief   Interface of the real-time scheduler of DOpE
 * \date    2004-04-27
 * \author  Norman Feske
 */

/*
 * Copyright (C) 2002-2007 Norman Feske
 * Copyright (C) 2008-2014 Genode Labs GmbH
 *
 * This file is part of the DOpE package, which is distributed under
 * the terms of the GNU General Public Licence 2.
 */

#ifndef _DOPE_SCHEDULER_H_
#define _DOPE_SCHEDULER_H_

#include "widget.h"

struct scheduler_services {

	/**
	 * Schedule new real-time widget
	 *
	 * \param w       widget to redraw periodically
	 * \param period  period length in milliseconds
	 * \return        0 on success
	 */
	s32  (*add) (WIDGET *w, u32 period);


	/**
	 * Stop real-time redraw of specified widget
	 */
	void (*remove) (WIDGET *w);


	/**
	 * Mainloop of dope
	 *
	 * Within the mainloop we must update real-time widgets,
	 * handle non-realtime redraw operations and pay some
	 * attention to the user.
	 */
	void (*process_mainloop) (void);
};


#endif /* _DOPE_SCHEDULER_H_ */
