#include <fstream>

#include <stdint.h>
#include <string>

//Linked with x64 bit libs, so this is only x64 by default
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

static_assert(sizeof(float) == 4, "f32 is not 32-bit on this system");
static_assert(sizeof(double) == 8, "f64 is not 64-bit on this system");

typedef float f32;
typedef double f64;

struct vec2 
{
	s32 x, y;
};

s32 round_float(f32 value) 
{
	f32 remainder = value - static_cast<s32>(value);
	return (remainder >= 0.5) ? static_cast<s32>(value + 1.0f) : static_cast<s32>(value);
}

void order_points_by_ordinates(vec2& p1, vec2& p2, vec2& p3)
{
	vec2 temps[3]{ p1, p2, p3 };

	for (u32 i = 0; i < 3 - 1; i++) {
		for (u32 j = 1; j < 3; j++) {
			if (temps[j].y < temps[i].y)
				std::swap(temps[i], temps[j]);
		}
	}

	p1 = temps[0];
	p2 = temps[1];
	p3 = temps[2];
}

void fill_row(u8* draw_buffer, u32 begin, u32 size)
{
	for (u32 i = 0; i <= size; i++) {
		draw_buffer[begin + i] = 1;
	}
}

void draw_edges_row(u8* draw_buffer, u32 begin, u32 size, f32 first_dx, f32 second_dx)
{
	if (first_dx >= 0.0f) { 
		for (u32 i = begin; i <= begin + static_cast<u32>(first_dx); i++)
			draw_buffer[i] = 1;
	}
	else {
		first_dx = -first_dx;

		for (u32 i = begin - static_cast<u32>(first_dx); i <= begin; i++)
			draw_buffer[i] = 1;
	}


	if (second_dx >= 0.0f) {
		for (u32 i = begin + size; i <= begin + size + (u32)second_dx; i++)
			draw_buffer[i] = 1;
	}
	else {
		second_dx = -second_dx;

		for (u32 i = begin + size - (u32)second_dx; i <= begin + size; i++)
			draw_buffer[i] = 1;
	}
}

void draw_triangle(u8* draw_buffer, u32 width, vec2 p1, vec2 p2, vec2 p3)
{
	order_points_by_ordinates(p1, p2, p3);

	//Divide by zero exception is handled in the for loops directly, no dx that equals infinity
	//should ever be accessed by the algorithm
	f32 dx1 = static_cast<f32>(p2.x - p1.x) / static_cast<f32>(p2.y - p1.y);
	f32 dx2 = static_cast<f32>(p3.x - p1.x) / static_cast<f32>(p3.y - p1.y);
	f32 dx3 = static_cast<f32>(p3.x - p2.x) / static_cast<f32>(p3.y - p2.y);

	if (dx1 > dx2)
		std::swap(dx1, dx2);

	for (u32 starting_y = p1.y; starting_y < p2.y; starting_y++) {
		u32 sx = p1.x + round_float(dx1 * static_cast<f32>(starting_y - p1.y));
		u32 dx = p1.x + round_float(dx2 * static_cast<f32>(starting_y - p1.y));
		draw_edges_row(draw_buffer, starting_y * width + sx, dx - sx, dx1, dx2);
	}

	u32 intersection_sx = p1.x + round_float(dx1 * static_cast<f32>(p2.y - p1.y));
	u32 intersection_dx = p1.x + round_float(dx2 * static_cast<f32>(p2.y - p1.y));

	if (intersection_sx == p2.x) {
		for (u32 starting_y = p2.y; starting_y < p3.y; starting_y++) {
			u32 sx = p2.x + round_float(dx3 * static_cast<f32>(starting_y - p2.y));
			u32 dx = p1.x + round_float(dx2 * static_cast<f32>(starting_y - p1.y));
			draw_edges_row(draw_buffer, starting_y * width + sx, dx - sx, dx3, dx2);
		}
	}
	else {
		for (u32 starting_y = p2.y; starting_y < p3.y; starting_y++) {
			u32 sx = p1.x + round_float(dx1 * static_cast<f32>(starting_y - p1.y));
			u32 dx = p2.x + round_float(dx3 * static_cast<f32>(starting_y - p2.y));
			draw_edges_row(draw_buffer, starting_y * width + sx, dx - sx, dx1, dx3);
		}
	}

}

