#pragma once

#include "widget_styles.h"

//
// style definitions
//

style::style() :
	name()
{ }

style::style(const std::string& name) :
	name(name)
{ }

std::string style::to_string(uint16_t indent_amt) const
{ 
	return "base style called";
}

//
// text_style definitions
//

text_style::text_style() :
	size(14.f),
	ol_thckns(1.f),
	clr({1.f, 1.f, 1.f, 1.f}),
	ol_clr({ 0.f, 0.f, 0.f, 1.f }),
	bg_clr({ 0.f, 0.f, 0.f, 0.f })
{ }

text_style::text_style(float font_size, const color& text_color) :
	size(font_size),
	ol_thckns(0.f),
	clr(text_color),
	ol_clr({0.f, 0.f, 0.f, 0.f}),
	bg_clr({0.f, 0.f, 0.f, 0.f})
{ }

text_style::text_style(float font_size, float outline_thickness, const color& text_color, const color& outline_color, const color& background_color) :
	size(font_size),
	ol_thckns(outline_thickness),
	clr(text_color),
	ol_clr(outline_color),
	bg_clr(background_color)
{ }

std::string text_style::to_string(uint16_t indent_amt) const
{
	std::string tab_str( indent_amt, '\t' );
	std::string brace_str(indent_amt > 0 ? indent_amt - 1 : 0, '\t');

	return brace_str + "text_style\n" +															//text_style
		brace_str +	"{\n" + tab_str +												//{
		std::to_string(size) + ", " + std::to_string(ol_thckns) + ",\n" + tab_str +	//		14.000000, 1.000000,
		clr.to_string() + ",\n" + tab_str +											//		{ 1.000000, 1.000000, 1.000000, 1.000000 },
		ol_clr.to_string() + ",\n" + tab_str +										//		{ 0.000000, 0.000000, 0.000000, 1.000000 },
		bg_clr.to_string() + "\n" + 												//		{ 0.000000, 0.000000, 0.000000, 0.000000 }
		brace_str + "}";															//},
}

//
// border style definitions
//

border_style::border_style() :
	thckns(2.f),
	ol_thckns(0.f),
	clr({0.f, 0.f, 0.f, 1.f}),
	ol_clr({0.f, 0.f, 0.f, 0.f})
{ }

border_style::border_style(float thickness, const color& border_color) :
	thckns(thickness),
	ol_thckns(0.f),
	clr(border_color),
	ol_clr({0.f, 0.f, 0.f, 0.f})
{ }

border_style::border_style(float thickness, float outline_thickness, const color& border_color, const color& outline_color) :
	thckns(thickness),
	ol_thckns(outline_thickness),
	clr(border_color),
	ol_clr(outline_color)
{ }

std::string border_style::to_string(uint16_t indent_amt) const
{
	std::string tab_str(indent_amt, '\t');
	std::string brace_str(indent_amt > 0 ? indent_amt - 1 : 0, '\t');

	return brace_str + "border_style\n" + brace_str +									//border_style
		"{\n" + tab_str +																//{
		std::to_string(thckns) + ", " + std::to_string(ol_thckns) + ",\n" + tab_str	+	//		2.000000, 0.000000,																									
		clr.to_string() + ",\n" + tab_str +												//		{ 0.000000, 0.000000, 0.000000, 1.000000 },
		ol_clr.to_string() + "\n" +														//		{ 0.000000, 0.000000, 0.000000, 0.000000 }
		brace_str + "}";																//},

}

//
// multicolored rectangle definitions
//

mc_rect::mc_rect() :
	tl_clr({0.f, 0.f, 0.f, 0.f}),
	tr_clr({0.f, 0.f, 0.f, 0.f}),
	bl_clr({0.f, 0.f, 0.f, 0.f}),
	br_clr({0.f, 0.f, 0.f, 0.f})
{ }

