/*
 * \brief   Interface of tick module
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

#ifndef _DOPE_TICK_H_
#define _DOPE_TICK_H_

struct tick_services {
	int   (*add)    (s32 msec, int (*callback)(void *), void *arg);
	void  (*handle) (void);
};

#endif /* _DOPE_TICK_H_ */
