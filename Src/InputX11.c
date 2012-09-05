/**********************************************************************
 *
 * PROJECT:		Input library
 * FILE:		InputX11.c
 * LICENCE:		See Licence.txt
 * PURPOSE:		A portable input hooker library.
 *				Functions to query X11 input systems.
 *
 *				(c) Tuomo Jauhiainen 2012
 *
 **********************************************************************/

#ifndef _WIN32

#include "Input.h"
#include <X11/Xlib.h>

void* window;

bool input_get_key_state( uint32 key )
{
	char keys[32];
	uint32 idx;

	XQueryKeymap( window, keys );

	idx = key >> 3;
	return keys[idx] & ( (key-idx) << 3 );
}

#endif _WIN32
