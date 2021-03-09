#pragma once

#include <string>

#include "../dx11_renderer/renderer_utils.h"

//
// generic styles most widgets will have
//

// base style every style inherits
struct style
{
	std::string name;

	style();
	style(const std::string& name);

	virtual std::string to_string(uint16_t indent_amt = 1) const;
};

// widget text styling
struct text_style : style
{
	float size;			// text font size
	float ol_thckns;	// text outline thickness
	color clr;			// text font color
	color ol_clr;		// text outline color
	color bg_clr;		// text background color

	text_style();
	text_style(float font_size, const color& text_color);
	text_style(float font_size, float outline_thickness, const color& text_color, const color& outline_color, const color& background_color);

	// return string with styles required code
	std::string to_string(uint16_t indent_amt = 1) const; 
};

// widget border styling
struct border_style : style
{
	float thckns;		// border thickness
	float ol_thckns;	// border outline thickness
	color clr;			// border color
	color ol_clr;		// border outline color

	border_style();
	border_style(float thickness, const color& border_color);
	border_style(float thickness, float outline_thickness, const color& border_color, const color& outline_color);

	// print out the style's required code
	std::string to_string(uint16_t indent_amt = 1) const;
};

// multicolored rect, usually used for backgrounds or rects with gradients
struct mc_rect : style
{
	color tl_clr;	// top left color
	color tr_clr;	// top right color
	color bl_clr;	// bottom left color
	color br_clr;	// bottom right color

	mc_rect();
	mc_rect(const color& rect_color);
	mc_rect(const color& top_left, const color& top_right, const color& bottom_left, const color& bottom_right);

	// print out the style's required code
	std::string to_string(uint16_t indent_amt = 1) const;
};

//
// specific widget styles
//

struct checkbox_style : style
{
	text_style text;	 // checkbox text styling
	border_style border; // checkbox border styling
	mc_rect bg;			 // checkbox background styling
	mc_rect check;		 // checkbox check styling
	float gap;			 // checkbox check gap from border

	checkbox_style();
	checkbox_style(const text_style& text, const border_style& border, const mc_rect& bg, const mc_rect& check, float gap);

	std::string to_string(uint16_t indent_amt = 1) const;
};

struct button_style : style
{
	text_style text;	 // button text styling
	border_style border; // button border styling
	mc_rect bg;			 // button background style

	button_style();
	button_style(const text_style& text, const border_style& border, const mc_rect& bg);

	std::string to_string(uint16_t indent_amt = 1) const;
};

struct slider_style : style
{
	text_style text;	 // slider text styling
	border_style border; // slider border styling
	mc_rect bg;			 // slider background styling
	mc_rect clr;		 // slider bar color

	slider_style();
	slider_style(const text_style& text, const border_style& border, const mc_rect& bg, const mc_rect& clr);

	std::string to_string(uint16_t indent_amt = 1) const;
} inline default_slider_style{};

struct text_entry_style : style
{
	text_style text;	 // text entry text style
	text_style buf_text; // text entry buffer text style
	border_style border; // text entry border styling
	mc_rect bg;			 // text entry background styling

	text_entry_style();
	text_entry_style(const text_style& text, const border_style& border, const mc_rect& bg);
	text_entry_style(const text_style& text, const text_style& buf_text, const border_style& border, const mc_rect& bg);

	std::string to_string(uint16_t indent_amt = 1) const;
};

struct combo_box_style : style
{
	text_style text;	 // combo box text styling
	border_style border; // combo box border styling
	mc_rect bg;			 // combo box background styling

	combo_box_style();
	combo_box_style(const text_style& text, const border_style& border);
	combo_box_style(const text_style& text, const border_style& border, const mc_rect& bg);

	std::string to_string(uint16_t indent_amt = 1) const;
};

struct color_picker_style : style
{
	text_style text;		  // color picker text styling
	border_style border;	  // color picker border styling
	mc_rect bg;				  // color picker background styling
	border_style sldr_border; // color picker slider border styling
	float sldr_gap;			  // color picker slider gap between sliders

	color_picker_style();
	color_picker_style(const text_style& text, const border_style& border, const mc_rect& bg);
	color_picker_style(const text_style& text, const border_style& border, const mc_rect& bg, const border_style& sldr_border, float sldr_gap);

	std::string to_string(uint16_t indent_amt = 1) const;

	bool operator==(const color_picker_style&) const;
	bool operator!=(const color_picker_style&) const;
} inline default_color_picker_style{};

struct color_editor_style : style
{
	text_style text;
	border_style border;
	mc_rect bg;

	color_editor_style();
	color_editor_style(const text_style& text, const border_style& border, const mc_rect& bg);

	std::string to_string(uint16_t indent_amt = 1) const;
};