void fill_triangle(u8* draw_buffer, u32 width, vec2 p1, vec2 p2, vec2 p3) 
{
	order_points_by_ordinates(p1, p2, p3);

	f32 dx1 = static_cast<f32>(p2.x - p1.x) / static_cast<f32>(p2.y - p1.y);
	f32 dx2 = static_cast<f32>(p3.x - p1.x) / static_cast<f32>(p3.y - p1.y);
	f32 dx3 = static_cast<f32>(p3.x - p2.x) / static_cast<f32>(p3.y - p2.y);

	if (dx1 > dx2)
		std::swap(dx1, dx2);

	for (u32 starting_y = p1.y; starting_y < p2.y; starting_y++) {
		u32 sx = p1.x + round_float(dx1 * static_cast<f32>(starting_y - p1.y));
		u32 dx = p1.x + round_float(dx2 * static_cast<f32>(starting_y - p1.y));
		fill_row(draw_buffer, starting_y * width + sx, dx - sx);
	}

	u32 intersection_sx = p1.x + round_float(dx1 * static_cast<f32>(p2.y - p1.y));
	u32 intersection_dx = p1.x + round_float(dx2 * static_cast<f32>(p2.y - p1.y));

	if (intersection_sx == p2.x) {
		for (u32 starting_y = p2.y; starting_y < p3.y; starting_y++) {
			u32 sx = p2.x + round_float(dx3 * static_cast<f32>(starting_y - p2.y));
			u32 dx = p1.x + round_float(dx2 * static_cast<f32>(starting_y - p1.y));
			fill_row(draw_buffer, starting_y * width + sx, dx - sx);
		}
	}
	else {
		for (u32 starting_y = p2.y; starting_y < p3.y; starting_y++) {
			u32 sx = p1.x + round_float(dx1 * static_cast<f32>(starting_y - p1.y));
			u32 dx = p2.x + round_float(dx3 * static_cast<f32>(starting_y - p2.y));
			fill_row(draw_buffer, starting_y * width + sx, dx - sx);
		}
	}

}

//Output to a file, later its time to switch to a p3m implementation
void txt_impl(u32 width, u32 height) 
{
	std::fstream output_stream("my_output.txt", std::ios::out);


	u8* draw_buffer = new u8[width * height];
	std::memset(draw_buffer, 0, width * height);

	{
		vec2 p1 = { 20, 20 };
		vec2 p2 = { 200, 90 };
		vec2 p3 = { 50, 400 };
		draw_triangle(draw_buffer, width, p1, p3, p2);
	}

	{
		vec2 p1 = { 250 + 20, 20 };
		vec2 p2 = { 250 + 200, 90 };
		vec2 p3 = { 250 + 50, 400 };
		fill_triangle(draw_buffer, width, p1, p3, p2);
	}

	for (u32 y = 0; y < height; y++) {
		for (u32 x = 0; x < width; x++) {
			output_stream << (draw_buffer[y * width + x] == 0 ? " " : "1");
		}
		output_stream << '\n';
	}

	delete[] draw_buffer;
	output_stream.close();
}



void ppm_impl(u32 width, u32 height) 
{
	std::fstream output_stream("my_output.ppm", std::ios::out);
	u8* draw_buffer = new u8[width * height];
	std::memset(draw_buffer, 0, width * height);

	{
		vec2 p1 = { 20, 20 };
		vec2 p2 = { 200, 90 };
		vec2 p3 = { 50, 400 };
		draw_triangle(draw_buffer, width, p1, p3, p2); 
	}

	{
		vec2 p1 = { 250 + 20, 20 };
		vec2 p2 = { 250 + 200, 90 };
		vec2 p3 = { 250 + 50, 400 };
		fill_triangle(draw_buffer, width, p1, p3, p2);
	}

	output_stream << "P3\n";
	output_stream << std::to_string(width) + " " + std::to_string(height) + "\n";
	output_stream << "255\n";

	for (u32 y = 0; y < height; y++) {
		for (u32 x = 0; x < width; x++) {
			output_stream << (draw_buffer[y * width + x] == 0 ? "0 0 0   " : "255 255 255   ");
		}
		output_stream << '\n';
	}

	delete[] draw_buffer;
	output_stream.close();
}

int main() 
{
	const u32 canvas_width = 500;
	const u32 canvas_height = 500;
	ppm_impl(canvas_width, canvas_height);
}