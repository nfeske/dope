/*
 * \brief   Error values that are exported by DOpE
 * \date    2004-08-06
 * \author  Norman Feske
 */

/*
 * Copyright (C) 2002-2007 Norman Feske
 * Copyright (C) 2008-2014 Genode Labs GmbH
 *
 * This file is part of the DOpE package, which is distributed under
 * the terms of the GNU General Public Licence 2.
 */

#ifndef _INCLUDE__DOPE__DOPEDEF_H_
#define _INCLUDE__DOPE__DOPEDEF_H_


enum {
	DOPE_ERR_PERM              =  -1,  /* permission denied          */
	DOPE_ERR_NOT_PRESENT       =  -2,  /* no DOpE server to speak to */
	
	DOPECMD_ERR_UNKNOWN_VAR    = -11,  /* variable does not exist               */
	DOPECMD_ERR_INVALID_VAR    = -12,  /* variable became invalid               */
	DOPECMD_ERR_INVALID_TYPE   = -13,  /* variable type does not exist          */
	DOPECMD_ERR_ATTR_W_PERM    = -14,  /* attribute cannot be written           */
	DOPECMD_ERR_ATTR_R_PERM    = -15,  /* attribute cannot be requested         */
	DOPECMD_ERR_NO_SUCH_MEMBER = -16,  /* method or attribute does not exist    */
	DOPECMD_ERR_NO_TAG         = -17,  /* expected tag but found something else */
	DOPECMD_ERR_UNKNOWN_TAG    = -18,  /* tag does not exist                    */
	DOPECMD_ERR_INVALID_ARG    = -19,  /* illegal argument value                */
	DOPECMD_ERR_LEFT_PAR       = -20,  /* missing left parenthesis              */
	DOPECMD_ERR_RIGHT_PAR      = -21,  /* missing right parenthesis             */
	DOPECMD_ERR_TOO_MANY_ARGS  = -22,  /* too many method arguments             */
	DOPECMD_ERR_MISSING_ARG    = -23,  /* mandatory argument is missing         */
	DOPECMD_ERR_UNCOMPLETE     = -24,  /* unexpected end of command             */
	DOPECMD_ERR_NO_SUCH_SCOPE  = -25,  /* variable scope could not be resolved  */
	DOPECMD_ERR_ILLEGAL_CMD    = -26,  /* command could not be examined         */
	
	DOPECMD_WARN_TRUNC_RET_STR =  11,  /* return string was truncated */
};

#endif /* _INCLUDE__DOPE__DOPEDEF_H_ */
