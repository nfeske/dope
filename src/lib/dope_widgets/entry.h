/*
 * \brief   Interface of DOpE Entry widget module
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

#ifndef _DOPE_ENTRY_H_
#define _DOPE_ENTRY_H_

struct entry_methods;
struct entry_data;

#define ENTRY struct entry

struct entry {
	struct widget_methods *gen;
	struct entry_methods  *but;
	struct widget_data    *wd;
	struct entry_data     *ed;
};

struct entry_methods {
	void    (*set_text)    (ENTRY *, char *new_txt);
	char   *(*get_text)    (ENTRY *);
};

struct entry_services {
	ENTRY *(*create) (void);
};


#endif /* _DOPE_ENTRY_H_ */
