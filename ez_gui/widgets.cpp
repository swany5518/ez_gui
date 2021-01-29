#include "widgets.h"

//
// widget definitions
//

widget::widget(const vec2& top_left, const vec2& size, const std::wstring& label) :
	top_left(top_left),
	size(size),
	label(label),
	label_pos({size.x, 0.f}),
	p_style(nullptr),
	mouse_info({ 0.f, 0.f }),
	active(true)
{ }

widget::widget(const vec2& top_left, const vec2& size, const std::wstring& label, style* p_style) :
	top_left(top_left),
	size(size),
	label(label),
	label_pos({ size.x, 0.f }),
	p_style(p_style),
	mouse_info({ 0.f, 0.f }),
	active(true)
{ }

widget::widget(const vec2& top_left, const vec2& size, const std::wstring& label, const vec2& label_pos) :
	top_left(top_left),
	size(size),
	label(label),
	label_pos(label_pos),
	p_style(nullptr),
	mouse_info({ 0.f, 0.f }),
	active(true)
{ }

widget::widget(const vec2& top_left, const vec2& size, const std::wstring& label, const vec2& label_pos, style* p_style) :
	top_left(top_left),
	size(size),
	label(label),
	label_pos(label_pos),
	p_style(p_style),
	mouse_info({ 0.f, 0.f }),
	active(true)
{ }

void widget::set_style(style* p_style)
{
	this->p_style = p_style;
}

bool widget::contains(const vec2& pos)
{
	return pos.x >= top_left.x
		&& pos.x <= top_left.x + size.x
		&& pos.y >= top_left.y
		&& pos.y <= top_left.y + size.y;
}

vec2 widget::relative_position(const vec2& pos)
{
	return pos - top_left;
}

void widget::draw() 
{ }

void widget::on_lbutton_down(const vec2& mouse_position)
{
	mouse_info.clicking = 1;
	mouse_info.clicked = 0;
	mouse_info.hovering = 0;
	mouse_info.click_origin = relative_position(mouse_position);
}

void widget::on_lbutton_up(const vec2& mouse_position)
{
	if (mouse_info.clicking)
	{
		mouse_info.clicking = 0;
		mouse_info.clicked = 1;
	}
}

void widget::on_drag(const vec2& new_position) 
{ 
	std::cout << "base on drag" << std::endl; 
}

void widget::on_widget_move(const vec2& new_position)
{
	top_left = new_position - mouse_info.click_origin;
}

void widget::on_key_down(char key) 
{ }

std::string widget::to_string(uint16_t indent_amt)
{
	return "";
}

widget_type widget::get_type()
{
	return widget_type::unknown;
}

//
// checkbox definitions
//

checkbox::checkbox(const vec2& top_left, const vec2& size, const std::wstring& label, bool* value, checkbox_style* style) :
	widget( top_left, size, label, style),
	value(value)
{ }

checkbox::checkbox(const vec2& top_left, const vec2& size, const std::wstring& label, const vec2& label_pos, bool* value, checkbox_style* style) :
	widget( top_left, size, label, label_pos, style),
	value(value)
{ }

void checkbox::on_lbutton_up(const vec2& mouse_position)
{
	if (mouse_info.clicking)
	{
		*value = !*value;
		mouse_info.clicked = true;
		mouse_info.clicking = false;;
	}
}

void checkbox::draw()
{
	auto style = static_cast<checkbox_style*>(p_style);
	// add checkbox background
	p_renderer->add_rect_filled_multicolor(top_left, size, style->bg.tl_clr, style->bg.tr_clr, style->bg.bl_clr, style->bg.br_clr);

	// add checkbox border
	p_renderer->add_outlined_frame(top_left, size, style->border.thckns, style->border.ol_thckns, style->border.clr, style->border.ol_clr);

	// add label
	p_renderer->add_outlined_text_with_bg(top_left + label_pos, size, label, style->text.clr, style->text.ol_clr, style->text.bg_clr, style->text.size, style->text.ol_thckns);

	// if value is true, add rect inside
	if (*value)
		p_renderer->add_rect_filled_multicolor(top_left + label_pos, size - (style->gap * 2.f), style->check.tl_clr, style->check.tr_clr, style->check.bl_clr , style->check.br_clr);
}

widget_type checkbox::get_type()
{
	return widget_type::checkbox;
}

