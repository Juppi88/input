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

static HWND			hwnd							= NULL;

void __input_initialize( void* window )
{
	hwnd = (HWND)window;
}

void __input_shutdown( void )
{
	hwnd = NULL;
}

bool input_process( void* data )
{
	MSG* msg;
	bool ret;
	uint16 x, y;

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
			x = (uint16)LOWORD(msg->lParam);
			y = (uint16)HIWORD(msg->lParam);

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
			x = (uint16)LOWORD(msg->lParam);
			y = (uint16)HIWORD(msg->lParam);

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
			x = (uint16)LOWORD(msg->lParam);
			y = (uint16)HIWORD(msg->lParam);

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
			x = (uint16)LOWORD(msg->lParam);
			y = (uint16)HIWORD(msg->lParam);

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
			x = (uint16)LOWORD(msg->lParam);
			y = (uint16)HIWORD(msg->lParam);

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
			x = (uint16)LOWORD(msg->lParam);
			y = (uint16)HIWORD(msg->lParam);

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
			x = (uint16)LOWORD(msg->lParam);
			y = (uint16)HIWORD(msg->lParam);

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

void input_set_cursor_pos( uint16 x, uint16 y )
{
	extern uint16 mouse_x, mouse_y;

	mouse_x = x;
	mouse_y = y;

	SetCursorPos( x, y );
}

#endif /* _WIN32 */
