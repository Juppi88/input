/**********************************************************************
 *
 * PROJECT:		Input library
 * FILE:		InputWin.c
 * LICENCE:		See Licence.txt
 * PURPOSE:		A portable input hooker library.
 *				Functions to query Windows input systems.
 *
 *				(c) Tuomo Jauhiainen 2012
 *
 **********************************************************************/

#ifdef _WIN32

#include "InputSys.h"

static HWND	hwnd = NULL;

void input_platform_initialize( void* window )
{
	hwnd = (HWND)window;
}

void input_platform_shutdown( void )
{
	hwnd = NULL;
}

bool input_process( void* data )
{
	MSG* msg;
	bool ret;
	int16 x, y;

	msg = (MSG*)data;

	switch ( msg->message )
	{
	case WM_CHAR:
		{
			ret = input_handle_keyboard_event( INPUT_CHARACTER, (uint32)msg->wParam );
			if ( ret )
			{
				ret = input_handle_char_bind( (uint32)msg->wParam );
			}

			return ret ? 0 : 1;
		}

	case WM_KEYUP:
	case WM_SYSKEYUP:
		{
			ret = input_handle_keyboard_event( INPUT_KEY_UP, (uint32)msg->wParam );
			if ( ret )
			{
				ret = input_handle_key_up_bind( (uint32)msg->wParam );
			}

			return ret ? 0 : 1;
		}

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			ret = input_handle_keyboard_event( INPUT_KEY_DOWN, (uint32)msg->wParam );
			if ( ret )
			{
				ret = input_handle_key_down_bind( (uint32)msg->wParam );
			}

			if ( !ret )
			{
				// This is here because the windows input model is retarded and also sends a WM_CHAR event for pressed down keys
				while ( PeekMessage( msg, hwnd, WM_CHAR, WM_CHAR, PM_REMOVE ) ) {} 
			}

			return ret ? 0 : 1;
		}

	case WM_MOUSEMOVE:
		{
			x = (int16)LOWORD(msg->lParam);
			y = (int16)HIWORD(msg->lParam);

			ret = input_handle_mouse_event( INPUT_MOUSE_MOVE, x, y, 0.0f );
			if ( ret )
			{
				ret = input_handle_mouse_move_bind( x, y );
			}

			return ret ? 0 : 1;
		} 

	case WM_MOUSEWHEEL:
		{
			return input_handle_mouse_event( INPUT_MOUSE_WHEEL,
				(uint16)LOWORD(msg->lParam), (uint16)HIWORD(msg->lParam),
				(float)((short)HIWORD((DWORD)msg->wParam)) ) ? 0 : 1;
		}

	case WM_LBUTTONUP:
		{
			x = (int16)LOWORD(msg->lParam);
			y = (int16)HIWORD(msg->lParam);

			ReleaseCapture();

			ret = input_handle_mouse_event( INPUT_LBUTTON_UP, x, y, 0.0f );
			if ( ret )
			{
				input_handle_mouse_up_bind( MOUSE_LBUTTON, x, y );
			}

			return ret ? 0 : 1;
		} 
		
	case WM_LBUTTONDOWN:
		{
			x = (int16)LOWORD(msg->lParam);
			y = (int16)HIWORD(msg->lParam);

			SetCapture( msg->hwnd );

			ret = input_handle_mouse_event( INPUT_LBUTTON_DOWN, x, y, 0.0f );
			if ( ret )
			{
				ret = input_handle_mouse_down_bind( MOUSE_LBUTTON, x, y );
			}

			return ret ? 0 : 1;
		}

	case WM_MBUTTONUP:
		{
			x = (int16)LOWORD(msg->lParam);
			y = (int16)HIWORD(msg->lParam);

			ReleaseCapture();
			ClipCursor( NULL );

			ret = input_handle_mouse_event( INPUT_MBUTTON_UP, x, y, 0.0f );
			if ( ret )
			{
				ret = input_handle_mouse_up_bind( MOUSE_MBUTTON, x, y );
			}

			return ret ? 0 : 1;
		} 

	case WM_MBUTTONDOWN:
		{
			x = (int16)LOWORD(msg->lParam);
			y = (int16)HIWORD(msg->lParam);

			SetCapture( msg->hwnd );

			ret = input_handle_mouse_event( INPUT_MBUTTON_DOWN, x, y, 0.0f );
			if ( ret )
			{
				ret = input_handle_mouse_down_bind( MOUSE_MBUTTON, x, y );
			}

			return ret ? 0 : 1;
		}

	case WM_RBUTTONUP:
		{
			x = (int16)LOWORD(msg->lParam);
			y = (int16)HIWORD(msg->lParam);

			ReleaseCapture();

			ret = input_handle_mouse_event( INPUT_RBUTTON_UP, x, y, 0.0f );
			if ( ret )
			{
				ret = input_handle_mouse_up_bind( MOUSE_RBUTTON, x, y );
			}

			return ret ? 0 : 1;
		} 

	case WM_RBUTTONDOWN:
		{
			x = (int16)LOWORD(msg->lParam);
			y = (int16)HIWORD(msg->lParam);

			SetCapture( msg->hwnd );

			ret = input_handle_mouse_event( INPUT_RBUTTON_DOWN, x, y, 0.0f );
			if ( ret )
			{
				ret = input_handle_mouse_down_bind( MOUSE_RBUTTON, x, y );
			}

			return ret ? 0 : 1;
		}
	}

	return true;
}

bool input_get_key_state( uint32 key )
{
	return ( GetKeyState( key ) & 0x8000 ) != 0;
}

void input_show_mouse_cursor( bool show )
{
	extern bool show_cursor;

	show_cursor = show;
	ShowCursor( show );
}

void input_show_mouse_cursor_ref( bool show )
{
	static uint32 refcount = 0;
	extern bool show_cursor;

	if ( !show )
	{
		if ( refcount )
		{
			if ( --refcount == 0 )
			{
				ShowCursor( FALSE );
				show_cursor = false;
			}
		}
	}
	else
	{
		if ( refcount++ == 0 )
		{
			ShowCursor( TRUE );
			show_cursor = true;
		}
	}
}

void input_set_cursor_pos( int16 x, int16 y )
{
	extern int16 mouse_x, mouse_y;

	mouse_x = x;
	mouse_y = y;

	SetCursorPos( x, y );
}

#endif /* _WIN32 */