#define M_PI 3.1415926535897932384626433832795
#define PI 3.1415926535897932384626433832795

struct Input{
	// Goes from 0 to 1
	float2 vertex_position:SV_Target0;
	float2 normal_direction:SV_Target1;
	float2 direction:SV_Target2;
	float2 texture_coord:SV_Target3;
	float type: SV_Target4;
	bool is_globe_map;
};


struct Output{
	float2 tex_coord;
	float4 position;
};

// Camera position
uniform float2 offset;
uniform float aspect_ratio;
// Zoom: big numbers = close
uniform float zoom;
// The size of the map in pixels
uniform float2 map_size;
// The scaling factor for the width
uniform float border_width;
uniform float3x3 rotation;



float4 globe_coords(float2 world_pos) {

	float3 new_world_pos;
	float section_x = 200;
	float angle_x1 = 2 * PI * floor(world_pos.x * section_x) / section_x;
	float angle_x2 = 2 * PI * floor(world_pos.x * section_x + 1) / section_x;
	float angle_x = lerp(angle_x1, angle_x2, fmod(world_pos.x * section_x, 1));
	new_world_pos.x = lerp(cos(angle_x1), cos(angle_x2), fmod(world_pos.x * section_x, 1));
	new_world_pos.y = lerp(sin(angle_x1), sin(angle_x2), fmod(world_pos.x * section_x, 1));
	float section_y = 200;
	float angle_y1 = PI * floor(world_pos.y * section_y) / section_y;
	float angle_y2 = PI * floor(world_pos.y * section_y + 1) / section_y;
	new_world_pos.x *= lerp(sin(angle_y1), sin(angle_y2), fmod(world_pos.y * section_y, 1));
	new_world_pos.y *= lerp(sin(angle_y1), sin(angle_y2), fmod(world_pos.y * section_y, 1));
	new_world_pos.z = lerp(cos(angle_y1), cos(angle_y2), fmod(world_pos.y * section_y, 1));
	new_world_pos =  new_world_pos * rotation;
	new_world_pos /= PI; 		// Will make the zoom be the same for the globe and flat map
	new_world_pos.y *= 0.02; 	// Sqeeze the z coords. Needs to be between -1 and 1
	new_world_pos.xz *= -1; 	// Invert the globe
	new_world_pos.xyz += 0.5; 	// Move the globe to the center

	return float4(
		(2. * new_world_pos.x - 1.f) / aspect_ratio  * zoom,
		(2. * new_world_pos.z - 1.f) * zoom,
		(2. * new_world_pos.y - 1.f), 1.0);
}

float4 flat_coords(float2 world_pos) {
	world_pos += float2(-offset.x, offset.y);
	world_pos.x = fmod(world_pos.x, 1.0f);
	return float4(
		(2. * world_pos.x - 1.f) * zoom / aspect_ratio * map_size.x / map_size.y,
		(2. * world_pos.y - 1.f) * zoom,
		0.0, 1.0);
}

// The borders are drawn by seperate quads.
// Each triangle in the quad is made up by two vertices on the same position and
// another one in the "direction" vector. Then all the vertices are offset in the "normal_direction".
Output main(Input input) {
	float thickness = border_width * 0.25f;
	float2 rot_direction = float2(-input.direction.y, input.direction.x);
	float2 normal_vector = normalize(input.normal_direction) * thickness;
	// Extend the border slightly to make it fit together with any other border in any octagon direction.
	float2 extend_vector = -normalize(input.direction) * thickness;
	extend_vector *= round(input.type) == 3.f ? 2.f : 0.f;
	float2 world_pos = input.vertex_position;


	world_pos.x *= map_size.x / map_size.y;
	world_pos += normal_vector;
	world_pos += extend_vector;
	world_pos.x /= map_size.x / map_size.y;

	Output output;
	output.position = input.is_globe_map ? globe_coords(world_pos) : flat_coords(world_pos);
	output.tex_coord = input.texture_coord / 2.f + float2(0.5 * fmod(round(input.type), 2.f), 0.5 * floor(fmod(input.type, 3.0f) / 2.f));

	return output;
}
