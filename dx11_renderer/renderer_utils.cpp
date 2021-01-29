#include "renderer_utils.h"

//
// vec2 definitions
//

vec2::vec2() :
	x(0.f), y(0.f)
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

std::string color::to_string() const
{
	return "{ " + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ", " + std::to_string(a) + " }";
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