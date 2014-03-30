/*
 * \brief   DOpE timer module
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

/* Genode includes */
#include <base/env.h>
#include <timer_session/connection.h>

/* local includes */
#include "dopestd.h"
#include "timer.h"

int init_timer(struct dope_services *d);

static Timer::Session *timer_session;


/***********************
 ** Service functions **
 ***********************/

/**
 * Return current system time counter in microseconds
 */
static u32 get_time(void)
{
	return timer_session->elapsed_ms()*1000;
}


/**
 * Return difference between two times
 */
static u32 get_diff(u32 time1,u32 time2)
{
	/* overflow check */
	if (time1>time2) {
		time1 -= time2;
		return (u32)0xffffffff - time1;
	}
	return time2-time1;
}


/**
 * Wait specified number of microseconds
 */
static void wait_usec(u32 num_usec)
{
	timer_session->usleep(num_usec);
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct timer_services services = {
	get_time,
	get_diff,
	wait_usec,
};


/************************
 ** Module entry point **
 ************************/

int init_timer(struct dope_services *d)
{
	static Timer::Connection timer;
	timer_session = &timer;

	d->register_module("Timer 1.0",&services);
	return 1;
}
