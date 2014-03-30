/*
 * \brief   Interface of clipping stack handling module
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

#ifndef _DOPE_CLIPPING_H_
#define _DOPE_CLIPPING_H_

struct clipping_services {
	void     (*push)        (long x1, long y1, long x2, long y2);
	void     (*pop)         (void);
	void     (*reset)       (void);
	void     (*set_range)   (long x1, long y1, long x2, long y2);
	long     (*get_x1)      (void);
	long     (*get_y1)      (void);
	long     (*get_x2)      (void);
	long     (*get_y2)      (void);
};


#endif /* _DOPE_CLIPPING_H_ */
