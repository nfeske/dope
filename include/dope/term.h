/*
 * \brief   Interface of DOpE terminal library
 * \date    2003-03-06
 * \author  Norman Feske
 */

/*
 * Copyright (C) 2002-2007 Norman Feske
 * Copyright (C) 2008-2014 Genode Labs GmbH
 *
 * This file is part of the DOpE package, which is distributed under
 * the terms of the GNU General Public Licence 2.
 */

#ifndef _INCLUDE__DOPE__TERM_H_
#define _INCLUDE__DOPE__TERM_H_

#include <stdarg.h>

extern int  term_init    (char *terminal_name);
extern void term_deinit  (void);
extern int  term_printf  (const char *format, ...);
extern int  term_vprintf (const char *format, va_list ap);
extern int  term_getchar (void);

struct history;

extern struct history *term_history_create(void *buf, long buf_size);
extern int   term_history_add(struct history *hist, char *new_str);
extern char *term_history_get(struct history *hist, int index);

extern int term_readline(char *dst, int max_len, struct history *hist);

#endif /* _INCLUDE__DOPE__TERM_H_ */
