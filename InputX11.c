/**********************************************************************
 *
 * PROJECT:		Input library
 * FILE:		InputX11.c
 * LICENCE:		See Licence.txt
 * PURPOSE:		A portable input hooker library.
 *				Functions to query X11 input systems.
 *
 *				(c) Tuomo Jauhiainen 2012-13
 *
 **********************************************************************/

#ifndef _WIN32

#include "Input.h"
#include "InputSys.h"
#include <X11/Xlib.h>

void* window;

void input_platform_initialize( void* window )
{
	UNREFERENCED_PARAM( window );
}

void input_platform_shutdown( void )
{
}

bool input_process( void* data )
{
	XEvent* event = (XEvent*)data;
	XKeyEvent* key;
	bool ret;

	switch ( event->type )
	{
	case KeyPress:
		{
			key = (XKeyEvent*)event;
			ret = input_handle_keyboard_event( INPUT_KEY_DOWN, (uint32)key->keycode );
			if ( ret )
			{
				ret = input_handle_key_down_bind( (uint32)key->keycode );
			}

			return ret ? 0 : 1;
		}
	case KeyRelease:
		{
			key = (XKeyEvent*)event;
			ret = input_handle_keyboard_event( INPUT_KEY_UP, (uint32)key->keycode );
			if ( ret )
			{
				ret = input_handle_key_up_bind( (uint32)key->keycode );
			}

			return ret ? 0 : 1;
		}

case ButtonPress:
case ButtonRelease:
case MotionNotify:
	break;

	}

	return true;
}

bool input_get_key_state( uint32 key )
{
	char keys[32];
	uint32 idx;

	XQueryKeymap( window, keys );

	idx = key >> 3;
	return keys[idx] & ( (key-idx) << 3 );
}

#endif /* _WIN32 */
