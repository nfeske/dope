/*
 * \brief   Interface of shared memory abstraction of DOpE
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

#ifndef _DOPE_SHAREDMEM_H_
#define _DOPE_SHAREDMEM_H_

#define SHAREDMEM struct shared_memory
struct shared_memory;

struct sharedmem_services {
	SHAREDMEM *(*alloc)       (size_t size);
	void       (*destroy)     (SHAREDMEM *sm);
	void      *(*get_address) (SHAREDMEM *sm);
	void       (*get_ident)   (SHAREDMEM *sm, char *dst_ident_buf);
	s32        (*share)       (SHAREDMEM *sm, void *dst_thread);
};


#endif /* _DOPE_SHAREDMEM_H_ */
