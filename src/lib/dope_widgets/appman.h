/*
 * \brief   Interface of DOpE application manager module
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

#ifndef _DOPE_APPMAN_H_
#define _DOPE_APPMAN_H_

#include "hashtab.h"
#include "scope.h"

struct appman_services {
	s32         (*reg_app)          (char const *app_name);
	s32         (*unreg_app)        (u32 app_id);
	void        (*set_rootscope)    (u32 app_id, SCOPE *rootscope);
	SCOPE      *(*get_rootscope)    (u32 app_id);
	void        (*reg_listener)     (s32 app_id, void *listener);
	void       *(*get_listener)     (s32 app_id);
	char const *(*get_app_name)     (s32 app_id);
	s32         (*app_id_of_thread) (void *app_thread);
	s32         (*app_id_of_name)   (char const *app_name);
	void        (*lock)             (s32 app_id);
	void        (*unlock)           (s32 app_id);
};


#endif /* _DOPE_APPMAN_H_ */
