#pragma once

#pragma warning(disable:6387)
#pragma warning(disable:26812)
#pragma warning(disable:26495)

#include <cstdint>
#include <cstddef>
#include <cmath>
#include <vector>
#include <string>
#include <unordered_map>
#include <cassert>
//#include <d3dx11.h>
#include <DirectXMath.h>

#pragma comment (lib, "d3d11.lib")

#include "renderer_utils.h"

// holds a vertex buffer and a batch list that our renderer will use
class draw_list
{
	friend class renderer;
public:
	draw_list() :
		vertices(),
		batch_list(),
		p_text_geometry(nullptr)
	{}

	void clear()
	{
		vertices.clear();
		batch_list.clear();
		p_text_geometry->Clear();
	}

	HRESULT init_text_geometry(IFW1Factory* font_factory)
	{
		return font_factory->CreateTextGeometry(&p_text_geometry);
	}

	~draw_list()
	{
		safe_release(p_text_geometry);
	}

private:
	std::vector<vertex> vertices;
	std::vector<batch> batch_list;
	IFW1TextGeometry* p_text_geometry;
};

// provides a directx api to easily render primitives
class renderer
{
public:
	renderer();
	~renderer();

	// submits the draw list to the gpu for rendering
	void draw();

	// initialize renderer onto a window 
	void initialize(HWND hwnd, const color& render_target_color = {}, const std::wstring& font_family = L"Consolas");

	// set the rendering target background color
	void set_render_target_color(const color& new_color);

	// cleanup renderer
	void cleanup();

	// adds a colored line from start to end
	void add_line(const vec2& start, const vec2& end, const color& color);
	
	// adds a connected line from passed in points
	void add_polyline(const vec2* points, size_t size, const color& color);

	// adds a multicolored line from start to end
	void add_line_multicolor(const vec2& start, const vec2& end, const color& start_color, const color& end_color);
	
	// add a rectangle 
	void add_rect_filled(const vec2& top_left, const vec2& size, const color& color);
	
	// add a multicolored rectangle
	void add_rect_filled_multicolor(const vec2& top_left, const vec2& size, const color& top_left_color, const color& top_right_color, const color& bottom_left_color, const color& bottom_right_color);
	
	// add triangle
	void add_triangle(const vec2& p1, const vec2& p2, const vec2& p3, const color& color);
	
	// add a filled triangle, vertices get arranged to clockwise order
	void add_triangle_filled(const vec2& p1, const vec2& p2, const vec2& p3, const color& color);
	
	// add a multicolored triangle, vertices get arranged to clockwise order so colors might not be on expected points
	void add_triangle_filled_multicolor(const vec2& p1, const vec2& p2, const vec2& p3, const color& p1_color, const color& p2_color, const color& p3_color);
	
	// add a circle, more segments means smoother looking circle
	void add_circle(const vec2& middle, float radius, const color& color, size_t segments);
	
	// add a filled circle
	void add_circle_filled(const vec2& middle, float radius, const color& box_color, size_t segments);

	// add a thin frame made out of rects
	void add_frame(const vec2& top_left, const vec2& size, float thickness, const color& frame_color);

	// add a wire frame (constructs frame from lines instead of rects)
	void add_wire_frame(const vec2& top_left, const vec2& size, const color& frame_color);

	// add a 3d wire frame
	void add_3d_wire_frame(const vec2& top_left, const vec3& size, const color& frame_color);

	// add a frame with a shadow behind it
	void add_outlined_frame(const vec2& top_left, const vec2& size, float thickness, float outline_thickness, const color& color_, const color& outline_color);

	// add text, top_left and size are for the text bounding box, see text_flags enum for flags
	void add_text(const vec2& top_left, const vec2& size, const std::wstring& text, const color& color, float font_size, text_align flags = text_align::left_top);

	// add text with background around the smallest rect containing the text
	void add_text_with_bg(const vec2& top_left, const vec2& size, const std::wstring& text, const color& text_color, const color& bg_color, float font_size, text_align text_flags = text_align::left_top);

	// add outlined text, this is not done in a good way so it could affect performance
	void add_outlined_text(const vec2& top_left, const vec2& size, const std::wstring& text, const color& text_color, const color& outline_color, float font_size, float outline_size = 1.f, text_align text_flags = text_align::left_top);

	// add outlined text with a background, this is not done in a good way so it could affect performance
	void add_outlined_text_with_bg(const vec2& top_left, const vec2& size, const std::wstring& text, const color& text_color, const color& outline_color, const color& bg_color, float font_size, float shadow_size = 1.f, text_align text_flags = text_align::left_top);

	// see how much space text will take up, returns the height and width text will take up
	vec2 measure_text(const std::wstring& text, float text_size);

private:
	bool initialized;

	IDXGISwapChain*			 p_swapchain;      // swapchain ptr
	ID3D11Device*			 p_device;         // d3d device interface ptr
	ID3D11DeviceContext*	 p_device_context; // d3d device context ptr
	ID3D11RenderTargetView*  p_backbuffer;     // backbuffer ptr
	ID3D11InputLayout*		 p_layout;         // layout ptr
	ID3D11BlendState*	     p_blend_state;    // blend state ptr
	ID3D11DepthStencilState* p_depth_stencil;  // depth stencil ptr
	ID3D11VertexShader*		 p_vertex_shader;  // vertex shader ptr
	ID3D11PixelShader*		 p_pixel_shader;   // pixel shader ptr
	ID3D11Buffer*			 p_vertex_buffer;  // vertex buffer ptr
	ID3D11Buffer*			 p_screen_projection_buffer; // screen projection buffer ptr
							 
	IFW1Factory*			 p_font_factory;   // font factory ptr
	IFW1FontWrapper*		 p_font_wrapper;   // font wrapper ptr

	draw_list default_draw_list; // default draw list, we should only need 1 draw list. In the future we could add more
	DirectX::XMMATRIX screen_projection;
	color render_target_color;
	std::wstring font;

	// add a vertex to the draw list
	void add_vertex(const vertex& vertex, const D3D_PRIMITIVE_TOPOLOGY type);

	// adds multiple vertices of the same typr to the defualt draw list
	void add_vertices(vertex* p_vertices, const size_t vertex_count, const D3D_PRIMITIVE_TOPOLOGY type);

	// process errors coming from the renderer
	void handle_error(const char* );

	// directx setup functions
	void setup_device_and_swapchain(HWND hwnd);
	void setup_backbuffer();
	void setup_viewport(HWND hwnd);
	void setup_shaders();
	void setup_input_layout();
	void setup_vertex_buffer();
	void setup_blend_state();
	void setup_rasterizer_state();
	void setup_depth_stencil_state();
	void setup_screen_projection();
	void setup_font_renderer(std::wstring font);
};