std::string checkbox::to_string(uint16_t indent_amt)
{
	std::string tab_str(indent_amt, '\t');
	std::string brace_str(indent_amt > 0 ? indent_amt - 1 : 0, '\t');

	return brace_str + "checkbox" + '\n' + brace_str + "{\n" +
		tab_str + top_left.to_string() + ", " + size.to_string() + "\n" +
		tab_str + "L\"" + std::string(label.begin(), label.end()) + "\"," + label_pos.to_string() + "\n,/*VALUE PTR HERE*/nullptr, nullptr\n" +
		brace_str + '}';
}

//
// button definitions
//

button::button(const vec2& top_left, const vec2& size, const std::wstring& label, button_style* style) :
	widget(top_left, size, label, style)
{ }

button::button(const vec2& top_left, const vec2& size, const std::wstring& label, const vec2& label_pos, button_style* style) :
	widget(top_left, size, label, label_pos, style)
{ }

void button::draw()
{
	auto style = static_cast<button_style*>(p_style);
	// add button rect
	p_renderer->add_rect_filled_multicolor(top_left, size, style->bg.tl_clr, style->bg.tr_clr, style->bg.bl_clr, style->bg.br_clr);

	// add button border
	p_renderer->add_outlined_frame(top_left, size, style->border.thckns, style->border.ol_thckns, style->border.clr, style->border.ol_clr);

	// add button text
	p_renderer->add_outlined_text_with_bg(top_left + label_pos, size, label, style->text.clr, style->text.ol_clr, style->text.bg_clr, style->text.size, style->text.ol_thckns, text_align::center_middle);
}

widget_type button::get_type()
{
	return widget_type::button;
}

std::string button::to_string(uint16_t indent_amt)
{
	std::string tab_str(indent_amt, '\t');
	std::string brace_str(indent_amt > 0 ? indent_amt - 1 : 0, '\t');

	return brace_str + "button" + '\n' + brace_str + "{\n" +
		tab_str + top_left.to_string() + ", " + size.to_string() + "\n" +
		tab_str + "L\"" + std::string(label.begin(), label.end()) + "\"," + label_pos.to_string() + ", nullptr\n" +
		brace_str + '}';
}

//
// text entry definitions
//

text_entry::text_entry(const vec2& top_left, const vec2& size, const std::wstring& label, text_entry_style* style, uint32_t max_buffer_size, const vec2& buffer_offset) :
	widget(top_left, size, label, style),
	buffer(),
	buffer_offset(buffer_offset),
	max_buffer_size(max_buffer_size)
{
	buffer.reserve(max_buffer_size);
}

text_entry::text_entry(const vec2& top_left, const vec2& size, const std::wstring& label, const vec2& label_pos, text_entry_style* style, uint32_t max_buffer_size, const vec2& buffer_offset) :
	widget(top_left, size, label, label_pos, style),
	buffer(),
	buffer_offset(buffer_offset),
	max_buffer_size(max_buffer_size)
{
	buffer.reserve(max_buffer_size);
}

void text_entry::on_key_down(char key)
{
	switch (key)
	{
	case 0x08: // backspace
		if (buffer.size())
			buffer.pop_back();
		break;
	default:
		if (buffer.size() < max_buffer_size)
			buffer += key;
	}
}

void text_entry::draw()
{
	auto style = static_cast<text_entry_style*>(p_style);
	// add our text entry background
	p_renderer->add_rect_filled_multicolor(top_left, size, style->bg.tl_clr, style->bg.tr_clr, style->bg.bl_clr, style->bg.br_clr);

	// add our border
	p_renderer->add_outlined_frame(top_left, size, style->border.thckns, style->border.ol_thckns, style->border.clr, style->border.ol_clr);

	// add our label
	p_renderer->add_outlined_text_with_bg(top_left + label_pos, size, label, style->text.clr, style->text.ol_clr, style->text.bg_clr, style->text.size, style->text.ol_thckns);

	// add our buffered text
	p_renderer->add_outlined_text_with_bg(top_left + buffer_offset, size, buffer, style->buf_text.clr, style->buf_text.ol_clr, style->buf_text.bg_clr, style->buf_text.size, style->buf_text.ol_thckns);
}

widget_type text_entry::get_type()
{
	return widget_type::text_entry;
}

std::string text_entry::to_string(uint16_t indent_amt)
{
	std::string tab_str(indent_amt, '\t');
	std::string brace_str(indent_amt > 0 ? indent_amt - 1 : 0, '\t');

	return brace_str + "text_entry" + '\n' + brace_str + "{\n" +
		tab_str + top_left.to_string() + ", " + size.to_string() + "\n" +
		tab_str + "L\"" + std::string(label.begin(), label.end()) + "\"," + label_pos.to_string() + ", nullptr,\n" +
		tab_str + std::to_string(max_buffer_size) + ',' + buffer_offset.to_string() + '\n' +
		brace_str + '}';
}

