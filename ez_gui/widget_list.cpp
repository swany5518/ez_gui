#include "widget_list.h"

//
// owned widget definitions
//

//owned_widget::owned_widget(std::unique_ptr<widget> p_wdgt, uint32_t owned_styles_idx) :
//	p_wdgt(std::move(p_wdgt)),	
//	owned_styles_idx(owned_styles_idx)
//{ }

//
// widget list definitions
//

widget_list::widget_list() :
	top_left({0.f, 0.f}),
	size({5000.f, 5000.f}),
	background(),
	active(true),
	move_mode(false),
	widgets(),
	owned_widgets(),
	owned_styles(),
	input_msgs()
{ }

widget_list::widget_list(const vec2& top_left, const vec2& size) :
	top_left(top_left),
	size(size),
	background(),
	active(true),
	move_mode(false),
	widgets(),
	owned_widgets(),
	owned_styles(),
	input_msgs()
{ }

widget_list::widget_list(const vec2& top_left, const vec2& size, const mc_rect& background) :
	top_left(top_left),
	size(size),
	background(background),
	active(true),
	move_mode(false),
	widgets(),
	owned_widgets(),
	owned_styles(),
	input_msgs()
{ }

//widget_list::widget_list(const vec2& top_left, const vec2& size, const mc_rect& background, const std::vector<owned_widget>& owned_widgets, std::vector<std::unique_ptr<style>> owned_styles) :
//	top_left(top_left),
//	size(size),
//	background(background),
//	active(true),
//	move_mode(false),
//	widgets(),
//	owned_widgets(owned_widgets),
//	owned_styles(owned_styles),
//	input_msgs()
//{ 
//	for (auto& owned_widget : this->owned_widgets)
//	{
//		owned_widget.p_wdgt->set_style(owned_styles[owned_widget.owned_styles_idx].get());
//		widgets.push_back(owned_widget.p_wdgt.get());
//	}
//}

void widget_list::add_widget(widget* p_widget)
{
	widgets.push_back(p_widget);
}

void widget_list::add_widgets(widget* p_widgets, size_t count)
{
	for (auto i = 0u; i < count; ++i)
		widgets.push_back(&p_widgets[i]);
}

bool widget_list::remove_widget(widget* p_widget)
{
	auto position = std::find(widgets.begin(), widgets.end(), p_widget);

	if (position == widgets.end())
		return false;
	
	widgets.erase(position);

	return true;
}

bool widget_list::remove_widget(uint32_t idx)
{
	if (idx < 0 || idx >= widgets.size())
		return false;

	widgets.erase(widgets.begin() + idx);

	return true;
}

void widget_list::add_input_msg(const widget_input& msg)
{
	if (!active)
		return;

	input_msgs.push(msg);
}

bool widget_list::contains(const vec2& pos)
{
	return pos.x >= top_left.x
		&& pos.x <= top_left.x + size.x
		&& pos.y >= top_left.y
		&& pos.y <= top_left.y + size.y;
}

void widget_list::draw_widgets()
{
	if (!active)
		return;

	handle_next_input();

    widget::p_renderer->add_rect_filled_multicolor(top_left, size, background.tl_clr, background.tr_clr, background.bl_clr, background.br_clr);

	for (auto& widget : widgets)
		widget->draw();
}

void widget_list::handle_next_input()
{
	if (input_msgs.empty())
		return;
	
	auto& msg = input_msgs.front();

	if (msg.type == input_type::mouse_move)
	{
		if (move_mode)
		{
			for (auto widget : widgets)
				if (widget->mouse_info.clicking)
					widget->on_widget_move(msg.m_pos);
		}
		else
		{
			for (auto widget : widgets)
				if (widget->mouse_info.clicking)
					widget->on_drag(msg.m_pos);
		}
	}
	else if (msg.type == input_type::lbutton_down)
	{
		for (auto widget : widgets)
			if (widget->contains(msg.m_pos))
				widget->on_lbutton_down(msg.m_pos);
	}
	else if (msg.type == input_type::lbutton_up)
	{
		for (auto widget : widgets)
		{
			if (widget->contains(msg.m_pos))
				widget->on_lbutton_up(msg.m_pos);
			else
				widget->mouse_info.clicking = false;
		}
	}
	else if (msg.type == input_type::key_press)
	{
		for (auto widget : widgets)
			widget->on_key_down(msg.key);
	}
	
	input_msgs.pop();
}

void widget_list::set_active(bool active)
{
	this->active = active;
}

void widget_list::set_move_mode(bool mode)
{
	move_mode = mode;

	// if we want to move our widgets, we should deactivate them so they don't respond normally to mouse input
	for (auto widget : widgets)
		widget->active = !mode;
}

void widget_list::toggle_move_mode()
{
	move_mode = !move_mode;

	for (auto widget : widgets)
		widget->active = !move_mode;
}

bool widget_list::get_move_mode()
{
	return move_mode;
}

std::string widget_list::to_string()
{
	std::string widgets_str{};
	std::string styles_str{};
	std::vector<style*> styles_to_save{};

	for (auto w : widgets)
	{
		uint32_t style_idx = 0;
		auto iterator = std::find(styles_to_save.begin(), styles_to_save.end(), w->p_style);

		if (iterator == styles_to_save.end())
		{
			styles_to_save.push_back(w->p_style);
			style_idx = styles_to_save.size() - 1; // index is last added
			styles_str += (styles_str.empty() ? "\n" : ",\n") + w->p_style->to_string(3);
		}
		else
			style_idx = std::distance(styles_to_save.begin(), iterator);

		widgets_str += "\t\towned_widget\n\t\t{\nstd::make_unique<widget>(\n" + w->to_string(4) + ", " + std::to_string(style_idx) + "/*owned_widget style index*/\n\t\t}" +
			(w == widgets.back() ? "" : ",") + "\n";
	}
	
	return "widget_list\n{\n\t" +
		top_left.to_string() + ',' + size.to_string() + ",\n" +
		background.to_string(2) + ",\n\t" +
		"std::vector<owned_widget>\n\t{\n" +
		widgets_str +
		"\t},\n\t" +
		"std::vector<style>\n\t{\n" +
		styles_str +
		"\n\t}\n};";
}