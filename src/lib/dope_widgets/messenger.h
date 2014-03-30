/*
 * \brief   Interface of DOpE messenger module
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

#ifndef _DOPE_MESSENGER_H_
#define _DOPE_MESSENGER_H_

#include "event.h"

struct messenger_services {
	void (*send_input_event) (s32 app_id, EVENT *e, char const *bindarg);
	void (*send_action_event)(s32 app_id, char const *action, char const *bindarg);
};

#endif /* _DOPE_MESSENGER_H_ */
