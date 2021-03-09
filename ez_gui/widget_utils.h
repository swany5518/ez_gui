#pragma once

#include <cstdint>

#include "../dx11_renderer/renderer_utils.h"

//
// widget utilities
//

// enum class containing all possible types of widgets
enum class widget_type
{
	unknown,
	checkbox,
	button,
	slider,
	text_entry,
	combo_box,
	color_picker,
	color_editor
};

// enum class containing all mouse cursors 
enum class mouse_cursor
{
	arrow,
	text_input,
	resize_all,
	resize_ew,
	resize_ns,
	resize_nesw,
	resize_nwse,
	hand,
	not_allowed
};

// structure containing mouse state for widgets to know if they are clicked, clicking, dragging or hovered
struct mouse_state
{
	vec2 click_origin; // used for dragging widgets

	union
	{
		uint8_t state;	   // contains various mouse information

		struct
		{
			uint8_t clicked : 1;  // if a widget was clicked
			uint8_t clicking : 1; // if a widget is being clicked
			uint8_t hovering : 1; // if a widget is being hovered
			uint8_t reserved : 5;
		};
	};

	mouse_state(const vec2&);
};

// input type enum
enum class input_type
{
	mouse_move,
	lbutton_down,
	lbutton_up,
	key_press
};

// widget input messages that come from a mouse/kybd hook or wndproc
struct widget_input
{
	input_type type; // type of the input
	vec2 m_pos;		 // position of the mouse
	char key;		 // key that was pressed

	widget_input() = delete;
	widget_input(const vec2& m_pos);
	widget_input(char key);
	widget_input(input_type type, const vec2& m_pos);
};