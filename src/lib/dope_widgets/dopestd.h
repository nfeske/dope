/*
 * \brief   Standard types and functions used by DOpE
 * \date    2003-08-01
 * \author  Norman Feske
 */

/*
 * Copyright (C) 2002-2007 Norman Feske
 * Copyright (C) 2008-2014 Genode Labs GmbH
 *
 * This file is part of the DOpE package, which is distributed under
 * the terms of the GNU General Public Licence 2.
 */

#ifndef _DOPE_DOPESTD_H_
#define _DOPE_DOPESTD_H_

#define SHOW_INFOS     0
#define SHOW_WARNINGS  1
#define SHOW_ERRORS    1

#include <base/printf.h>
#include <base/snprintf.h>
#include <util/string.h>


/************************
 ** Types used by dope **
 ************************/

typedef Genode::int8_t    s8;
typedef Genode::uint8_t   u8;
typedef Genode::int16_t   s16;
typedef Genode::uint16_t  u16;
typedef Genode::int32_t   s32;
typedef Genode::uint32_t  u32;
typedef Genode::addr_t    adr;
typedef Genode::size_t    size_t;

#if !defined(NULL)
#define NULL 0
#endif


/*************************************
 ** Standard functions used by dope **
 *************************************/

void   *malloc(size_t size);
void   *zalloc(unsigned long size);
void    free(void const *addr);
long    atol(const char *nptr);
double  atof(const char *nptr);

using Genode::strlen;
using Genode::printf;
using Genode::snprintf;
using Genode::memset;
using Genode::memcpy;
using Genode::strcmp;


/******************************
 ** Implemented in dopestd.c **
 ******************************/

/**
 * Convert a float into a string
 *
 * This function performs zero-termination of the string.
 *
 * \param v       float value to convert
 * \param prec    number of digits after comma
 * \param dst     destination buffer
 * \param max_len destination buffer size
 */
extern int ftoa(float v, int prec, char *dst, int max_len);


/**
 * Check if two strings are equal
 *
 * This function compares two strings s1 and s2. The length of string s1 can
 * be defined via max_s1.  If max_s1 characters are identical and strlen(s2)
 * equals max_s1, the two strings are considered to be equal. This way, s2
 * can be compared against a substring without null-termination.
 *
 * \param s1      string (length is bounded by max_s1)
 * \param s2      null-terminated second string
 * \param max_s1  max length of string s1
 * \return        1 if the two strings are equal.
 */
extern int streq(const char *s1, const char *s2, int max_len);


/**
 * Duplicate string
 */
extern char const *strdup(char const *s);


/*******************************
 ** Debug macros used in dope **
 *******************************/

/*
 * Within the code of DOpE the following macros for filtering
 * debug output are used.
 *
 * INFO    - for presenting status information
 * WARNING - for informing the user about non-serious problems
 * ERROR   - for reporting really worse stuff
 */

#if SHOW_WARNINGS
	#define WARNING(x) x
#else
	#define WARNING(x) /* x */
#endif

#undef INFO
#if SHOW_INFOS
	#define INFO(x) x
#else
	#define INFO(x) /* x */
#endif

#undef ERROR
#if SHOW_ERRORS
	#define ERROR(x) x
#else
	#define ERROR(x) /* x */
#endif


#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif


/****************************
 ** DOpE service structure **
 ****************************/

/*
 * DOpE provides the following service structure to all
 * its components. Via this structure components can
 * access functionality of other components or make an
 * interface available to other components.
 */

struct dope_services {
	void *(*get_module)      (char const *name);
	long  (*register_module) (char const *name, void *services);
};

#endif /* _DOPE_DOPESTD_H_ */
