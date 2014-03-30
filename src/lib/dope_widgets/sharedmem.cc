/*
 * \brief   DOpE shared memory management module
 * \date    2002-02-04
 * \author  Norman Feske
 *
 * This implementation is meant be used if DOpE and its client
 * reside in the same address space. Here, shared memory is
 * just allocated via malloc and the textual identifier contains
 * the start address.
 */

/*
 * Copyright (C) 2002-2007 Norman Feske
 * Copyright (C) 2008-2014 Genode Labs GmbH
 *
 * This file is part of the DOpE package, which is distributed under
 * the terms of the GNU General Public Licence 2.
 */

#include "dopestd.h"
#include "module.h"
#include "sharedmem.h"


struct shared_memory {
	int   fh;
	s32   size;
	char  fname[64];
	void *addr;
};

int init_sharedmem(struct dope_services *d);


/***********************
 ** Service functions **
 ***********************/

/**
 * Allocate shared memory block of specified size
 */
static SHAREDMEM *shm_alloc(size_t size)
{
	SHAREDMEM *shm = (SHAREDMEM *)(malloc(sizeof(SHAREDMEM)));
	if (!shm) {
		ERROR(printf("SharedMemory(alloc): out of memory.\n"));
		return NULL;
	}
	shm->size = size;
	shm->addr = malloc(size);
	return shm;
}


/**
 * Free shared memory block
 */
static void shm_destroy(SHAREDMEM *sm)
{
	if (!sm) return;
	if (sm->addr) free(sm->addr);
	free(sm);
}


/**
 * Return the adress of the shared memory block
 */
static void *shm_get_adr(SHAREDMEM *sm)
{
	if (!sm) return NULL;
	return sm->addr;
}


/**
 * Generate a global identifier for the specified shared memory block
 */
static void shm_get_ident(SHAREDMEM *sm, char *dst)
{
	if (!sm) return;
	snprintf(dst, 32, "%x", (int)sm->addr);
}


/**
 * Share memory block to another thread
 */
static s32 shm_share(SHAREDMEM *sm, void *dst_thread)
{
	return 0;
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct sharedmem_services sharedmem = {
	shm_alloc,
	shm_destroy,
	shm_get_adr,
	shm_get_ident,
	shm_share,
};


/************************
 ** Module entry point **
 ************************/

int init_sharedmem(struct dope_services *d)
{
	d->register_module("SharedMemory 1.0",&sharedmem);
	return 1;
}
