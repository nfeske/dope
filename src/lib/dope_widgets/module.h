/*
 * \brief   DOpE module structure
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

#ifndef _DOPE_MODULE_H_
#define _DOPE_MODULE_H_

struct module_info {
	char    *type;      /* kind of module */
	char    *name;      /* name and version */
	char    *conflict;  /* lowest version to which the module is compatibe with */
	char    *uses;      /* required modules */
};

struct module {
	struct module_info  *info;
	int                (*init)   (void);
	int                (*deinit) (void);
	void                *services;
};


#endif /* _DOPE_MODULE_H_ */
