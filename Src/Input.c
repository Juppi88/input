/**********************************************************************
 *
 * PROJECT:		Input library
 * FILE:		Input.c
 * LICENCE:		See Licence.txt
 * PURPOSE:		A portable input hooker library.
 *
 *				(c) Tuomo Jauhiainen 2012
 *
 **********************************************************************/

#include "Input.h"
#include "InputSys.h"
#include "Types/List.h"
#include "Types/Tree.h"
#include "Platform/Alloc.h"
#include "Platform/Window.h"
#include <assert.h>

static bool		input_initialized				= false;	// Is the library properly initialized?
static  bool	block_keys						= false;	// Should keyboard input be blocked
bool			show_cursor						= true;		// Display mouse cursor
uint16			mouse_x							= 0;		// Current mouse x coordinate
uint16			mouse_y							= 0;		// Current mouse y coordinate
static float	mouse_wheel						= 0.0f;		// Current wheel position
static list_t*	input_hooks[NUM_INPUT_EVENTS]	= { NULL };	// A list of custom input hooks
static tree_t*	char_binds						= NULL;		// Character input binds
static tree_t*	key_up_binds					= NULL;		// Key up hooks
static tree_t*	key_down_binds					= NULL;		// Key down hooks
static list_t*	mouse_up_binds					= NULL;		// Mouse button up binds
static list_t*	mouse_down_binds				= NULL;		// Mouse button down binds
static list_t*	mouse_move_binds				= NULL;		// Mouse move binds

typedef struct {
	node_t;
	input_handler_t handler;
} hookfunc;

typedef struct {
	tnode_t;
	list_t* list;
} bindnode;

struct keybind_s
{
	node_t;
	uint32				key;
	keybind_func_t		handler;
	void*				userdata;
	
	enum BINDTYPE {
		BIND_KEYUP,
		BIND_KEYDOWN,
		BIND_CHAR
	}					type;
};

struct mousebind_s
{
	node_t;
	rectangle_t			bounds;
	mousebind_func_t	handler;
	MOUSEBTN			button;
	void*				userdata;

	enum MBINDTYPE {
		BIND_BTNUP,
		BIND_BTNDOWN,
		BIND_MOVE
	}					type;
};

void input_bind_destructor( void* data )
{
	bindnode* bind;

	bind = (bindnode*)data;

	if ( bind->list )
		list_destroy( bind->list );

	mem_free( data );
}

void input_initialize( void* window )
{
	uint32 i;

	if ( !window ) return;

	for ( i = 0; i < NUM_INPUT_EVENTS; i++ )
	{
		// Initialize hook lists
		input_hooks[i] = list_create();
	}

	// Initialize key/mouse binds
	char_binds = tree_create( input_bind_destructor );
	key_up_binds = tree_create( input_bind_destructor );
	key_down_binds = tree_create( input_bind_destructor );
	mouse_up_binds = list_create();
	mouse_down_binds = list_create();
	mouse_move_binds = list_create();

	input_initialized = true;

	// Do window system specific initializing (event hooks etc)
	__input_initialize( window );
}

void input_shutdown( void )
{
	uint32 i;

	for ( i = 0; i < NUM_INPUT_EVENTS; i++ )
	{
		// Destroy the hook lists
		if ( input_hooks[i] )
		{
			list_destroy( input_hooks[i] );
			input_hooks[i] = NULL;
		}
	}

	// Delink key/mouse binds
	// TODO: Free bind memory
	tree_destroy( char_binds );
	tree_destroy( key_up_binds );
	tree_destroy( key_down_binds );
	list_destroy( mouse_up_binds );
	list_destroy( mouse_down_binds );
	list_destroy( mouse_move_binds );

	char_binds = NULL;
	key_up_binds = NULL;
	key_down_binds = NULL;
	mouse_up_binds = NULL;
	mouse_down_binds = NULL;
	mouse_move_binds = NULL;

	input_initialized = false;

	// Do window system specific cleanup
	__input_shutdown();
}

