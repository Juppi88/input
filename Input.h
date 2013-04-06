/**********************************************************************
 *
 * PROJECT:		Input library
 * FILE:		Input.h
 * LICENCE:		See Licence.txt
 * PURPOSE:		A portable input hooker library.
 *
 *				(c) Tuomo Jauhiainen 2012
 *
 **********************************************************************/

#pragma once
#ifndef __MYLLY_INPUT_H
#define __MYLLY_INPUT_H

#include "Math/Rectangle.h"
#include "Input/KeyDefs.h"

typedef enum
{
	INPUT_CHARACTER,		// Input a character
	INPUT_KEY_UP,			// A keyboard button is released
	INPUT_KEY_DOWN,			// A keyboard button is pressed
	INPUT_MOUSE_MOVE,		// Mouse movement
	INPUT_MOUSE_WHEEL,		// Mouse scroll
	INPUT_LBUTTON_UP,		// Left mouse button released
	INPUT_LBUTTON_DOWN,		// Left mouse button pressed
	INPUT_MBUTTON_UP,		// Middle mouse button released
	INPUT_MBUTTON_DOWN,		// Middle mouse button pressed
	INPUT_RBUTTON_UP,		// Right mouse button released
	INPUT_RBUTTON_DOWN,		// Right mouse button pressed
	NUM_INPUT_EVENTS
} INPUT_EVENT;

typedef enum
{
	MOUSE_LBUTTON = 0x1,
	MOUSE_MBUTTON,
	MOUSE_RBUTTON,
} MOUSEBTN;

typedef struct
{
	INPUT_EVENT type;

	union {
		struct {
			uint16	x;
			uint16	y;
			int16	dx;
			int16	dy;
			float	dwheel;
		} mouse;

		struct {
			uint32	key;
		} keyboard;
	};
}
input_event_t;

typedef struct keybind_s	keybind_t;
typedef struct mousebind_s	mousebind_t;

typedef bool				( *input_handler_t )			( input_event_t* event );
typedef bool				( *keybind_func_t )				( uint32 key, void* data );
typedef bool				( *mousebind_func_t )			( MOUSEBTN button, uint16 x, uint16 y, void* data );

__BEGIN_DECLS

MYLLY_API void				input_initialize				( void* window );
MYLLY_API void				input_shutdown					( void );
MYLLY_API bool				input_process					( void* data );

MYLLY_API void				input_add_hook					( INPUT_EVENT event, input_handler_t handler );
MYLLY_API void				input_remove_hook				( INPUT_EVENT event, input_handler_t handler );

MYLLY_API keybind_t*		input_add_char_bind				( uint32 key, keybind_func_t func, void* data );
MYLLY_API keybind_t*		input_add_key_up_bind			( uint32 key, keybind_func_t func, void* data );
MYLLY_API keybind_t*		input_add_key_down_bind			( uint32 key, keybind_func_t func, void* data );
MYLLY_API mousebind_t*		input_add_mouse_move_bind		( rectangle_t* r, mousebind_func_t func, void* data );
MYLLY_API mousebind_t*		input_add_mousebtn_up_bind		( MOUSEBTN button, rectangle_t* r, mousebind_func_t func, void* data );
MYLLY_API mousebind_t*		input_add_mousebtn_down_bind	( MOUSEBTN button, rectangle_t* r, mousebind_func_t func, void* data );

MYLLY_API void				input_remove_char_bind			( uint32 key, keybind_func_t func );
MYLLY_API void				input_remove_key_up_bind		( uint32 key, keybind_func_t func );
MYLLY_API void				input_remove_key_down_bind		( uint32 key, keybind_func_t func );
MYLLY_API void				input_remove_mouse_move_bind	( mousebind_func_t func );
MYLLY_API void				input_remove_mousebtn_up_bind	( MOUSEBTN button, mousebind_func_t func );
MYLLY_API void				input_remove_mousebtn_down_bind	( MOUSEBTN button, mousebind_func_t func );
MYLLY_API void				input_remove_key_bind			( keybind_t* bind );
MYLLY_API void				input_remove_mouse_bind			( mousebind_t* bind );

MYLLY_API void				input_set_mousebind_button		( mousebind_t* bind, MOUSEBTN button );
MYLLY_API void				input_set_mousebind_rect		( mousebind_t* bind, rectangle_t* r );
MYLLY_API void				input_set_mousebind_func		( mousebind_t* bind, mousebind_func_t func );
MYLLY_API void				input_set_mousebind_param		( mousebind_t* bind, void* data );

MYLLY_API bool				input_get_key_state				( uint32 key );
MYLLY_API void				input_block_keys				( bool block );

MYLLY_API void				input_show_mouse_cursor			( bool show );
MYLLY_API bool				input_is_cursor_showing			( void );
MYLLY_API void				input_get_cursor_pos			( uint16* x, uint16* y );
MYLLY_API void				input_set_cursor_pos			( uint16 x, uint16 y );
			
__END_DECLS

#endif /* __MYLLY_INPUT_H */
