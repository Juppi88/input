/**********************************************************************
 *
 * PROJECT:		Mylly Input library
 * FILE:		Input.h
 * LICENCE:		See Licence.txt
 * PURPOSE:		A portable input hooker library. Public header.
 *
 *				(c) Tuomo Jauhiainen 2012-13
 *
 **********************************************************************/

#pragma once
#ifndef __MYLLY_INPUT_H
#define __MYLLY_INPUT_H

#include "Math/Rectangle.h"
#include "Input/KeyDefs.h"

/**
 * Input event types.
 *
 * List of supported input types for direct user input hooking
 * via input_add_hook.
 */
typedef enum {
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

/**
 * Mouse buttons.
 * Used for mouse binds and input hooks.
 */
typedef enum {
	MOUSE_NONE,
	MOUSE_LBUTTON,
	MOUSE_MBUTTON,
	MOUSE_RBUTTON,
	MOUSE_FORCE_DWORD = 0x7FFFFFFF
} MOUSEBTN;

/**
 * Mouse wheel movement.
 * Used to report the current state of the mouse wheel.
 */
typedef enum {
	MWHEEL_STATIONARY,
	MWHEEL_UP,
	MWHEEL_DOWN,
} MOUSEWHEEL;

/**
 * Input hook callback arguments.
 *
 * Passed to an input hook function when a event that is hooked is triggered.
 * If the event is a keyboard event, 'keyboard' field of the struct is used,
 * similarly the 'mouse' field is used when a mouse event is triggered.
 */
typedef struct {
	/* Type of the event is always returned first. */
	INPUT_EVENT type;

	union {
		/* Mouse info, returned when a mouse event is triggered. */
		struct {
			int16 x, y;		/* Current mouse cursor position. */
			int16 dx, dy;	/* Cursor position change since the last callback. */
			uint8 button;	/* Pressed button (see MOUSEBTN above). */
			uint8 wheel;	/* Mouse wheel movement (see MOUSEWHEEL above). */
		} mouse;

		/* Keyboard info returns the key that triggered a keybord event. */
		struct {
			uint32 key;		/* Pressed key or injected chracter. */
		} keyboard;
	};
} InputEvent;

/**
 * Typedefs for key/mouse bind data and bind/hook functions.
 */
typedef struct KeyBind		KeyBind;
typedef struct MouseBind	MouseBind;

typedef bool			( *input_handler_t )			( InputEvent* event );
typedef bool			( *keybind_func_t )				( uint32 key, void* data );
typedef bool			( *mousebind_func_t )			( MOUSEBTN button, uint16 x, uint16 y, void* data );

__BEGIN_DECLS

MYLLY_API void			input_initialize				( void* window );
MYLLY_API void			input_shutdown					( void );
MYLLY_API bool			input_process					( void* data );

MYLLY_API void			input_enable_hook				( bool enable );

MYLLY_API void			input_add_hook					( INPUT_EVENT event, input_handler_t handler );
MYLLY_API void			input_remove_hook				( INPUT_EVENT event, input_handler_t handler );

MYLLY_API KeyBind*		input_add_char_bind				( uint32 key, keybind_func_t func, void* data );
MYLLY_API KeyBind*		input_add_key_up_bind			( uint32 key, keybind_func_t func, void* data );
MYLLY_API KeyBind*		input_add_key_down_bind			( uint32 key, keybind_func_t func, void* data );
MYLLY_API MouseBind*	input_add_mouse_move_bind		( rectangle_t* r, mousebind_func_t func, void* data );
MYLLY_API MouseBind*	input_add_mousebtn_up_bind		( MOUSEBTN button, rectangle_t* r, mousebind_func_t func, void* data );
MYLLY_API MouseBind*	input_add_mousebtn_down_bind	( MOUSEBTN button, rectangle_t* r, mousebind_func_t func, void* data );

MYLLY_API void			input_remove_char_bind			( uint32 key, keybind_func_t func );
MYLLY_API void			input_remove_key_up_bind		( uint32 key, keybind_func_t func );
MYLLY_API void			input_remove_key_down_bind		( uint32 key, keybind_func_t func );
MYLLY_API void			input_remove_mouse_move_bind	( mousebind_func_t func );
MYLLY_API void			input_remove_mousebtn_up_bind	( MOUSEBTN button, mousebind_func_t func );
MYLLY_API void			input_remove_mousebtn_down_bind	( MOUSEBTN button, mousebind_func_t func );
MYLLY_API void			input_remove_key_bind			( KeyBind* bind );
MYLLY_API void			input_remove_mouse_bind			( MouseBind* bind );

MYLLY_API void			input_set_mousebind_button		( MouseBind* bind, MOUSEBTN button );
MYLLY_API void			input_set_mousebind_rect		( MouseBind* bind, rectangle_t* r );
MYLLY_API void			input_set_mousebind_func		( MouseBind* bind, mousebind_func_t func );
MYLLY_API void			input_set_mousebind_param		( MouseBind* bind, void* data );

MYLLY_API bool			input_get_key_state				( uint32 key );
MYLLY_API void			input_block_keys				( bool block );

MYLLY_API void			input_show_mouse_cursor			( bool show );
MYLLY_API void			input_show_mouse_cursor_ref		( bool show );
MYLLY_API bool			input_is_cursor_showing			( void );
MYLLY_API void			input_get_cursor_pos			( int16* x, int16* y );
MYLLY_API void			input_set_cursor_pos			( int16 x, int16 y );

__END_DECLS

#endif /* __MYLLY_INPUT_H */