void input_add_hook( INPUT_EVENT event, input_handler_t handler )
{
	list_t* list;
	hookfunc* hook;

	if ( !input_initialized ) return;

	list = input_hooks[event];
	hook = mem_alloc( sizeof(*hook) );
	hook->handler = handler;

	list_push( list, (node_t*)hook );
}

void input_remove_hook( INPUT_EVENT event, input_handler_t handler )
{
	list_t* list;
	node_t* node;
	hookfunc* hook;

	if ( !input_initialized ) return;

	list = input_hooks[event];
	
	list_foreach( list, node )
	{
		hook = (hookfunc*)node;
		if ( handler == hook->handler )
		{
			list_remove( list, node );
			mem_free( hook );

			return;
		}
	}
}

static keybind_t* __input_add_key_bind( uint32 key, keybind_func_t func, void* data, enum BINDTYPE type )
{
	bindnode* binds;
	keybind_t* bind;
	tree_t* bindtree = NULL;

	switch ( type )
	{
	case BIND_CHAR: bindtree = char_binds; break;
	case BIND_KEYUP: bindtree = key_up_binds; break;
	case BIND_KEYDOWN: bindtree = key_down_binds; break;
	}

	binds = (bindnode*)tree_find( bindtree, key );
	if ( !binds )
	{
		// We have no previous binds for this key, add a new list
		binds = mem_alloc( sizeof(*binds) );
		binds->list = list_create();
		binds->key = key;
		binds->left = NULL;
		binds->right = NULL;
		tree_insert( bindtree, (tnode_t*)binds );
	}

	// Add the bind
	bind = (keybind_t*)mem_alloc_clean( sizeof(*bind) );
	bind->key = key;
	bind->handler = func;
	bind->type = type;
	bind->userdata = data;

	list_push( binds->list, (node_t*)bind );

	return bind;
}

keybind_t* input_add_char_bind( uint32 key, keybind_func_t func, void* data )
{
	return __input_add_key_bind( key, func, data, BIND_CHAR );
}

keybind_t* input_add_key_up_bind( uint32 key, keybind_func_t func, void* data )
{
	return __input_add_key_bind( key, func, data, BIND_KEYUP );
}

keybind_t* input_add_key_down_bind( uint32 key, keybind_func_t func, void* data )
{
	return __input_add_key_bind( key, func, data, BIND_KEYDOWN );
}

static mousebind_t* __input_add_mouse_bind( MOUSEBTN button, rectangle_t* r, mousebind_func_t func, void* data, enum MBINDTYPE type )
{
	mousebind_t* bind;
	list_t* bindlist = NULL;

	switch ( type )
	{
	case BIND_MOVE: bindlist = mouse_move_binds; break;
	case BIND_BTNUP: bindlist = mouse_up_binds; break;
	case BIND_BTNDOWN: bindlist = mouse_down_binds; break;
	}

	// Add the bind
	bind = (mousebind_t*)mem_alloc_clean( sizeof(*bind) );
	bind->bounds = *r;
	bind->handler = func;
	bind->button = button;
	bind->type = type;
	bind->userdata = data;

	list_push( bindlist, (node_t*)bind );

	return bind;
}

mousebind_t* input_add_mouse_move_bind( rectangle_t* r, mousebind_func_t func, void* data )
{
	return __input_add_mouse_bind( 0, r, func, data, BIND_MOVE );
}

mousebind_t* input_add_mousebtn_up_bind( MOUSEBTN button, rectangle_t* r, mousebind_func_t func, void* data )
{
	return __input_add_mouse_bind( button, r, func, data, BIND_BTNUP );
}

mousebind_t* input_add_mousebtn_down_bind( MOUSEBTN button, rectangle_t* r, mousebind_func_t func, void* data )
{
	return __input_add_mouse_bind( button, r, func, data, BIND_BTNDOWN );
}


