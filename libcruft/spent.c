/*
 * dietlibc/lib/spent.c - getspent(3)/setspent(3)/endspent(3) implementation
 *
 * Copyright 2001 Jeff Garzik <jgarzik@mandrakesoft.com>
 *
 * This is a brand new implementation, based on the interface
 * described in man-pages-1.34-1mdk man pages package.
 */


#include "entlib.h" /* this is our only include */

static struct __ent_state *st = NULL; /* state held across calls */
static struct spwd sp;

struct spwd *getspent(void)
{
	char *parts[9];

	if (__ent_start(_PATH_SHADOW, &st))
		return NULL;
	if (__ent_get_line(st))
		goto err_out;
	if (__ent_split(st, parts, 9, ':', 1) < 0)
		goto err_out;

	sp.sp_namp	= parts[0];
	sp.sp_pwdp	= parts[1];

	if (*parts[2]!=0) 
	  sp.sp_lstchg	= atoi(parts[2]); /* XXX: atol */
	else
	  sp.sp_lstchg	= -1;

	if (*parts[3]!=0) 
	  sp.sp_min	= atoi(parts[3]);
	else
	  sp.sp_min	= -1;

	if (*parts[4]!=0) 
	  sp.sp_max	= atoi(parts[4]);
	else
	  sp.sp_max	= -1;

	if (*parts[5]!=0) 
	  sp.sp_warn	= atoi(parts[5]);
	else
	  sp.sp_warn	= -1;

	if (*parts[6]!=0) 
	  sp.sp_inact	= atoi(parts[6]);
	else
	  sp.sp_inact	= -1;

	if (*parts[7]!=0) 
	  sp.sp_expire	= atoi(parts[7]);
	else
	  sp.sp_expire	= -1;

	if (*parts[8]!=0) 
	  sp.sp_flag	= atoi(parts[8]);
	else
	  sp.sp_flag	= -1;

	return &sp;

err_out:
	st->buflen = st->bufptr = 0;	/* invalidate file data buffer */
	return NULL;
}

void setspent(void) { __ent_set(st); }
void endspent(void) { __ent_end(st); st=NULL; }
