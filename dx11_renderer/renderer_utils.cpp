#include "renderer_utils.h"

//
// vec2 definitions
//

vec2::vec2() :
	x(0.f), y(0.f)
{ }

vec2::vec2(float xy) :
	x(xy), y(xy)
{ }

vec2::vec2(float x, float y) :
	x(x), y(y)
{ }

vec2::vec2(int x, int y) :
	x(static_cast<float>(x)),
	y(static_cast<float>(y))
{ }

bool vec2::operator==(const vec2& other) const
{
	return x == other.x && y == other.y;
}

vec2 vec2::operator-(float amount) const
{
	return { x - amount, y - amount };
}

vec2 vec2::operator-(const vec2& other) const
{
	return { x - other.x, y - other.y };
}

vec2 vec2::operator+(float amount) const
{
	return { x + amount, y + amount };
}

vec2 vec2::operator+(const vec2& other) const
{
	return { x + other.x, y + other.y };
}

vec2 vec2::operator/(const vec2& other) const
{
	return { x / other.x, y / other.y };
}

vec2 vec2::operator/(float amount) const
{
	return { x / amount, y / amount };
}

vec2 vec2::operator*(float amount) const
{
	return { x * amount, y * amount };
}

vec2 vec2::operator*(const vec2& other) const
{
	return { x * other.x, y * other.y };
}

std::string vec2::to_string() const
{
	return "{ " + std::to_string(x) + "f, " + std::to_string(y) + "f }";
}

bool vec2::higher_or_leftmost(const vec2& other) const
{
	return y < other.y || y == other.y && x < other.x;
}

bool vec2::rightmost_or_higher(const vec2& other) const
{
	return x > other.x || x == other.x && y < other.y;
}

bool vec2::is_inside(const vec2& top_left, const vec2& size) const
{
	return x >= top_left.x 
		&& y >= top_left.y 
		&& x <= top_left.x + size.x 
		&& y <= top_left.y + size.y;
}

vec2& vec2::clamp(const vec2& min, const vec2& max)
{
	if (x < min.x)
		x = min.x;
	if (x > max.x)
		x = max.x;
	if (y < min.y)
		y = min.y;
	if (y > max.y)
		y = max.y;

	return *this;
}

//
// vec3 definitions
//

vec3::vec3(float x, float y, float z) :
	x(x), y(y), z(z)
{}

std::string vec3::to_string() const
{
	return "{ " + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + " }";
}

//
// color definitions
//

color::color() :
	r(0.f), g(0.f), b(0.f), a(0.f)
{ }

color::color(float rgba) :
	r(rgba), g(rgba), b(rgba), a(rgba)
{ }

color::color(float r, float g, float b, float a) :
	r(r), g(g), b(b), a(a)
{ }

void color::operator+=(float amount)
{
	r += amount;
	g += amount;
	b += amount;
}

float& color::operator[](int index)
{
	if (index == 0) return r;
	if (index == 1) return g;
	if (index == 2) return b;
	if (index == 3) return a;
}

uint32_t color::to_hex_abgr() const
{
	uint32_t hex{};
	hex |= static_cast<uint32_t>(a * 255) << 24;
	hex |= static_cast<uint32_t>(b * 255) << 16;
	hex |= static_cast<uint32_t>(g * 255) << 8;
	hex |= static_cast<uint32_t>(r * 255) << 0;

	return hex;
}

hsv color::to_hsv() const
{
	hsv out{};
	double min, max, delta;

	min = r < g ? r : g;
	min = min < b ? min : b;

	max = r > g ? r : g;
	max = max > b ? max : b;

	out.v = max;                                // v
	delta = max - min;
	if (delta < 0.00001)
	{
		out.s = 0;
		out.h = 0; // undefined, maybe nan?
		return out;
	}
	if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
		out.s = (delta / max);                  // s
	}
	else {
		// if max is 0, then r = g = b = 0              
		// s = 0, h is undefined
		out.s = 0.0;
		out.h = NAN;                            // its now undefined
		return out;
	}
	if (r >= max)                           // > is bogus, just keeps compilor happy
		out.h = (g - b) / delta;        // between yellow & magenta
	else
		if (g >= max)
			out.h = 2.0 + (b - r) / delta;  // between cyan & yellow
		else
			out.h = 4.0 + (r - g) / delta;  // between magenta & cyan

	out.h *= 60.0;                              // degrees

	if (out.h < 0.0)
		out.h += 360.0;

	return out;
	
}

std::string color::to_string() const
{
	return "{ " + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ", " + std::to_string(a) + " }";
}

//
// hsv definitions
//

color hsv::to_rgb() const
{
	color rgba{};
	if (s == 0.0f)
	{
		// gray
		rgba.r = rgba.g = rgba.b = v;
		return rgba;
	}

	float deg_h = std::fmodf(h, 1.0f) / (60.0f / 360.0f);
	int   i = (int)deg_h;
	float f = deg_h - (float)i;
	float p = v * (1.0f - s);
	float q = v * (1.0f - s * f);
	float t = v * (1.0f - s * (1.0f - f));

	switch (i)
	{
	case 0:
		rgba.r = v;
		rgba.g = t;
		rgba.b = p;
		break;
	case 1:
		rgba.r = q;
		rgba.g = v;
		rgba.b = p;
		break;
	case 2:
		rgba.r = p;
		rgba.g = v;
		rgba.b = t;
		break;
	case 3:
		rgba.r = p;
		rgba.g = q;
		rgba.b = v;
		break;
	case 4:
		rgba.r = t;
		rgba.g = p;
		rgba.b = v;
		break;
	case 5:
	default:
		rgba.r = v;
		rgba.g = p;
		rgba.b = q;
		break;
	}

	return rgba;
}

//
// vertex definitions
//

vertex::vertex() :
	x(0.f), y(0.f), z(0.f),
	r(0.f), g(0.f), b(0.f), a(0.f)
{ }

vertex::vertex(float x, float y, float z, float r, float g, float b, float a) :
	x(x), y(y), z(z),
	r(r), g(g), b(b), a(a)
{ }

vertex::vertex(const vec2& pos, const color& rgba) :
	x(pos.x), y(pos.y), z(0.f),
	r(rgba.r), g(rgba.g), b(rgba.b), a(rgba.a)
{ }

vertex::vertex(const vec3& pos, const color& rgba) :
	x(pos.x), y(pos.y), z(pos.z),
	r(rgba.r), g(rgba.g), b(rgba.b), a(rgba.a)
{ }

vertex::vertex(float x, float y, float z, const color& rgba) :
	x(x), y(y), z(z),
	r(rgba.r), g(rgba.g), b(rgba.b), a(rgba.a)
{ }

void vertex::set_color(const color& new_color)
{
	r = new_color.r;
	g = new_color.g;
	b = new_color.b;
	a = new_color.a;
}

void vertex::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
}

void vertex::operator+=(float addition)
{
	x += addition;
	y += addition;
}

void vertex::operator+=(const vec2& add)
{
	x += add.x;
	y += add.y;
}

//
// batch definitions
//

batch::batch(D3D_PRIMITIVE_TOPOLOGY type, size_t vertex_count) :
	type(type),
	vertex_count(vertex_count)
{ }