static void __input_remove_key_bind( uint32 key, keybind_func_t func, enum BINDTYPE type )
{
	bindnode* binds;
	keybind_t* bind;
	node_t *node, *tmp;
	tree_t* tree = NULL;

	switch ( type )
	{
	case BIND_CHAR: tree = char_binds; break;
	case BIND_KEYUP: tree = key_up_binds; break;
	case BIND_KEYDOWN: tree = key_down_binds; break;
	}

	binds = (bindnode*)tree_find( tree, key );
	if ( !binds ) return; // No binds for the given key

	list_foreach_safe( binds->list, node, tmp )
	{
		bind = (keybind_t*)node;
		if ( bind->key == key && bind->handler == func )
		{
			list_remove( binds->list, node );
			mem_free( bind );
		}
	}

	if ( list_empty( binds->list ) )
	{
		// The list is empty, remove it to save some space
		// Handled by the destructor func
		tree_remove( tree, key );
	}
}

void input_remove_char_bind( uint32 key, keybind_func_t func )
{
	__input_remove_key_bind( key, func, BIND_CHAR );
}

void input_remove_key_up_bind( uint32 key, keybind_func_t func )
{
	__input_remove_key_bind( key, func, BIND_KEYUP );
}

void input_remove_key_down_bind( uint32 key, keybind_func_t func )
{
	__input_remove_key_bind( key, func, BIND_KEYDOWN );
}

void input_remove_key_bind( keybind_t* bind )
{
	__input_remove_key_bind( bind->key, bind->handler, bind->type );
}

static void __input_remove_mouse_bind( MOUSEBTN button, mousebind_func_t func, enum MBINDTYPE type )
{
	mousebind_t* bind;
	node_t *node, *tmp;
	list_t* bindlist = NULL;

	switch ( type )
	{
	case BIND_MOVE: bindlist = mouse_move_binds; break;
	case BIND_BTNUP: bindlist = mouse_up_binds; break;
	case BIND_BTNDOWN: bindlist = mouse_down_binds; break;
	}

	list_foreach_safe( bindlist, node, tmp )
	{
		bind = (mousebind_t*)node;
		if ( bind->button != button ) continue;
		if ( bind->handler != func ) continue;

		list_remove( bindlist, node );
		mem_free( bind );
	}
}

void input_remove_mouse_move_bind( mousebind_func_t func )
{
	__input_remove_mouse_bind( 0, func, BIND_MOVE );
}

void input_remove_mousebtn_up_bind( MOUSEBTN button, mousebind_func_t func )
{
	__input_remove_mouse_bind( button, func, BIND_BTNUP );
}

void input_remove_mousebtn_down_bind( MOUSEBTN button, mousebind_func_t func )
{
	__input_remove_mouse_bind( button, func, BIND_BTNDOWN );
}

void input_remove_mouse_bind( mousebind_t* bind )
{
	__input_remove_mouse_bind( bind->button, bind->handler, bind->type );
}

void input_set_mousebind_button( mousebind_t* bind, MOUSEBTN button )
{
	assert( bind != NULL );
	bind->button = button;
}

void input_set_mousebind_rect( mousebind_t* bind, rectangle_t* r )
{
	assert( bind != NULL );
	bind->bounds = *r;
}

void input_set_mousebind_func( mousebind_t* bind, mousebind_func_t func )
{
	assert( bind != NULL );
	bind->handler = func;
}

void input_set_mousebind_param( mousebind_t* bind, void* data )
{
	assert( bind != NULL );
	bind->userdata = data;
}

void input_block_keys( bool block )
{
	block_keys = block;
}

bool input_is_cursor_showing( void )
{
	return show_cursor;
}

void input_get_cursor_pos( uint16* x, uint16* y )
{
	*x = mouse_x;
	*y = mouse_y;
}

bool input_handle_keyboard_event( INPUT_EVENT type, uint32 key )
{
	list_t* list;
	node_t* node;
	input_event_t event;
	hookfunc* hook;

	list = input_hooks[type];

	if ( list_empty(list) ) return true;

	event.type = type;
	event.keyboard.key = key;

	list_foreach( list, node )
	{
		hook = (hookfunc*)node;

		if ( !hook->handler( &event ) )
			return false;
	}

	if ( block_keys ) return false;

	return true;
}

