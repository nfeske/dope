/*
 * \brief   Interface of keymap module
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

#ifndef _DOPE_KEYMAP_H_
#define _DOPE_KEYMAP_H_

enum {
	KEYMAP_SWITCH_LSHIFT   = 0x01,
	KEYMAP_SWITCH_RSHIFT   = 0x02,
	KEYMAP_SWITCH_LCONTROL = 0x04,
	KEYMAP_SWITCH_RCONTROL = 0x08,
	KEYMAP_SWITCH_ALT      = 0x10,
	KEYMAP_SWITCH_ALTGR    = 0x11,
};


struct keymap_services {
	char    (*get_ascii) (long keycode,long switches);
};

#endif /* _DOPE_KEYMAP_H_ */
