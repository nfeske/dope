/*
 * \brief   Interface of the cache module of DOpE
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

#ifndef _DOPE_CACHE_H_
#define _DOPE_CACHE_H_

#define CACHE struct cache
struct cache;

struct cache_services {
	CACHE *(*create)      (s32 max_entries,s32 max_size);
	void   (*destroy)     (CACHE *c);
	s32    (*add_elem)    (CACHE *c,void *elem,s32 elemsize,s32 ident,void (*destroy)(void *));
	void  *(*get_elem)    (CACHE *c,s32 index,s32 ident);
	void   (*remove_elem) (CACHE *c,s32 index);
};


#endif /* _DOPE_CACHE_H_ */