bool input_handle_mouse_event( INPUT_EVENT type, uint16 x, uint16 y, float wheel )
{
	list_t* list;
	node_t* node;
	input_event_t event;
	hookfunc* hook;

	list = input_hooks[type];

	if ( list_empty(list) ) return true;

	event.type = type;
	event.mouse.x = x;
	event.mouse.y = y;
	event.mouse.dx = x - mouse_x;
	event.mouse.dy = y - mouse_y;
	event.mouse.dwheel = wheel - mouse_wheel;

	mouse_x = x;
	mouse_y = y;
	mouse_wheel = wheel;

	list_foreach( list, node )
	{
		hook = (hookfunc*)node;

		if ( !hook->handler( &event ) )
			return false;
	}

	return true;
}

bool input_handle_char_bind( uint32 key )
{
	keybind_t* bind;
	node_t *tmp, *tmp2;
	bindnode* binds;
	bool ret = true;

	binds = (bindnode*)tree_find( char_binds, key );
	if ( !binds ) return true;

	list_foreach_safe( binds->list, tmp, tmp2 )
	{
		bind = (keybind_t*)tmp;
		if ( !bind->handler( key, bind->userdata ) )
		{
			ret = false;
		}
	}

	return ret;
}

bool input_handle_key_down_bind( uint32 key )
{
	keybind_t* bind;
	node_t *tmp, *tmp2;
	bindnode* binds;
	bool ret = true;

	binds = (bindnode*)tree_find( key_down_binds, key );
	if ( !binds ) return true;

	list_foreach_safe( binds->list, tmp, tmp2 )
	{
		bind = (keybind_t*)tmp;
		if ( !bind->handler( key, bind->userdata ) )
		{
			ret = false;
		}
	}

	return ret;
}

bool input_handle_key_up_bind( uint32 key )
{
	keybind_t* bind;
	node_t *tmp, *tmp2;
	bindnode* binds;
	bool ret = true;

	binds = (bindnode*)tree_find( key_up_binds, key );
	if ( !binds ) return true;

	list_foreach_safe( binds->list, tmp, tmp2 )
	{
		bind = (keybind_t*)tmp;
		if ( !bind->handler( key, bind->userdata ) )
		{
			ret = false;
		}
	}

	return ret;
}

bool input_handle_mouse_move_bind( uint16 x, uint16 y )
{
	mousebind_t* bind;
	node_t *tmp, *tmp2;
	bool ret = true;

	list_foreach_safe( mouse_move_binds, tmp, tmp2 )
	{
		bind = (mousebind_t*)tmp;

		if ( rect_is_point_in( &bind->bounds, x, y ) )
		{
			if ( !bind->handler( 0, x, y, bind->userdata ) )
			{
				ret = false;
			}
		}
	}

	return ret;
}

bool input_handle_mouse_up_bind( MOUSEBTN button, uint16 x, uint16 y )
{
	mousebind_t* bind;
	node_t *tmp, *tmp2;
	bool ret = true;

	list_foreach_safe( mouse_up_binds, tmp, tmp2 )
	{
		bind = (mousebind_t*)tmp;

		if ( bind->button == button && rect_is_point_in( &bind->bounds, x, y ) )
		{
			if ( !bind->handler( button, x, y, bind->userdata ) )
			{
				ret = false;
			}
		}
	}

	return ret;
}

bool input_handle_mouse_down_bind( MOUSEBTN button, uint16 x, uint16 y )
{
	mousebind_t* bind;
	node_t *tmp, *tmp2;
	bool ret = true;

	list_foreach_safe( mouse_down_binds, tmp, tmp2 )
	{
		bind = (mousebind_t*)tmp;

		if ( bind->button == button && rect_is_point_in( &bind->bounds, x, y ) )
		{
			if ( !bind->handler( button, x, y, bind->userdata ) )
			{
				ret = false;
			}
		}
	}

	return ret;
}
