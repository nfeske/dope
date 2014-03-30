/*
 * \brief   DOpE pool module
 * \date    2002-11-13
 * \author  Norman Feske
 *
 * Pool is the component provider of DOpE.  Each
 * component can register at  Pool by specifying
 * an identifier string and a pointer to the its
 * service structure.
 * After that, the component's service structure
 * can be requested  by other components via the
 * associated identifier.
 */

/*
 * Copyright (C) 2002-2007 Norman Feske
 * Copyright (C) 2008-2014 Genode Labs GmbH
 *
 * This file is part of the DOpE package, which is distributed under
 * the terms of the GNU General Public Licence 2.
 */

#include "dopestd.h"

enum { MAX_POOL_ENTRIES = 100 };

struct pool_entry {
	char const *name;          /* id of system module */
	void       *structure;     /* system module structure */
};

static struct pool_entry pool[MAX_POOL_ENTRIES];
static long pool_size=0;


/**
 * Add new pool entry
 */
long pool_add(char const *name, void *structure)
{
	long i;
	if (pool_size>=100) return 0;
	else {
		for (i=0;pool[i].name!=NULL;i++) {};

		pool[i].name=name;
		pool[i].structure=structure;

		pool_size++;
		INFO(printf("Pool(add): %s\n",name));
		return 1;
	}
}


/**
 * Remove poolentry from pool
 */
void pool_remove(char const *name)
{
	for (int i = 0; i < 100; i++) {
		char const *s = pool[i].name;
		if (s!=NULL) {
			if (streq(name,pool[i].name,255)) {
				pool[i].name=NULL;
				pool[i].structure=NULL;
				pool_size--;
				return;
			}
		}
	}
}


/**
 * Get structure of a specified pool entry
 */
void *pool_get(char const *name)
{
	for (int i = 0; i < MAX_POOL_ENTRIES; i++) {
		char const *s = pool[i].name;
		if (s!=NULL) {
			if (streq(name,pool[i].name,255)) {
				INFO(printf("Pool(get): module matched: %s\n",name));
			    return pool[i].structure;
			}
		}
	}
	ERROR(printf("Pool(get): module not found: %s\n",name));
	return NULL;
}
