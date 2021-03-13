#pragma once

#include "renderer.h"

//
// [public] renderer utilities
//

void renderer::initialize(HWND hwnd, const color& render_target_color, const std::wstring& font_family)
{
	font = font_family;
	setup_device_and_swapchain(hwnd);
	setup_backbuffer();
	setup_viewport(hwnd);
	setup_shaders();
	setup_input_layout();
	setup_vertex_buffer();
	setup_blend_state();
	//setup_depth_stencil_state();
	//setup_rasterizer_state();
	setup_font_renderer(font);
	setup_screen_projection();
	setup_font_renderer(font);
	this->render_target_color = render_target_color;

	initialized = true;
}

void renderer::draw()
{
	if (!initialized)
		handle_error("draw - renderer is not initialized, did you call initialize()?");

	p_device_context->ClearRenderTargetView(p_backbuffer, &render_target_color.r);

	// only draw draw list vertices if size > 0
	if (default_draw_list.vertices.size())
	{
		// map our vertex buffer 
		D3D11_MAPPED_SUBRESOURCE mapped_resource;
		if (FAILED(p_device_context->Map(p_vertex_buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapped_resource)))
			return;

		// copy our vertex buffer and unmap
		memcpy(mapped_resource.pData, default_draw_list.vertices.data(), default_draw_list.vertices.size() * sizeof(vertex));
		p_device_context->Unmap(p_vertex_buffer, NULL);

		// iterate each batch and draw it with the respective primitive type
		size_t buffer_index = 0;
		for (auto& batch : default_draw_list.batch_list)
		{
			p_device_context->IASetPrimitiveTopology(batch.type);
			p_device_context->Draw(static_cast<UINT>(batch.vertex_count), static_cast<UINT>(buffer_index));
			buffer_index += batch.vertex_count;
		}
	}
	
	p_font_wrapper->Flush(p_device_context);
	p_font_wrapper->DrawGeometry(p_device_context, default_draw_list.p_text_geometry, nullptr, nullptr, FW1_RESTORESTATE);

	default_draw_list.clear();

	p_swapchain->Present(1, 0);
}

void renderer::set_render_target_color(const color& new_color)
{
	render_target_color = new_color;
}

void renderer::cleanup()
{
	p_device_context->ClearRenderTargetView(p_backbuffer, &render_target_color.r);
	p_swapchain->Present(1, 0);
	initialized = false;
}

//
// [public] low level geometry functions
//

void renderer::add_line(const vec2& start, const vec2& end, const color& color)
{
	vertex vertices[] =
	{
		vertex{start, color },
		vertex{end,   color },
	};

	add_vertices(vertices, sizeof(vertices) / sizeof(vertex), D3D_PRIMITIVE_TOPOLOGY_LINELIST);
}

