/*
 * \brief   Heap memory management
 * \date    2004-05-23
 * \author  Norman Feske
 *
 * This implementation is quite stupid and slow.
 */

/*
 * Copyright (C) 2002-2007 Norman Feske
 * Copyright (C) 2008-2014 Genode Labs GmbH
 *
 * This file is part of the DOpE package, which is distributed under
 * the terms of the GNU General Public Licence 2.
 */

/* local includes */
#include "dopestd.h"

static char heap[4000*1000];

struct memblock;
struct memblock {
	long    size;
	struct  memblock *next;
};

static void  *membase = &heap;         /* begin of memarea               */
static long   memsize = 4000*1000;      /* size of whole memarea          */
static struct memblock *firstmemblock = (struct memblock *)&heap;

//void  dope_free(void *adr);
//void *dope_malloc(unsigned int blocksize);


/**
 * Determines space between two memory blocks (internal)
 */
long mem_get_diff( struct memblock *blk1 )
{
	long s;
	struct memblock *blk2 = blk1->next;

	/* if in the middle of block list */
	if (blk2 != NULL) s = (long)blk2 - (long)blk1 - blk1->size - 8;

	/* last block of the list */
	else s = (long)membase + memsize - (long)blk1 - blk1->size - 8;

	return s;
};


/**
 * Free memoryblock
 */
void free(void const *adr)
{
	struct memblock *prevblk;
	struct memblock *blk;

	blk = (struct memblock *)((long)adr - 8);

	/* search previous block */
	prevblk = firstmemblock;
	while ((prevblk != NULL) && (prevblk->next != blk)) {
		prevblk = prevblk->next;
	};

	if (prevblk->next != blk) {

		/* No block found! */
		return;
	} else if (blk && prevblk) prevblk->next = blk->next;
};


/**
 * Allocate memoryblock
 */
void *malloc(unsigned int blocksize)
{
	struct memblock *blk;
	unsigned long freelenght;
	struct memblock *minblk;
	struct memblock *newblk;

	/* round blocksize up to an long-even size */
	blocksize += 4;
	blocksize &= 0xfffffffc;

	blk = firstmemblock;
	minblk = (memblock *)-1;

	/* search fitting block */
	while (blk != NULL) {
		freelenght = mem_get_diff(blk);
		if (freelenght >= blocksize+8) {
			minblk = blk;
			break;
		};
		blk = blk->next;
	};

	if ((long)minblk != -1) {

		/* insert new block after minblk */
		newblk = (memblock *)((long)minblk + 8 + minblk->size);
		newblk->next = minblk->next;
		minblk->next = newblk;
		newblk->size = blocksize;

		memset((void *)((long)newblk + (long)8), 0, blocksize);

		/* return address if first free byte of memory block */
		return (void *)((long)newblk + (long)8);
	};
	return NULL;
};
