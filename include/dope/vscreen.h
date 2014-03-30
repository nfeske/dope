/*
 * \brief   Interface of VScreen library
 * \date    2002-11-25
 * \author  Norman Feske
 */

/*
 * Copyright (C) 2002-2007 Norman Feske
 * Copyright (C) 2008-2014 Genode Labs GmbH
 *
 * This file is part of the DOpE package, which is distributed under
 * the terms of the GNU General Public Licence 2.
 */

#ifndef _INCLUDE__DOPE__VSCREEN_H_
#define _INCLUDE__DOPE__VSCREEN_H_


/******************************************
 * Handle shared memory buffer of vscreen *
 ******************************************/

/**
 * Return local address of specified shared memory block
 *
 * \param smb_ident  identifier string of the shared memory block
 * \return           address of the smb block in local address space
 *
 * The format of the smb_ident string is platform dependent. The
 * result of the VScreen widget's map function can directly be passed
 * into this function. (as done by vscr_get_fb)
 */
extern void *vscr_map_smb(char *smb_ident);


/**
 * Return local address of the framebuffer of the specified vscreen
 *
 * \param app_id     DOpE application id to which the VScreen widget belongs
 * \param vscr_name  name of the VScreen widget
 * \return           address of the VScreen buffer in the local address space
 */
extern void *vscr_get_fb(int app_id, const char *vscr_name);


/**
 * Release vscreen buffer from local address space
 *
 * \param fb_adr   start address of the vscreen buffer
 * \return         0 on success
 */
extern int vscr_free_fb(void *fb_adr);

#endif /* _INCLUDE__DOPE__VSCREEN_H_ */
