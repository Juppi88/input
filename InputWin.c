/**********************************************************************
 *
 * PROJECT:		Input library
 * FILE:		InputWin.c
 * LICENCE:		See Licence.txt
 * PURPOSE:		A portable input hooker library.
 *				Functions to query Windows input systems.
 *
 *				(c) Tuomo Jauhiainen 2012-13
 *
 **********************************************************************/

#ifdef _WIN32

#include "InputSys.h"

// --------------------------------------------------

static HWND	hwnd = NULL;
static WNDPROC old_proc = NULL;
static bool input_hooked = false;

// --------------------------------------------------

static LRESULT __stdcall input_process_hook( HWND wnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

// --------------------------------------------------

void input_platform_initialize( void* window )
{
	hwnd = (HWND)window;
}

void input_platform_shutdown( void )
{
	if ( input_hooked )
	{
		SetWindowLong( hwnd, GWL_WNDPROC, (LONG)old_proc );

		old_proc = false;
		input_hooked = false;
	}

	hwnd = NULL;
}

void input_enable_hook( bool enable )
{
	if ( enable && !input_hooked )
	{
		old_proc = (WNDPROC)GetWindowLong( hwnd, GWL_WNDPROC );
		input_hooked = true;
	}
	else if ( !enable && input_hooked )
	{
		SetWindowLong( hwnd, GWL_WNDPROC, (LONG)old_proc );

		old_proc = NULL;
		input_hooked = false;
	}
}

bool input_process( void* data )
{
	MSG* msg;
	bool ret;
	int16 x, y;

	msg = (MSG*)data;

	if ( data == NULL && input_hooked )
	{
		if ( hwnd && old_proc ) SetWindowLong( hwnd, GWL_WNDPROC, (LONG)input_process_hook );
		return true;
	}

	switch ( msg->message )
	{
	case WM_CHAR:
		{
			ret = input_handle_keyboard_event( INPUT_CHARACTER, (uint32)msg->wParam );
			if ( ret )
			{
				ret = input_handle_char_bind( (uint32)msg->wParam );
			}

			return ret;
		}

	case WM_KEYUP:
	case WM_SYSKEYUP:
		{
			ret = input_handle_keyboard_event( INPUT_KEY_UP, (uint32)msg->wParam );
			if ( ret )
			{
				ret = input_handle_key_up_bind( (uint32)msg->wParam );
			}

			return ret;
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

			return ret;
		}

	case WM_MOUSEMOVE:
		{
			x = (int16)LOWORD(msg->lParam);
			y = (int16)HIWORD(msg->lParam);

			ret = input_handle_mouse_event( INPUT_MOUSE_MOVE, x, y, MOUSE_NONE, MWHEEL_STATIONARY );
			if ( ret )
			{
				ret = input_handle_mouse_move_bind( x, y );
			}

			return ret;
		}

	case WM_MOUSEWHEEL:
		{
			return input_handle_mouse_event( INPUT_MOUSE_WHEEL,
				(int16)LOWORD(msg->lParam), (int16)HIWORD(msg->lParam), MOUSE_NONE,
				(float)((short)HIWORD((DWORD)msg->wParam)) > 0 ? MWHEEL_UP : MWHEEL_DOWN );
		}

	case WM_LBUTTONUP:
		{
			x = (int16)LOWORD(msg->lParam);
			y = (int16)HIWORD(msg->lParam);

			ReleaseCapture();

			ret = input_handle_mouse_event( INPUT_LBUTTON_UP, x, y, MOUSE_LBUTTON, MWHEEL_STATIONARY );
			if ( ret )
			{
				input_handle_mouse_up_bind( MOUSE_LBUTTON, x, y );
			}

			return ret;
		}

	case WM_LBUTTONDOWN:
		{
			x = (int16)LOWORD(msg->lParam);
			y = (int16)HIWORD(msg->lParam);

			SetCapture( msg->hwnd );

			ret = input_handle_mouse_event( INPUT_LBUTTON_DOWN, x, y, MOUSE_LBUTTON, MWHEEL_STATIONARY );
			if ( ret )
			{
				ret = input_handle_mouse_down_bind( MOUSE_LBUTTON, x, y );
			}

			return ret;
		}

	case WM_MBUTTONUP:
		{
			x = (int16)LOWORD(msg->lParam);
			y = (int16)HIWORD(msg->lParam);

			ReleaseCapture();
			ClipCursor( NULL );

			ret = input_handle_mouse_event( INPUT_MBUTTON_UP, x, y, MOUSE_MBUTTON, MWHEEL_STATIONARY );
			if ( ret )
			{
				ret = input_handle_mouse_up_bind( MOUSE_MBUTTON, x, y );
			}

			return ret;
		}

	case WM_MBUTTONDOWN:
		{
			x = (int16)LOWORD(msg->lParam);
			y = (int16)HIWORD(msg->lParam);

			SetCapture( msg->hwnd );

			ret = input_handle_mouse_event( INPUT_MBUTTON_DOWN, x, y, MOUSE_MBUTTON, MWHEEL_STATIONARY );
			if ( ret )
			{
				ret = input_handle_mouse_down_bind( MOUSE_MBUTTON, x, y );
			}

			return ret;
		}

	case WM_RBUTTONUP:
		{
			x = (int16)LOWORD(msg->lParam);
			y = (int16)HIWORD(msg->lParam);

			ReleaseCapture();

			ret = input_handle_mouse_event( INPUT_RBUTTON_UP, x, y, MOUSE_RBUTTON, MWHEEL_STATIONARY );
			if ( ret )
			{
				ret = input_handle_mouse_up_bind( MOUSE_RBUTTON, x, y );
			}

			return ret;
		}

	case WM_RBUTTONDOWN:
		{
			x = (int16)LOWORD(msg->lParam);
			y = (int16)HIWORD(msg->lParam);

			SetCapture( msg->hwnd );

			ret = input_handle_mouse_event( INPUT_RBUTTON_DOWN, x, y, MOUSE_RBUTTON, MWHEEL_STATIONARY );
			if ( ret )
			{
				ret = input_handle_mouse_down_bind( MOUSE_RBUTTON, x, y );
			}

			return ret;
		}
	}

	return true;
}

static LRESULT __stdcall input_process_hook( HWND wnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	MSG msg;
	
	if ( old_proc == NULL || wnd != hwnd ) return 0;

	msg.hwnd = wnd;
	msg.message = uMsg;
	msg.wParam = wParam;
	msg.lParam = lParam;

	if ( input_process( &msg ) )
		return CallWindowProc( old_proc, hwnd, uMsg, wParam, lParam );
	
	return 0;
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
