/*
 * \brief   DOpE font manager module
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

#ifndef _DOPE_FONTMAN_H_
#define _DOPE_FONTMAN_H_

struct font {
	s32         font_id;
	s32        *width_table;
	s32        *offset_table;
	s32         img_w,img_h;
	s16         top,bottom;
	u8         *image;
	char const *name;
};


struct fontman_services {
	struct font *(*get_by_id)       (s32 font_id);
	s32          (*calc_str_width)  (s32 font_id, char const *str);
	s32          (*calc_str_height) (s32 font_id, char const *str);
	s32          (*calc_char_idx)   (s32 font_id, char const *str, s32 pixpos);
};


#endif /* _DOPE_FONTMAN_H_ */
