#pragma once

#include <windowsx.h>
#include <type_traits>
#include <string>
#include <iostream>
#include <algorithm>

#include "../dx11_renderer/renderer.h"
#include "widget_styles.h"
#include "widget_utils.h"

// base widget inheritable, includes size and position and click/clicking/hovered state
struct widget
{
	vec2 top_left;			// top left corner of the widgets
	vec2 size;				// widget width and height
	std::wstring label;		// widget label 
	vec2 label_pos;			// widget label position relative to top left
	style* p_style;         // pointer to the widgets style struct
	mouse_state mouse_info; // widget mouse information (dragged, clicked, hovered, clicking, etc.)
	bool active;			// disable functionality of a widget

	inline static renderer* p_renderer;
	inline static void set_renderer(renderer* p_instance)
	{
		p_renderer = p_instance;
	}
	inline static void set_cursor(mouse_cursor cursor)
	{
		LPTSTR win32_cursor = IDC_ARROW;
		switch (cursor)
		{
		case mouse_cursor::arrow:
			win32_cursor = IDC_ARROW;
			break;
		case mouse_cursor::text_input:
			win32_cursor = IDC_IBEAM;
			break;
		case mouse_cursor::resize_all:
			win32_cursor = IDC_SIZEALL;
			break;
		case mouse_cursor::resize_ew:
			win32_cursor = IDC_SIZEWE;
			break;
		case mouse_cursor::resize_ns:
			win32_cursor = IDC_SIZENS;
			break;
		case mouse_cursor::resize_nesw:
			win32_cursor = IDC_SIZENESW;
			break;
		case mouse_cursor::resize_nwse:
			win32_cursor = IDC_SIZENWSE;
			break;
		case mouse_cursor::hand:
			win32_cursor = IDC_HAND;
			break;
		case mouse_cursor::not_allowed:
			win32_cursor = IDC_NO;
			break;
		}
		SetCursor(LoadCursor(NULL, win32_cursor));
	}

	widget() = delete;
	widget(const vec2& top_left, const vec2& size, const std::wstring& label);
	widget(const vec2& top_left, const vec2& size, const std::wstring& label, style* p_style);
	widget(const vec2& top_left, const vec2& size, const std::wstring& label, const vec2& label_pos);
	widget(const vec2& top_left, const vec2& size, const std::wstring& label, const vec2& label_pos, style* p_style);

	// set the widgets style pointer
	virtual void set_style(style* p_style);

	// determine if an widget contains a position
	bool contains(const vec2& pos);

	// get relative position from the top_left of the widget
	vec2 relative_position(const vec2& pos);

	// virtual drawing function
	virtual void draw();

	// should be called when the mouse is over the widget
	virtual void on_lbutton_down(const vec2& mouse_position);
	virtual void on_lbutton_up(const vec2& mouse_position);
	virtual void on_drag(const vec2& new_position);
	virtual void on_widget_move(const vec2& new_position);
	virtual void on_key_down(char key);
	virtual std::string to_string(uint16_t indent_amt);

	// virtual widget type function
	virtual widget_type get_type();
};

// simple checkbox widget for boolean values
struct checkbox : widget
{
	bool* value;

	checkbox() = delete;
	checkbox(const vec2& top_left, const vec2& size, const std::wstring& label, bool* value, checkbox_style* style);
	checkbox(const vec2& top_left, const vec2& size, const std::wstring& label, const vec2& label_pos, bool* value, checkbox_style* style);
	void on_lbutton_up(const vec2& mouse_position);
	void draw();
	widget_type get_type();

	std::string to_string(uint16_t indent_amt);
};

// simple button widget for event triggering
struct button : widget
{
	button() = delete;
	button(const vec2& top_left, const vec2& size, const std::wstring& label, button_style* style);
	button(const vec2& top_left, const vec2& size, const std::wstring& label, const vec2& label_pos, button_style* style);
	void draw();

	widget_type get_type();

	std::string to_string(uint16_t indent_amt);
};

// simple slider widget for floating point and integer types
template<typename Ty>
struct slider : widget
{
	Ty* value;
	Ty min_value;
	Ty max_value;

	slider(const vec2& top_left, const vec2& size, const std::wstring& label, Ty* value, Ty min, Ty max, slider_style* style) :
		widget(top_left, size, label, style),
		value(value),
		min_value(min),
		max_value(max)
	{ }

	slider(const vec2& top_left, const vec2& size, const std::wstring& label, Ty min, Ty max, slider_style* style) :
		widget(top_left, size, label, style),
		value(nullptr),
		min_value(min),
		max_value(max)
	{ }

	slider(const vec2& top_left, const vec2& size, const std::wstring& label, const vec2& label_pos, Ty* value, Ty min, Ty max, slider_style* style) :
		widget(top_left, size, label, label_pos, style),
		value(value),
		min_value(min),
		max_value(max)
	{ }

	Ty get_range()
	{
		return max_value - min_value;
	}

	void set_value_ptr(Ty* p_value)
	{
		value = p_value;
	}

	void on_lbutton_down(const vec2& mouse_position)
	{
		mouse_info.clicking = 1;
		mouse_info.clicked = 0;
		mouse_info.hovering = 0;
		mouse_info.click_origin = relative_position(mouse_position);

		if (active)
		{
			float ratio = std::clamp<float>(mouse_position.x - top_left.x, 0.f, size.x) / size.x;

			if (value != nullptr && std::is_integral_v<Ty>)
				*value = static_cast<Ty>(std::round(static_cast<float>(get_range()) * ratio + static_cast<float>(min_value)));
			else if (std::is_floating_point_v<Ty>)
				*value = static_cast<Ty>(get_range() * ratio + min_value);
		}
	}