void renderer::add_polyline(const vec2* points, size_t size, const color& color)
{
	std::vector<vertex> vertices;
	
	for (auto i = 0u; i < size; ++i)
		vertices.emplace_back(points[i], color);

	add_vertices(vertices.data(), vertices.size(), D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
}

void renderer::add_line_multicolor(const vec2& start, const vec2& end, const color& start_color, const color& end_color)
{
	vertex vertices[] =
	{
		{start, start_color },
		{end,   end_color   }
	};

	add_vertices(vertices, sizeof(vertices) / sizeof(vertex), D3D_PRIMITIVE_TOPOLOGY_LINELIST);
}

void renderer::add_rect_filled(const vec2& top_left, const vec2& size, const color& color)
{
	vertex vertices[] = 
	{
		{ top_left,                                       color },//{ -0.5f, 0.5f, 0.0f, red },   // top left
		{ vec2{top_left.x + size.x, top_left.y},		  color }, // top right
		{ vec2{top_left.x, top_left.y + size.y},          color },  // bottom_left
		{ vec2{top_left.x + size.x, top_left.y + size.y}, color }, // bottom_right
	};

	add_vertices(vertices, sizeof(vertices) / sizeof(vertex), D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void renderer::add_rect_filled_multicolor(const vec2& top_left, const vec2& size, const color& top_left_color, const color& top_right_color, const color& bottom_left_color, const color& bottom_right_color)
{
	vertex vertices[] =
	{
		{ top_left,                                       top_left_color},//{ -0.5f, 0.5f, 0.0f, red },   // top left
		{ {top_left.x + size.x, top_left.y},			  top_right_color }, // top right
		{ {top_left.x, top_left.y + size.y},          bottom_left_color },  // bottom_left
		{ {top_left.x + size.x, top_left.y + size.y}, bottom_right_color }, // bottom_right
	};

	add_vertices(vertices, sizeof(vertices) / sizeof(vertex), D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void renderer::add_triangle(const vec2& p1, const vec2& p2, const vec2& p3, const color& color)
{
	vertex vertices[] =
	{
		{ p1,  color},
		{ p2,  color},
		{ p3,  color},
		{ p1,  color},
	};

	add_vertices(vertices, sizeof(vertices) / sizeof(vertex), D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
}

void renderer::add_triangle_filled(const vec2& p1, const vec2& p2, const vec2& p3, const color& color)
{
	// need to arrange filled triangles in clockwise order
	vec2 first{};
	vec2 second{};
	vec2 third{};

	// choose the highest then left most first, then choose right most and highest second to ensure vertices are in clockwise order to get filled
	if (p1.higher_or_leftmost(p2) && p1.higher_or_leftmost(p3))
	{
		first = p1;
		bool p2_is_next = p2.rightmost_or_higher(p3);

		if (p2_is_next)
		{
			second = p2;
			third = p3;
		}
		else
		{
			second = p3;
			third = p2;
		}
	}
	else if (p2.higher_or_leftmost(p3))
	{
		first = p2;
		bool p1_is_next = p1.rightmost_or_higher(p3);
		if (p1_is_next)
		{
			second = p1;
			third = p3;
		}
		else
		{
			second = p3;
			third = p1;
		}
	}
	else
	{
		first = p3;
		bool p1_is_next = p1.rightmost_or_higher(p2);
		if (p1_is_next)
		{
			second = p1;
			third = p2;
		}
		else
		{
			second = p2;
			third = p1;
		}
	}

	vertex vertices[] =
	{
		{ first,  color},
		{ second, color},
		{ third,  color},
	};

	add_vertices(vertices, sizeof(vertices) / sizeof(vertex), D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void renderer::add_triangle_filled_multicolor(const vec2& p1, const vec2& p2, const vec2& p3, const color& p1_color, const color& p2_color, const color& p3_color)
{
	// need to arrange filled triangles in clockwise order
	vec2 first{};
	vec2 second{};
	vec2 third{};

	// choose the highest then left most first, then choose right most and highest second to ensure vertices are in clockwise order to get filled
	if (p1.higher_or_leftmost(p2) && p1.higher_or_leftmost(p3))
	{
		first = p1;
		bool p2_is_next = p2.rightmost_or_higher(p3);

		if (p2_is_next)
		{
			second = p2;
			third = p3;
		}
		else
		{
			second = p3;
			third = p2;
		}
	}
	else if (p2.higher_or_leftmost(p3))
	{
		first = p2;
		bool p1_is_next = p1.rightmost_or_higher(p3);
		if (p1_is_next)
		{
			second = p1;
			third = p3;
		}
		else
		{
			second = p3;
			third = p1;
		}
	}
	else
	{
		first = p3;
		bool p1_is_next = p1.rightmost_or_higher(p2);
		if (p1_is_next)
		{
			second = p1;
			third = p2;
		}
		else
		{
			second = p2;
			third = p1;
		}
	}

	vertex vertices[] =
	{
		{ first,  p1_color},
		{ second, p2_color},
		{ third,  p3_color},
	};

	add_vertices(vertices, sizeof(vertices) / sizeof(vertex), D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void renderer::add_circle(const vec2& middle, float radius, const color& color, size_t segments)
{
	// segment count must be between 4 and MAX_DRAW_LIST_VERTICES
	if (segments < 4 || segments > MAX_DRAW_LIST_VERTICES - 1)
		handle_error("add_circle - need at least 4 and less than MAX_DRAW_LIST_VERTICES");

	// store unit circle locations for circle resolutions(segments) to avoid calculating each add
	static std::unordered_map<size_t, std::vector<vec2>> positions_cache{};

	std::vector<vec2> positions{};

	auto cached_positions = positions_cache.find(segments);

	if (cached_positions == positions_cache.end())
	{
		for (auto i = 0u; i <= segments; ++i)
		{
			float theta = calc_theta(i, segments);
			positions.emplace_back( cos(theta), sin(theta));
		}

		positions_cache[segments] = positions;
	}

	else
		positions = cached_positions->second;
	
	// might be a good idea to cache these later on
	std::vector<vertex> vertices{};

	for (auto& position : positions)
		vertices.emplace_back(vec2{ position.x * radius + middle.x, position.y * radius + middle.y }, color);
	
	add_vertices(vertices.data(), vertices.size(), D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
}

void renderer::add_clipped_circle(const region& region, const vec2& middle, float radius, const color& color, size_t segments)
{
	// segment count must be between 4 and MAX_DRAW_LIST_VERTICES
	if (segments < 4 || segments > MAX_DRAW_LIST_VERTICES - 1)
		handle_error("add_circle - need at least 4 and less than MAX_DRAW_LIST_VERTICES");

	// store unit circle locations for circle resolutions(segments) to avoid calculating each add
	static std::unordered_map<size_t, std::vector<vec2>> positions_cache{};

	std::vector<vec2> positions{};

	auto cached_positions = positions_cache.find(segments);

	if (cached_positions == positions_cache.end())
	{
		for (auto i = 0u; i <= segments; ++i)
		{
			float theta = calc_theta(i, segments);
			positions.emplace_back(cos(theta), sin(theta));
		}

		positions_cache[segments] = positions;
	}

	else
		positions = cached_positions->second;

	// might be a good idea to cache these later on
	std::vector<vertex> vertices{};

	for (auto& position : positions)
	{
		vec2 abs{ position.x * radius + middle.x, position.y * radius + middle.y };

		vertices.emplace_back(abs, region.is_within(abs) ? color : colors::clear);
	}

	add_vertices(vertices.data(), vertices.size(), D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
}

void renderer::add_circle_filled(const vec2& middle, float radius, const color& color, size_t segments)
{
	// segment count must be between 4 and MAX_DRAW_LIST_VERTICES
	if (segments < 4 || segments > MAX_DRAW_LIST_VERTICES - 1)
		handle_error("add_circle_filled - need at least 4 and less than MAX_DRAW_LIST_VERTICES");

	// for each circle resolution(segments), we only need to calculate the vertex locations once to avoid calling calc_theta(), sin(), and cos() every call
	static std::unordered_map<size_t, std::vector<vec2>> positions_cache{};

	// declare our vertex list, these will be unit circle coords, multiply by radius and account for middle position to get correct size
	std::vector<vec2> positions{};

	// check for cached vertices
	auto cached_positions = positions_cache.find(segments);

	// if we do not have this circle resolution cached, we need to add it
	if (cached_positions == positions_cache.end())
	{
		// vertices 1, 2 and 3 need to be added first
		auto theta_1 = calc_theta(0, segments);
		auto theta_2 = calc_theta(1, segments);
		auto theta_3 = calc_theta(segments - 1, segments);
		
		positions.emplace_back(cos(theta_1), sin(theta_1));
		positions.emplace_back(cos(theta_2), sin(theta_2));
		positions.emplace_back(cos(theta_3), sin(theta_3));

		// for the 4th, 5th, ... nth vertex, its position is dependant on its nth number becuase of trianglestrips
		for (auto list_place = 4u; list_place <= segments; ++list_place)
		{
			// calculate where on the circle the vertex needs to calculated from vertex order for 8 segments the clockwise order is goes 1,2,4,6,8,7,5,3
			auto vertex_n = list_place % 2 != 0 ? segments - list_place / 2 : list_place / 2;
			auto theta_n = calc_theta(vertex_n, segments);
			positions.emplace_back(cos(theta_n), sin(theta_n));
		}

		positions_cache[segments] = positions;
	}
	else
		positions = cached_positions->second;

	std::vector<vertex> vertices{};

	for (auto& position : positions)
		vertices.emplace_back(vec2{ position.x * radius + middle.x, position.y * radius + middle.y }, color);

	add_vertices(vertices.data(), vertices.size(), D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

// 
// [public] intermediate shapes and model functions
//

void renderer::add_text(const vec2& top_left, const vec2& size, const std::wstring& text, const color& color, float font_size, text_align text_flags)
{
	if (text.empty())
		return;

	auto final_flags = static_cast<uint32_t>(text_flags) | FW1_NOFLUSH | FW1_NOWORDWRAP;

	FW1_RECTF rect{ top_left.x, top_left.y, top_left.x + size.x, top_left.y + size.y };
	p_font_wrapper->AnalyzeString(nullptr, text.c_str(), L"Consolas", font_size, &rect, color.to_hex_abgr(), final_flags, default_draw_list.p_text_geometry);
}

void renderer::add_text_with_bg(const vec2& top_left, const vec2& size, const std::wstring& text, const color& text_color, const color& bg_color, float font_size, text_align text_flags)
{
	if (text.empty())
		return;

	auto final_flags = static_cast<uint32_t>(text_flags) | FW1_NOFLUSH | FW1_NOWORDWRAP;

	// rect for formatting text
	FW1_RECTF rect{ top_left.x, top_left.y, top_left.x + size.x, top_left.y + size.y };

	// rect for drawing background behind text
	FW1_RECTF text_box_rect{ top_left.x, top_left.y, top_left.x, top_left.y };
	FW1_RECTF text_box = p_font_wrapper->MeasureString(text.c_str(), L"Consolas", font_size, &text_box_rect, final_flags);

	add_rect_filled({text_box.Left - 1.f, text_box.Top}, { text_box.Right - text_box.Left + 1.f, text_box.Bottom - text_box.Top }, bg_color);

	p_font_wrapper->AnalyzeString(nullptr, text.c_str(), L"Consolas", font_size, &rect, text_color.to_hex_abgr(), final_flags, default_draw_list.p_text_geometry);
}

void renderer::add_outlined_text(const vec2& top_left, const vec2& size, const std::wstring& text, const color& text_color, const color& outline_color, float font_size, float outline_size, text_align flags)
{
	// add shadows
	// -1,-1
	add_text(top_left - outline_size, size, text, outline_color, font_size, flags);
	// 0, -1
	add_text({ top_left.x, top_left.y - outline_size }, size, text, outline_color, font_size, flags);
	// 1, -1
	add_text({ top_left.x + outline_size, top_left.y - outline_size }, size, text, outline_color, font_size, flags);
	// 1, 0
	add_text({ top_left.x + outline_size, top_left.y }, size, text, outline_color, font_size, flags);
	// 1, 1
	add_text(top_left + outline_size, size, text, outline_color, font_size, flags);
	// 0, 1
	add_text({ top_left.x, top_left.y + outline_size }, size, text, outline_color, font_size, flags);
	// -1, 1
	add_text({ top_left.x - outline_size, top_left.y + outline_size }, size, text, outline_color, font_size, flags);
	// -1, 0
	add_text({ top_left.x - outline_size, top_left.y }, size, text, outline_color, font_size, flags);

	// add actual text
	add_text(top_left, size, text, text_color, font_size, flags);
}

void renderer::add_outlined_text_with_bg(const vec2& top_left, const vec2& size, const std::wstring& text, const color& text_color, const color& outline_color, const color& bg_color, float font_size, float outline_size, text_align text_flags)
{
	if (text.empty())
		return;

	auto final_flags = static_cast<uint32_t>(text_flags) | FW1_NOFLUSH | FW1_NOWORDWRAP;

	// rect for formatting text
	FW1_RECTF rect{ top_left.x, top_left.y, top_left.x + size.x, top_left.y + size.y };

	// rect for drawing background behind text
	FW1_RECTF text_box_rect{ top_left.x, top_left.y, top_left.x, top_left.y };
	FW1_RECTF text_box = p_font_wrapper->MeasureString(text.c_str(), L"Consolas", font_size, &text_box_rect, final_flags);

	add_rect_filled({ text_box.Left - outline_size, text_box.Top }, { text_box.Right - text_box.Left + outline_size + 1.f, text_box.Bottom - text_box.Top }, bg_color);

	add_outlined_text(top_left, size, text, text_color, outline_color, font_size, outline_size, text_flags);
	//p_font_wrapper->AnalyzeString(nullptr, text.c_str(), L"Consolas", font_size, &rect, text_color.to_hex_abgr(), final_flags, default_draw_list.p_text_geometry);

}

void renderer::add_frame(const vec2& top_left, const vec2& size, float thickness, const color& frame_color)
{
	// top left -> top right - thick
	add_rect_filled(top_left, { size.x - thickness, thickness }, frame_color);
	// top left -> bottom left - thick
	add_rect_filled({ top_left.x, top_left.y + thickness }, { thickness, size.y - thickness }, frame_color);
	// top right -> bottom right
	add_rect_filled({top_left.x + size.x - thickness, top_left.y}, {thickness, size.y - thickness}, frame_color);
	// bottom left -> bottom right
	add_rect_filled({ top_left.x + thickness, top_left.y + size.y - thickness }, {size.x - thickness, thickness}, frame_color);
}

void renderer::add_wire_frame(const vec2& top_left, const vec2& size, const color& frame_color)
{
	vec2 points[] =
	{
		{top_left.x - 1, top_left.y - 1},
		{top_left.x + size.x, top_left.y},
		{top_left.x + size.x, top_left.y + size.y},
		{top_left.x, top_left.y + size.y},
		top_left
	};

	add_polyline(points, sizeof(points) / sizeof(vec2), frame_color);
}

void renderer::add_3d_wire_frame(const vec2& top_left, const vec3& size, const color& frame_color)
{
	vec2 points[] =
	{
		{top_left.x, top_left.y + size.y},						// bottom left
		top_left,												// top left
		{top_left.x + size.x, top_left.y},						// top right
		{top_left.x + size.x, top_left.y + size.y},				// bottom right
		{top_left.x, top_left.y + size.y},						// bottom left
		{top_left.x - size.z, top_left.y + size.y - size.z},	// bottom right - z
		{top_left.x - size.z, top_left.y - size.z},				// top_left - z
		{top_left.x + size.x - size.z, top_left.y - size.z },	// top right - z
		{top_left.x + size.x, top_left.y},						// top right
		top_left,												// top left
		{top_left.x - size.z, top_left.y - size.z},				// top_left - z
	};

	add_polyline(points, sizeof(points) / sizeof(vec2), frame_color);
}

void renderer::add_outlined_frame(const vec2& top_left, const vec2& size, float thickness, float outline_thickness, const color& frame_color, const color& outline_color)
{
	// frame shadow
	add_frame(top_left - outline_thickness, size + (outline_thickness * 2), thickness + outline_thickness * 2, outline_color);

	// actual frame
	add_frame(top_left, size, thickness, frame_color);
}

vec2 renderer::measure_text(const std::wstring& text, float text_size)
{
	FW1_RECTF in{};
	auto rect = p_font_wrapper->MeasureString(text.c_str(), font.c_str(), text_size, &in, FW1_LEFT | FW1_NOWORDWRAP);
	return { rect.Right - rect.Left, rect.Bottom - rect.Top };
}

//
// [public] constructors
//

renderer::renderer() :
	initialized(false),
	p_swapchain(nullptr),
	p_device(nullptr),
	p_device_context(nullptr),
	p_backbuffer(nullptr),
	p_layout(nullptr),
	p_blend_state(nullptr),
	p_depth_stencil(nullptr),
	p_vertex_shader(nullptr),
	p_pixel_shader(nullptr),
	p_vertex_buffer(nullptr),
	p_screen_projection_buffer(nullptr),
	p_font_factory(nullptr),
	p_font_wrapper(nullptr),
	default_draw_list(),
	screen_projection(),
	render_target_color()
{ }

// 
// [private] directx initialization functions
//

void renderer::setup_device_and_swapchain(HWND hwnd)
{
	RECT wnd_size{};
	if (!GetClientRect(hwnd, &wnd_size))
		handle_error("setup_device_and_swapchain - failed to get hwnd window size");

	DXGI_SWAP_CHAIN_DESC swapchain_desc;
	ZeroMemory(&swapchain_desc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapchain_desc.BufferCount = 1;                                   // one back buffer
	swapchain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;    // use 32-bit color
	swapchain_desc.BufferDesc.Width = wnd_size.right - wnd_size.left; // set the back buffer width
	swapchain_desc.BufferDesc.Height = wnd_size.bottom - wnd_size.top;// set the back buffer height
	swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;     // how swap chain is to be used
	swapchain_desc.OutputWindow = hwnd;                               // the window to be used
	swapchain_desc.SampleDesc.Count = 4;                              // how many multisamples
	swapchain_desc.Windowed = TRUE;                                   // windowed/full-screen mode
	swapchain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;    // allow full-screen switching
	swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapchain_desc.BufferDesc.RefreshRate.Numerator = 60;
	swapchain_desc.BufferDesc.RefreshRate.Numerator = 1;

	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, &swapchain_desc, &p_swapchain, &p_device, NULL, &p_device_context)))
		handle_error("setup_device_and_swapchain - failed to create device and swapchain");

}

void renderer::setup_backbuffer()
{
	ID3D11Texture2D* p_backbuffer_texture = nullptr;

	if (FAILED(p_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&p_backbuffer_texture)))
		handle_error("setup_backbuffer - failed to get backbuffer texture");

	if (FAILED(p_device->CreateRenderTargetView(p_backbuffer_texture, NULL, &p_backbuffer)))
		handle_error("setup_backbuffer - failed to create render target view");

	p_backbuffer_texture->Release();

	p_device_context->OMSetRenderTargets(1, &p_backbuffer, NULL);
}

void renderer::setup_viewport(HWND hwnd)
{
	RECT wnd_size{};
	if (!GetClientRect(hwnd, &wnd_size))
		handle_error("setup_device_and_swapchain - failed to get hwnd window size");

	// set the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(wnd_size.right - wnd_size.left);
	viewport.Height = static_cast<float>(wnd_size.bottom - wnd_size.top);
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;

	p_device_context->RSSetViewports(1, &viewport);
}

void renderer::setup_shaders()
{
	if (FAILED(p_device->CreateVertexShader(shaders::vertex, sizeof(shaders::vertex), NULL, &p_vertex_shader)))
		handle_error("renderer - failed to create vertex shader");

	if (FAILED(p_device->CreatePixelShader(shaders::pixel, sizeof(shaders::pixel), NULL, &p_pixel_shader)))
		handle_error("renderer - failed to create pixel shader");

	// set the shader objects
	p_device_context->VSSetShader(p_vertex_shader, 0, 0);
	p_device_context->PSSetShader(p_pixel_shader, 0, 0);
}

void renderer::setup_input_layout()
{
	// create the input layout object
	D3D11_INPUT_ELEMENT_DESC input_elem_desc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	if (FAILED(p_device->CreateInputLayout(input_elem_desc, 2, shaders::vertex, sizeof(shaders::vertex), &p_layout)))
		handle_error("renderer - failed to create input layout");

	// this use to be after we set constant projection buffer
	p_device_context->IASetInputLayout(p_layout);
}

void renderer::setup_vertex_buffer()
{
	// create the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;							// write access access by CPU and GPU
	bd.ByteWidth = sizeof(vertex) * MAX_DRAW_LIST_VERTICES; // size is the VERTEX struct * max vertices
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;				// use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;				// allow CPU to write in buffer

	if (FAILED(p_device->CreateBuffer(&bd, NULL, &p_vertex_buffer)))	// create the buffer
		handle_error("renderer - failed to create vertex buffer");

	UINT stride = sizeof(vertex);
	UINT offset = 0;
	p_device_context->IASetVertexBuffers(0, 1, &p_vertex_buffer, &stride, &offset);
}

void renderer::setup_blend_state()
{
	D3D11_BLEND_DESC blend_desc{};
	ZeroMemory(&blend_desc, sizeof(blend_desc));
	blend_desc.RenderTarget->BlendEnable = TRUE;
	blend_desc.RenderTarget->SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_desc.RenderTarget->DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blend_desc.RenderTarget->BlendOp = D3D11_BLEND_OP_ADD;

	blend_desc.RenderTarget->SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget->DestBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget->BlendOpAlpha = D3D11_BLEND_OP_ADD;

	blend_desc.RenderTarget->RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	p_device->CreateBlendState(&blend_desc, &p_blend_state);
	p_device_context->OMSetBlendState(p_blend_state, nullptr, 0xFFFFFFFF);
}

void renderer::setup_depth_stencil_state()
{
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depth_stencil_desc.DepthEnable = false;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
	depth_stencil_desc.StencilEnable = false;
	depth_stencil_desc.FrontFace.StencilFailOp = depth_stencil_desc.FrontFace.StencilDepthFailOp = depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.BackFace = depth_stencil_desc.FrontFace;

	p_device->CreateDepthStencilState(&depth_stencil_desc, &p_depth_stencil);
	p_device_context->OMSetDepthStencilState(p_depth_stencil, 0);

}

void renderer::setup_rasterizer_state()
{
	ID3D11RasterizerState* p_rasterizer_state;
	D3D11_RASTERIZER_DESC rasterizer_desc;
	ZeroMemory(&rasterizer_desc, sizeof(rasterizer_desc));
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.ScissorEnable = false;
	rasterizer_desc.DepthClipEnable = true;

	if (FAILED(p_device->CreateRasterizerState(&rasterizer_desc, &p_rasterizer_state)))
		handle_error("setup_rasterizer_state - failed to create rasterizer state");

	p_device_context->RSSetState(p_rasterizer_state);

	p_rasterizer_state->Release();
}

void renderer::setup_screen_projection()
{
	// create the screen projection buffer
	D3D11_BUFFER_DESC projection_buffer_desc;
	ZeroMemory(&projection_buffer_desc, sizeof(projection_buffer_desc));
	projection_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	projection_buffer_desc.ByteWidth = sizeof(DirectX::XMMATRIX);
	projection_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	projection_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	projection_buffer_desc.MiscFlags = 0;

	if (FAILED(p_device->CreateBuffer(&projection_buffer_desc, nullptr, &p_screen_projection_buffer)))
		handle_error("renderer - failed to create screen projection buffer");

	D3D11_VIEWPORT viewport{};
	UINT num_viewports_test = 1;
	p_device_context->RSGetViewports(&num_viewports_test, &viewport);

	// calculate the screen projection from the buffer
	screen_projection = DirectX::XMMatrixOrthographicOffCenterLH(viewport.TopLeftX, viewport.Width, viewport.Height, viewport.TopLeftY, viewport.MinDepth, viewport.MaxDepth);

	// map the screen projection in
	D3D11_MAPPED_SUBRESOURCE projection_map_subresource;
	if (FAILED(p_device_context->Map(p_screen_projection_buffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &projection_map_subresource)))
		handle_error("renderer - failed to map screen projection buffer");

	memcpy(projection_map_subresource.pData, &screen_projection, sizeof(DirectX::XMMATRIX));

	p_device_context->Unmap(p_screen_projection_buffer, NULL);

	// set the screen projection buffer constant
	p_device_context->VSSetConstantBuffers(0, 1, &p_screen_projection_buffer);
}

void renderer::setup_font_renderer(std::wstring font)
{
	if (FAILED(FW1CreateFactory(FW1_VERSION, &p_font_factory)))
		handle_error("renderer - failed to create font factory");

	if (FAILED(default_draw_list.init_text_geometry(p_font_factory)))
		handle_error("renderer - failed to init text geometry");

	if (FAILED(p_font_factory->CreateFontWrapper(p_device, font.c_str(), &p_font_wrapper)))
		handle_error("renderer - failed to create font wrapper");

	p_font_wrapper->DrawString(p_device_context, L"", 0.0f, 0.0f, 0.0f, 0xff000000, FW1_RESTORESTATE | FW1_NOFLUSH);
}

//
// [private] internal helper functions
//

void renderer::add_vertex(const vertex& vertex, const D3D_PRIMITIVE_TOPOLOGY type)
{
	if (default_draw_list.vertices.size() >= MAX_DRAW_LIST_VERTICES)
	{
		handle_error("vertex buffer limit reached, did you forget to call renderer::draw()?");
		draw();
	}
	if (default_draw_list.batch_list.empty() || default_draw_list.batch_list.back().type != type)
		default_draw_list.batch_list.emplace_back(type, 1);
	else
		default_draw_list.batch_list.back().vertex_count++;

	default_draw_list.vertices.push_back(vertex);
}

void renderer::add_vertices(vertex* p_vertices, const size_t vertex_count, const D3D_PRIMITIVE_TOPOLOGY type)
{
	if (vertex_count > MAX_DRAW_LIST_VERTICES)
		handle_error("add_vertices - trying to add too many vertices");

	if (default_draw_list.vertices.size() + vertex_count >= MAX_DRAW_LIST_VERTICES)
	{
		handle_error("vertex buffer limit reached, did you forget to call renderer::draw()?");
		draw();
	}

	if (default_draw_list.batch_list.empty() || default_draw_list.batch_list.back().type != type)
		default_draw_list.batch_list.emplace_back(type, vertex_count);
	else
		default_draw_list.batch_list.back().vertex_count += vertex_count;

	// store old size for our memcpy destination address
	auto old_size = default_draw_list.vertices.size();

	// resize our vertex buffer so we can copy the new vertices in
	default_draw_list.vertices.resize(old_size + vertex_count);
	memcpy(&default_draw_list.vertices[old_size], p_vertices, vertex_count * sizeof(vertex));

	// we need to add a vertex between these primitives otherwise it will connect them
	if (type == D3D_PRIMITIVE_TOPOLOGY_LINESTRIP ||
		type == D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ ||
		type == D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP ||
		type == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ)
		add_vertex({}, D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED);
}

renderer::~renderer()
{
	if (p_swapchain)
		p_swapchain->SetFullscreenState(FALSE, NULL);

	safe_release(p_swapchain);
	safe_release(p_device);
	safe_release(p_device_context);
	safe_release(p_backbuffer);
	safe_release(p_blend_state);
	safe_release(p_layout);
	safe_release(p_vertex_shader);
	safe_release(p_pixel_shader);
	safe_release(p_vertex_buffer);
	safe_release(p_screen_projection_buffer);
	safe_release(p_font_factory);
	safe_release(p_font_wrapper);
}

void renderer::handle_error(const char* message)
{
	MessageBoxA(NULL, message, "rendering error", MB_ICONERROR);
	exit(-1);
}