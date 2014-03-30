/*
 * \brief   Interface of tokenizer module
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

#ifndef _DOPE_TOKENIZER_H_
#define _DOPE_TOKENIZER_H_

enum {
	TOKEN_EMPTY  =  0,
	TOKEN_IDENT  =  1,      /* identifier */
	TOKEN_STRUCT =  2,      /* structure element (brackets etc.) */
	TOKEN_STRING =  3,      /* string */
	TOKEN_WEIRD  =  4,      /* weird */
	TOKEN_NUMBER =  5,      /* number */
	TOKEN_EOS    = 99,      /* end of string */
};

struct tokenizer_services {
	int (*parse)  (const char *str, u32 max_tok, u32 *offbuf, u32 *lenbuf);
	int (*toktype)(const char *str, u32 offset);
};

#endif /* _DOPE_TOKENIZER_H_ */