//
// combo box definitions
//

combo_box::combo_box(const vec2& top_left, const vec2& size, const std::wstring& label, combo_box_style* style) :
	widget(top_left, size, label, {20.f, 0.f}, style)
{ }

combo_box::combo_box(const vec2& top_left, const vec2& size, const std::wstring& label, const vec2& label_pos, combo_box_style* style) :
	widget(top_left, size, label, label_pos, style)
{ }

void combo_box::draw()
{
	auto style = static_cast<combo_box_style*>(p_style);
	p_renderer->add_outlined_frame(top_left, size, style->border.thckns, style->border.ol_thckns, style->border.clr, style->border.ol_clr);
	p_renderer->add_outlined_text_with_bg({ top_left.x + label_pos.x, top_left.y - style->text.size / 2.f - style->text.ol_thckns + label_pos.y }, size, label, style->text.clr, style->text.ol_clr, style->text.bg_clr, style->text.size);
}

widget_type combo_box::get_type()
{
	return widget_type::combo_box;
}

std::string combo_box::to_string(uint16_t indent_amt)
{
	std::string tab_str(indent_amt, '\t');
	std::string brace_str(indent_amt > 0 ? indent_amt - 1 : 0, '\t');

	return brace_str + "combo_box" + '\n' + brace_str + "{\n" +
		tab_str + top_left.to_string() + ", " + size.to_string() + "\n" +
		tab_str + "L\"" + std::string(label.begin(), label.end()) + "\"," + label_pos.to_string() + ", nullptr\n" +
		brace_str + '}';
}

//
// color picker definitions
//

color_picker::color_picker(const vec2& top_left, const vec2& size, const std::wstring& label, color_picker_style* style) :
	widget(top_left, size, label, style),
	p_color(nullptr),
	active_slider_index(-1),
	rgba_slider_size()
{
	instance_count++;
	calc_pos_and_sizes();
}

color_picker::color_picker(const vec2& top_left, const vec2& size, const std::wstring& label, color* p_color, color_picker_style* style) :
	widget(top_left, size, label, style),
	p_color(p_color),
	active_slider_index(-1),
	rgba_slider_size()
{
	instance_count++;
	calc_pos_and_sizes();
}

color_picker::color_picker(const vec2& top_left, const vec2& size, const std::wstring& label, const vec2& label_pos, color* p_color, color_picker_style* style) :
	widget(top_left, size, label, label_pos, style),
	p_color(p_color),
	active_slider_index(-1),
	rgba_slider_size()
{
	instance_count++;
	calc_pos_and_sizes();
}

void color_picker::set_style(style* p_new_style)
{
	p_style = p_new_style;

	// recalculate position and sizes when new style applied
	calc_pos_and_sizes();
}

void color_picker::calc_pos_and_sizes()
{
	if (p_style == nullptr)
		return;

	auto style = static_cast<color_picker_style*>(p_style);
	// calculate header size and position (this is dependant on border thicknesses and text size)
	border_padding = 2.f * (style->border.thckns + style->border.ol_thckns);
	header_size = vec2{ size.x - border_padding * 2.f, p_renderer->measure_text(label, style->text.size).y };

	// calculate and store rgba slider positions and size (all relative to the top_left)
	rgba_slider_size = vec2{ header_size.x,  (size.y - header_size.y - border_padding * 3.f - style->sldr_gap * 3.f) / 4 };
	float slider_y_start_pos = header_size.y + border_padding * 2.f;

	for (auto i = 0u; i < 4; ++i)
		rgba_slider_pos[i] = vec2{ border_padding, slider_y_start_pos + i * (style->sldr_gap + rgba_slider_size.y) };
}

void color_picker::set_color_ptr(color* p_color)
{
	this->p_color = p_color;
}

int color_picker::slider_click_index(const vec2& relative_mouse_pos) const
{
	for (auto i = 0; i < 4; ++i)
	{
		if (relative_mouse_pos.y >= rgba_slider_pos[i].y
			&& relative_mouse_pos.y <= rgba_slider_pos[i].y + rgba_slider_size.y
			&& relative_mouse_pos.x >= rgba_slider_pos[i].x
			&& relative_mouse_pos.x <= rgba_slider_pos[i].x + rgba_slider_size.x)
			return i;
	}

	return -1;
}

