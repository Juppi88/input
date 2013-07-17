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

static void* window;

void input_platform_initialize( void* wnd )
{
	window = wnd;
}

void input_platform_shutdown( void )
{
	window = NULL;
}

bool input_process( void* data )
{
	XEvent* event = (XEvent*)data;
	XKeyEvent* key;
	XButtonEvent* button;
	XMotionEvent* motion;
	int16 x, y;
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
		{
			button = (XButtonEvent*)event;

			x = (int16)button->x;
			y = (int16)button->y;

			switch ( button->button )
			{
			case Button1:
				// Left mouse button
				XGrabPointer( button->display, button->window, False, ButtonPressMask|ButtonReleaseMask|
								PointerMotionMask|FocusChangeMask|EnterWindowMask|LeaveWindowMask,
								GrabModeAsync, GrabModeAsync, button->window, None, CurrentTime );

				ret = input_handle_mouse_event( INPUT_LBUTTON_DOWN, x, y, 0.0f );
				if ( ret ) input_handle_mouse_down_bind( MOUSE_LBUTTON, x, y );

				break;

			case Button2:
				// Right mouse button
				ret = input_handle_mouse_event( INPUT_RBUTTON_DOWN, x, y, 0.0f );
				if ( ret ) input_handle_mouse_down_bind( MOUSE_RBUTTON, x, y );

				break;

			case Button3:
				// Middle mouse button (wheel)
				ret = input_handle_mouse_event( INPUT_MBUTTON_DOWN, x, y, 0.0f );
				if ( ret ) input_handle_mouse_down_bind( MOUSE_MBUTTON, x, y );

				break;
			}

			return ret ? 0 : 1;
		}

	case ButtonRelease:
		{
			button = (XButtonEvent*)event;

			x = (int16)button->x;
			y = (int16)button->y;

			switch ( button->button )
			{
			case Button1:
				// Left mouse button
				XUngrabPointer( button->display, CurrentTime );

				ret = input_handle_mouse_event( INPUT_LBUTTON_UP, x, y, 0.0f );
				if ( ret ) input_handle_mouse_up_bind( MOUSE_LBUTTON, x, y );

				break;

			case Button2:
				// Right mouse button
				ret = input_handle_mouse_event( INPUT_RBUTTON_UP, x, y, 0.0f );
				if ( ret ) input_handle_mouse_up_bind( MOUSE_RBUTTON, x, y );

				break;

			case Button3:
				// Middle mouse button (wheel)
				ret = input_handle_mouse_event( INPUT_MBUTTON_UP, x, y, 0.0f );
				if ( ret ) input_handle_mouse_up_bind( MOUSE_MBUTTON, x, y );

				break;
			}

			return ret ? 0 : 1;
		}

	case MotionNotify:
		{
			motion = (XMotionEvent*)event;
			x = (int16)motion->x;
			y = (int16)motion->y;

			ret = input_handle_mouse_event( INPUT_MOUSE_MOVE, x, y, 0.0f );
			if ( ret )
			{
				ret = input_handle_mouse_move_bind( x, y );
			}

			return ret ? 0 : 1;
		}
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
