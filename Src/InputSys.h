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
bool	input_handle_mouse_event		( INPUT_EVENT type, uint16 x, uint16 y, float wheel );
void	input_handle_char_bind			( uint32 key );
void	input_handle_key_up_bind		( uint32 key );
void	input_handle_key_down_bind		( uint32 key );
void	input_handle_mouse_move_bind	( uint16 x, uint16 y );
void	input_handle_mouse_up_bind		( MOUSEBTN, uint16 x, uint16 y );
void	input_handle_mouse_down_bind	( MOUSEBTN, uint16 x, uint16 y );

// Platform specific library initializers
void	__input_initialize				( void* window );
void	__input_shutdown				( void );

#endif /* __MYLLY_INPUT_SYS_H */