void color_picker::on_lbutton_down(const vec2& mouse_position)
{
	mouse_info.clicking = true;
	mouse_info.clicked = false;
	mouse_info.hovering = false;
	mouse_info.click_origin = relative_position(mouse_position);

	if (active)
	{
		auto relative = mouse_position - top_left;
		active_slider_index = slider_click_index(relative);
		if (active_slider_index != -1)
		{
			float ratio = (relative.x - rgba_slider_pos[active_slider_index].x) / rgba_slider_size.x;
			(*p_color)[active_slider_index] = ratio;

		}
	}
}

void color_picker::on_lbutton_up(const vec2& mouse_position)
{
	active_slider_index = -1;
	mouse_info.clicked = true;
	mouse_info.clicking = false;
	mouse_info.hovering = true;
}

void color_picker::on_drag(const vec2& new_position)
{
	if (p_color == nullptr)
		return;

	if (active )
	{
		// check if relative click is inside any slider
		auto relative = new_position - top_left;

		if (active_slider_index == -1)
			active_slider_index = slider_click_index(relative);

		if (active_slider_index != -1)
		{
			float ratio = (relative.x - rgba_slider_pos[active_slider_index].x) / rgba_slider_size.x;
			(*p_color)[active_slider_index] = std::clamp<float>(ratio, 0.f, 1.f);

		}
	}
}

void color_picker::draw()
{
	if (p_color == nullptr)
		return;

	auto style = static_cast<color_picker_style*>(p_style);

	/*static auto old_style = *style;
	static int updated = 0;
	if (old_style != *style)
	{
		calc_pos_and_sizes();
		if (++updated == instance_count)
		{
			old_style = *style;
			updated = 0;
		}
	}*/

	// add background
	p_renderer->add_rect_filled_multicolor(top_left, size, style->bg.tl_clr, style->bg.tr_clr, style->bg.bl_clr, style->bg.br_clr);

	// add frame
	p_renderer->add_outlined_frame(top_left, size, style->border.thckns, style->border.ol_thckns, style->border.clr, style->border.ol_clr);

	// draw label
	p_renderer->add_outlined_text_with_bg(top_left + border_padding, header_size, label, style->text.clr, style->text.ol_clr, style->text.bg_clr, style->text.size, style->text.ol_thckns);

	// draw preview square (aligned to the right of the header) on top of white square, preview should be a square, same size as the text y size
	vec2 preview_size{ header_size.y / 2.f, header_size.y / 2.f };
	vec2 preview_pos{ top_left.x + size.x - border_padding - preview_size.x * 2.f, top_left.y + border_padding };

	p_renderer->add_rect_filled(preview_pos, preview_size, { 1.f, 1.f, 1.f, 1.f });
	p_renderer->add_rect_filled({ preview_pos.x + preview_size.x, preview_pos.y }, preview_size, { .75f, .75f, .75f, .75f });
	p_renderer->add_rect_filled({ preview_pos.x, preview_pos.y + preview_size.y }, preview_size, { .75f, .75f, .75f, .75f });
	p_renderer->add_rect_filled(preview_pos + preview_size, preview_size, { 1.f, 1.f, 1.f, 1.f });

	p_renderer->add_rect_filled(preview_pos, preview_size * 2.f, *p_color);

	// draw sliders, these should take up the rest of 
	static color rgba[3] =
	{
		{1.f, 0.f, 0.f, 1.f},
		{0.f, 1.f, 0.f, 1.f},
		{0.f, 0.f, 1.f, 1.f}
	};

	for (auto i = 0u; i < 4; ++i)
	{
		p_renderer->add_rect_filled(top_left + rgba_slider_pos[i], { rgba_slider_size.x * (*p_color)[i], rgba_slider_size.y }, (i < 3 ? rgba[i] : color{ .5f, .5f, .5f, (*p_color)[i] * .7f + .3f }));
		p_renderer->add_outlined_frame(top_left + rgba_slider_pos[i], rgba_slider_size, style->sldr_border.thckns, style->sldr_border.ol_thckns, style->sldr_border.clr, style->sldr_border.ol_clr);
	}
}

widget_type color_picker::get_type()
{
	return widget_type::color_picker;
}

std::string color_picker::to_string(uint16_t indent_amt)
{
	std::string tab_str(indent_amt, '\t');
	std::string brace_str(indent_amt > 0 ? indent_amt - 1 : 0, '\t');

	return brace_str + "color_picker" + '\n' + brace_str + "{\n" +
		tab_str + top_left.to_string() + ", " + size.to_string() + ",\n" +
		tab_str + "L\"" + std::string(label.begin(), label.end()) + "\", " + label_pos.to_string() + ", /*COLOR PTR HERE*/nullptr, nullptr\n" +
		brace_str + '}';
}