/*
 * \brief  Interface of color window
 * \author Norman Feske
 * \date   2009-03-09
 */

/*
 * Copyright (C) 2009-2014 Genode Labs GmbH
 *
 * This file is part of the DOpE package, which is distributed under
 * the terms of the GNU General Public Licence 2.
 */

#ifndef _COLORS_H_
#define _COLORS_H_

/**
 * Display 4096 Colors
 *
 * \return  DOpE application ID used for the color window
 */
extern int init_colors_window(void);

/**
 * Bring color window to front
 *
 * \param app_id  DOpE application ID as returned by 'init_colors_window'
 */
extern void open_colors_window(int app_id);

#endif /* _COLORS_H_ */
