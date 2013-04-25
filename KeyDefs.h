/**********************************************************************
 *
 * PROJECT:		Input library
 * FILE:		KeyDefs.h
 * LICENCE:		See Licence.txt
 * PURPOSE:		Key definitions.
 *
 *				(c) Tuomo Jauhiainen 2012
 *
 **********************************************************************/

#pragma once
#ifndef __MYLLY_INPUT_KEYDEFS_H
#define __MYLLY_INPUT_KEYDEFS_H

#ifdef _WIN32

#include "stdtypes.h"

#define MKEY_LMB			VK_LBUTTON
#define MKEY_RMB			VK_RBUTTON
#define MKEY_MMB			VK_MBUTTON

#define MKEY_CANCEL			VK_CANCEL
#define MKEY_BACKSPACE		VK_BACK
#define MKEY_TAB			VK_TAB
#define MKEY_CLEAR			VK_CLEAR
#define MKEY_RETURN			VK_RETURN

#define MKEY_SHIFT			VK_SHIFT
#define MKEY_CONTROL		VK_CONTROL
#define MKEY_ALT			VK_MENU
#define MKEY_PAUSE			VK_PAUSE
#define MKEY_CAPS			VK_CAPITAL

#define MKEY_ESCAPE			VK_ESCAPE

#define MKEY_SPACE			VK_SPACE
#define MKEY_PAGEDOWN		VK_PRIOR
#define MKEY_PAGEUP			VK_NEXT
#define MKEY_END			VK_END
#define MKEY_HOME			VK_HOME
#define MKEY_LEFT			VK_LEFT
#define MKEY_UP				VK_UP
#define MKEY_RIGHT			VK_RIGHT
#define MKEY_DOWN			VK_DOWN
#define MKEY_PRINTSCR		VK_SNAPSHOT
#define MKEY_INSERT			VK_INSERT
#define MKEY_DELETE			VK_DELETE

/*
 * ASCII 0-9 and A-Z are the same as their ASCII codes.
 * Note that you should be using uppercase characters with
 * input_add_key_(up/down)_bind
 */

#define MKEY_NUMPAD0		VK_NUMPAD0
#define MKEY_NUMPAD1		VK_NUMPAD1
#define MKEY_NUMPAD2		VK_NUMPAD2
#define MKEY_NUMPAD3		VK_NUMPAD3
#define MKEY_NUMPAD4		VK_NUMPAD4
#define MKEY_NUMPAD5		VK_NUMPAD5
#define MKEY_NUMPAD6		VK_NUMPAD6
#define MKEY_NUMPAD7		VK_NUMPAD7
#define MKEY_NUMPAD8		VK_NUMPAD8
#define MKEY_NUMPAD9		VK_NUMPAD9
#define MKEY_MULTIPLY		VK_MULTIPLY
#define MKEY_ADD			VK_ADD
#define MKEY_SEPARATOR		VK_SEPARATOR
#define MKEY_SUBTRACT		VK_SUBTRACT
#define MKEY_DECIMAL		VK_DECIMAL
#define MKEY_DIVIDE			VK_DIVIDE
#define MKEY_F1				VK_F1
#define MKEY_F2				VK_F2
#define MKEY_F3				VK_F3
#define MKEY_F4				VK_F4
#define MKEY_F5				VK_F5
#define MKEY_F6				VK_F6
#define MKEY_F7				VK_F7
#define MKEY_F8				VK_F8
#define MKEY_F9				VK_F9
#define MKEY_F10			VK_F10
#define MKEY_F11			VK_F11
#define MKEY_F12			VK_F12
#define MKEY_NUMLOCK		VK_NUMLOCK
#define MKEY_SCROLL			VK_SCROLL

/* The following can only be used to query the key state via input_get_key_state */
#define MKEY_LSHIFT			VK_LSHIFT
#define MKEY_RSHIFT			VK_RSHIFT
#define MKEY_LCONTROL		VK_LCONTROL
#define MKEY_RCONTROL		VK_RCONTROL
#define MKEY_LALT			VK_LMENU
#define MKEY_RALT			VK_RMENU

#else

#endif /* _WIN32 */

#endif /* __MYLLY_INPUT_KEYDEFS_H */