mc_rect::mc_rect(const color& rect_color) :
	tl_clr(rect_color),
	tr_clr(rect_color),
	bl_clr(rect_color),
	br_clr(rect_color)
{ }

mc_rect::mc_rect(const color& top_left, const color& top_right, const color& bottom_left, const color& bottom_right) :
	tl_clr(top_left),
	tr_clr(top_right),
	bl_clr(bottom_left),
	br_clr(bottom_right)
{ }

std::string mc_rect::to_string(uint16_t indent_amt) const
{
	std::string tab_str(indent_amt, '\t');
	std::string brace_str(indent_amt > 0 ? indent_amt - 1 : 0, '\t');

	return brace_str + "mc_rect\n" + 			//mc_rect
		brace_str + "{\n" + tab_str +			//{
		tl_clr.to_string() + ",\n" + tab_str +	//		{ 0.500000, 0.500000, 0.500000, 0.500000 },
		tr_clr.to_string() + ",\n" + tab_str +	//		{ 0.500000, 0.500000, 0.500000, 0.500000 },
		bl_clr.to_string() + ",\n" + tab_str +	//		{ 0.500000, 0.500000, 0.500000, 0.500000 },
		br_clr.to_string() + "\n" + 			//		{ 0.500000, 0.500000, 0.500000, 0.500000 }
		brace_str + "}";						//}
}

//
// checkbox style definitions
//

checkbox_style::checkbox_style() :
	text(),
	border(),
	bg(),
	check({1.f, 1.f, 1.f, 1.f}),
	gap(3.f)
{ }

checkbox_style::checkbox_style(const text_style& text, const border_style& border, const mc_rect& bg, const mc_rect& check, float gap) :
	text(text),
	border(border),
	bg(bg),
	check(check),
	gap(gap)
{ }

std::string checkbox_style::to_string(uint16_t indent_amt) const
{
	std::string tab_str(indent_amt, '\t');
	std::string brace_str(indent_amt > 0 ? indent_amt - 1 : 0, '\t');

	return brace_str + "checkbox_style\n" +
		brace_str + "{\n" + 
		tab_str + text.to_string(indent_amt + 1) + ",\n" +
		tab_str + border.to_string(indent_amt + 1) + ",\n" +
		tab_str + bg.to_string(indent_amt + 1) + ",\n" +
		tab_str + check.to_string(indent_amt + 1) + ",\n" +
		tab_str + std::to_string(gap) + "\n" + 
		brace_str + '}';
}

//
// button style definitions
//

button_style::button_style() :
	text(),
	border(),
	bg()
{ }

button_style::button_style(const text_style& text, const border_style& border, const mc_rect& bg) :
	text(text),
	border(border),
	bg(bg)
{ }

std::string button_style::to_string(uint16_t indent_amt) const
{
	std::string tab_str(indent_amt, '\t');
	std::string brace_str(indent_amt > 0 ? indent_amt - 1 : 0, '\t');

	return brace_str + "button_style\n" +
		brace_str + "{\n" +
		tab_str + text.to_string(indent_amt + 1) + ",\n" +
		tab_str + border.to_string(indent_amt + 1) + ",\n" +
		tab_str + bg.to_string(indent_amt + 1) + 
		'\n' + brace_str + '}';
}

//
// slider style definitions
//

slider_style::slider_style() :
	text(),
	border(),
	bg(),
	clr({ 1.f, 1.f, 1.f, 1.f })
{ }

slider_style::slider_style(const text_style& text, const border_style& border, const mc_rect& bg, const mc_rect& clr) :
	text(text),
	border(border),
	bg(bg),
	clr(clr)
{ }

std::string slider_style::to_string(uint16_t indent_amt) const
{
	std::string brace_str(indent_amt > 0 ? indent_amt - 1 : 0, '\t');

	return brace_str + "slider_style\n" + 
		brace_str + "{\n" +
		text.to_string(indent_amt + 1) + ",\n" +
		border.to_string(indent_amt + 1) + ",\n" +
		bg.to_string(indent_amt + 1) + ",\n" +
		clr.to_string(indent_amt + 1) + '\n' + 
		brace_str + '}';
}