	void on_drag(const vec2& new_position)
	{
		// ratio is the clamped offset of the new position relative to the top_left.x / the slider's size.x
		float ratio = std::clamp<float>(new_position.x - top_left.x, 0.f, size.x) / size.x;

		if (std::is_integral_v<Ty>)
			*value = static_cast<Ty>(std::round(static_cast<float>(get_range()) * ratio + static_cast<float>(min_value)));
		else if (std::is_floating_point_v<Ty>)
			*value = static_cast<Ty>(get_range() * ratio + min_value);
	}

	void draw()
	{
		auto style = static_cast<slider_style*>(p_style);
		// calculate the slider width
		auto scaled_width = static_cast<float>(*value - min_value) / static_cast<float>(get_range()) * size.x;

		// add the background color
		p_renderer->add_rect_filled_multicolor(top_left, size, style->bg.tl_clr, style->bg.tr_clr, style->bg.bl_clr, style->bg.br_clr);

		// add the slider
		p_renderer->add_rect_filled_multicolor(top_left, { scaled_width, size.y }, style->clr.tl_clr, style->clr.tr_clr, style->clr.bl_clr, style->clr.br_clr);

		// add the border
		p_renderer->add_outlined_frame(top_left, size, style->border.thckns, style->border.ol_thckns, style->border.clr, style->border.ol_clr);

		// add the label
		p_renderer->add_outlined_text_with_bg(top_left + label_pos, size, label, style->text.clr, style->text.ol_clr, style->text.bg_clr, style->text.size, style->text.ol_thckns);
	}

	widget_type get_type()
	{
		return widget_type::slider;
	}

	std::string to_string(uint16_t indent_amt)
	{
		std::string ty_type{};

		if (std::is_same_v<Ty, float>)
			ty_type = "float";
		else if (std::is_same_v<Ty, double>)
			ty_type = "double";
		else if (std::is_same_v<Ty, int>)
			ty_type = "int";
		else
			ty_type = "/*Ty TYPE HERE*/float";

		std::string tab_str(indent_amt, '\t');
		std::string brace_str(indent_amt > 0 ? indent_amt - 1 : 0, '\t');

		return brace_str + "slider<" + ty_type + ">\n" + brace_str + "{\n" +
			tab_str + top_left.to_string() + ", " + size.to_string() + ",\n" +
			tab_str + "L\"" + std::string(label.begin(), label.end()) + "\"," + label_pos.to_string() + ",\n" + 
			tab_str + "/*Ty VALUE PTR HERE*/nullptr," + std::to_string(min_value) + ',' + std::to_string(max_value) + ", nullptr\n" +
			brace_str + '}';
	}
};

// simple text entry
struct text_entry : widget
{
	std::wstring buffer;	  // string containing the text entry input
	uint32_t max_buffer_size; // max length of the buffer
	vec2 buffer_offset;	      // buffer position relative to top left

	text_entry(const vec2& top_left, const vec2& size, const std::wstring& label, text_entry_style* style, uint32_t max_buffer_size = 128u, const vec2& buffer_offset = {0.f, 0.f});
	text_entry(const vec2& top_left, const vec2& size, const std::wstring& label, const vec2& label_pos, text_entry_style* style, uint32_t max_buffer_size = 128u, const vec2& buffer_offset = { 0.f, 0.f });
	
	void on_key_down(char key);
	void draw();

	widget_type get_type();

	std::string to_string(uint16_t indent_amt);
};

// widget commonly used to place multiple grouped widgets inside
struct combo_box : widget
{
	combo_box(const vec2& top_left, const vec2& size, const std::wstring& label, combo_box_style* style);

	combo_box(const vec2& top_left, const vec2& size, const std::wstring& label, const vec2& label_pos, combo_box_style* style);

	void draw();

	widget_type get_type();

	std::string to_string(uint16_t indent_amt);
};

// edit a color with rgba sliders
struct color_picker : widget
{
	color* p_color;

	float border_padding;
	vec2 header_size;
	vec2 rgba_slider_pos[4];
	vec2 rgba_slider_size;
	int active_slider_index;
	inline static int instance_count;

	color_picker(const vec2& top_left, const vec2& size, const std::wstring& label, color_picker_style* style);
	color_picker(const vec2& top_left, const vec2& size, const std::wstring& label, color* p_color, color_picker_style* style);
	color_picker(const vec2& top_left, const vec2& size, const std::wstring& label, const vec2& label_pos, color* p_color, color_picker_style* style);

	// dynamically size positions and sizes based off the style, should be called on any style change
	void calc_pos_and_sizes();

	void set_style(style* p_new_style);
	void set_color_ptr(color* p_color);

	// check if a relative click is in any 4 of the rgba sliders, if so return the index of which one, else return -1
	int slider_click_index(const vec2& relative_mouse_pos) const;

	void on_lbutton_down(const vec2& mouse_position);
	void on_lbutton_up(const vec2& mouse_position);
	void on_drag(const vec2& new_position);
	void draw();

	widget_type get_type();

	std::string to_string(uint16_t indent_amt);
};