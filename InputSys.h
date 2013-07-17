/**********************************************************************
 *
 * PROJECT:		Input library
 * FILE:		InputSys.h
 * LICENCE:		See Licence.txt
 * PURPOSE:		Platform specific input functionality.
 *
 *				(c) Tuomo Jauhiainen 2012
 *
 **********************************************************************/

#pragma once
#ifndef __MYLLY_INPUT_SYS_H
#define __MYLLY_INPUT_SYS_H

#include "Input.h"

// Generic functions used by platform specific implementation
bool	input_handle_keyboard_event		( INPUT_EVENT type, uint32 key );
bool	input_handle_mouse_event		( INPUT_EVENT type, int16 x, int16 y, float wheel );
bool	input_handle_char_bind			( uint32 key );
bool	input_handle_key_up_bind		( uint32 key );
bool	input_handle_key_down_bind		( uint32 key );
bool	input_handle_mouse_move_bind	( int16 x, int16 y );
bool	input_handle_mouse_up_bind		( MOUSEBTN, int16 x, int16 y );
bool	input_handle_mouse_down_bind	( MOUSEBTN, int16 x, int16 y );

// Platform specific library initializers
void	input_platform_initialize		( void* window );
void	input_platform_shutdown			( void );

#endif /* __MYLLY_INPUT_SYS_H */
