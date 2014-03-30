/*
 * \brief   Interface of DOpE font conversion module
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

#ifndef _DOPE_FONTCONV_H_
#define _DOPE_FONTCONV_H_

struct fontconv_services {
	s16         (*probe)            (void *fontadr);
	void        (*gen_width_table)  (void *fontadr, s32 *dst_wtab);
	void        (*gen_offset_table) (void *fontadr, s32 *dst_otab);
	char const *(*get_name)         (void *fontadr);
	u16         (*get_top)          (void *fontadr);
	u16         (*get_bottom)       (void *fontadr);
	u32         (*get_image_width)  (void *fontadr);
	u32         (*get_image_height) (void *fontadr);
	void        (*gen_image)        (void *fontadr, u8 *dst_img);
};


#endif /* _DOPE_FONTCONV_H_ */
