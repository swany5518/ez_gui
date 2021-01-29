#pragma once

#include "widget_utils.h"

//
// mouse state definitions
//

mouse_state::mouse_state(const vec2& click_origin) : 
	click_origin(click_origin),
	state(0)
{ }

//
// widget input definitions
//

widget_input::widget_input(const vec2& m_pos) :
	type(input_type::mouse_move),
	m_pos(m_pos),
	key(0)
{ }

widget_input::widget_input(char key) :
	type(input_type::key_press),
	m_pos({ 0.f, 0.f }),
	key(0)
{ }

widget_input::widget_input(input_type type, const vec2& m_pos) :
	type(type),
	m_pos(m_pos),
	key(0)
{ }