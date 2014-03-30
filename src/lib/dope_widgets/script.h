/*
 * \brief   Interface of the DOpE command interpreter
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

#ifndef _DOPE_SCRIPT_H_
#define _DOPE_SCRIPT_H_

struct widtype;
struct script_services
{
	struct widtype *(*reg_widget_type) (char const *widtype_name, void *(*create_func)(void));

	void  (*reg_widget_method) (struct widtype *, char const *desc, void *methadr);
	void  (*reg_widget_attrib) (struct widtype *, char const *desc, void *get, void *set, void *update);
	int   (*exec_command)      (u32 app_id, char const *cmd, char *dst, int dst_len);
};


#endif /* _DOPE_SCRIPT_H_ */