//
// text entry style definitions
//

text_entry_style::text_entry_style() :
	text(),
	buf_text(),
	border(),
	bg({1.f, 1.f, 1.f, 1.f})
{ }

text_entry_style::text_entry_style(const text_style& text, const border_style& border, const mc_rect& bg) :
	text(text),
	buf_text(text),
	border(border),
	bg(bg)
{ }

text_entry_style::text_entry_style(const text_style& text, const text_style& buf_text, const border_style& border, const mc_rect& bg) :
	text(text),
	buf_text(buf_text),
	border(border),
	bg(bg)
{ }

std::string text_entry_style::to_string(uint16_t indent_amt) const
{
	std::string tab_str(indent_amt, '\t');
	std::string brace_str(indent_amt > 0 ? indent_amt - 1 : 0, '\t');

	return brace_str + "text_entry_style\n" +
		brace_str + "{\n" + 
		tab_str + text.to_string(indent_amt + 1) + ",\n" +
		tab_str + buf_text.to_string(indent_amt + 1) + ",\n" +
		tab_str + border.to_string(indent_amt + 1) + ",\n" +
		tab_str + bg.to_string(indent_amt + 1) + '\n' +
		brace_str + '}';
}

//
// combo box style definitions
//

combo_box_style::combo_box_style() :
	text(),
	border(),
	bg()
{ }

combo_box_style::combo_box_style(const text_style& text, const border_style& border) :
	text(text),
	border(border),
	bg()
{ }

combo_box_style::combo_box_style(const text_style& text, const border_style& border, const mc_rect& bg) :
	text(text),
	border(border),
	bg(bg)
{ }

std::string combo_box_style::to_string(uint16_t indent_amt) const
{
	std::string tab_str(indent_amt, '\t');
	std::string brace_str(indent_amt > 0 ? indent_amt - 1 : 0, '\t');

	return brace_str + "combo_box_style\n" + 
		brace_str + "{\n" +
		tab_str + text.to_string(indent_amt + 1) + ",\n" +
		tab_str + border.to_string(indent_amt + 1) + ",\n" +
		tab_str + bg.to_string(indent_amt + 1) + '\n' +
		brace_str + '}';
}

//
// color picker style definitions
//

color_picker_style::color_picker_style() :
	text(),
	border(), 
	bg(),
	sldr_border(),
	sldr_gap(3.f)
{ }

color_picker_style::color_picker_style(const text_style& text, const border_style& border, const mc_rect& bg) :
	text(text),
	border(border),
	bg(bg),
	sldr_border(),
	sldr_gap(3.f)
{ }

color_picker_style::color_picker_style(const text_style& text, const border_style& border, const mc_rect& bg, const border_style& sldr_border, float sldr_gap) :
	text(text),
	border(border),
	bg(bg),
	sldr_border(sldr_border),
	sldr_gap(sldr_gap)
{ }

std::string color_picker_style::to_string(uint16_t indent_amt) const
{
	std::string tab_str(indent_amt, '\t');
	std::string brace_str(indent_amt > 0 ? indent_amt - 1 : 0, '\t');

	return brace_str + "color_picker_style\n" +
		brace_str + "{\n" +
		text.to_string(indent_amt + 1) + ",\n" +
		border.to_string(indent_amt + 1) + ",\n" +
		bg.to_string(indent_amt + 1) + ",\n" +
		sldr_border.to_string(indent_amt + 1) + ",\n" +
		tab_str + std::to_string(sldr_gap) + '\n' +
		brace_str + '}';
}

bool color_picker_style::operator==(const color_picker_style& other) const
{
	return !memcmp(this, &other, sizeof(color_picker_style));
}

bool color_picker_style::operator!=(const color_picker_style& other) const
{
	return memcmp(this, &other, sizeof(color_picker_style));
}