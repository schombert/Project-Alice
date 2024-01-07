#define M_PI 3.1415926535897932384626433832795
#define PI 3.1415926535897932384626433832795

// Goes from 0 to 1
struct Input {
	float2 vertex_position:SV_Target0;
	bool is_globe_map;
};
// layout (location = 1) in float2 v_tex_coord;

struct Output {
	float2 tex_coord;
	float4 position;
};

// Camera position
float2 offset;
float aspect_ratio;
float zoom;
float2 map_size;
float3x3 rotation;


float4 globe_coords(float2 vertex_position) {
	float3 new_world_pos;
	float angle_x = 2 * vertex_position.x * PI;
	new_world_pos.x = cos(angle_x);
	new_world_pos.y = sin(angle_x);

	float angle_y = vertex_position.y * PI;
	new_world_pos.x *= sin(angle_y);
	new_world_pos.y *= sin(angle_y);
	new_world_pos.z = cos(angle_y);
	new_world_pos = rotation * new_world_pos;
	new_world_pos /= PI; 		// Will make the zoom be the same for the globe and flat map
	new_world_pos.y *= 0.02; 	// Sqeeze the z coords. Needs to be between -1 and 1
	new_world_pos.xz *= -1; 	// Invert the globe
	new_world_pos.xyz += 0.5; 	// Move the globe to the center

	return float4(
		(2. * new_world_pos.x - 1.f) / aspect_ratio  * zoom,
		(2. * new_world_pos.z - 1.f) * zoom,
		(2. * new_world_pos.y - 1.f) - 1.0f,
		1.0);
}

float4 flat_coords(float2 vertex_position) {
	float2 world_pos = vertex_position + float2(-offset.x, offset.y);
	world_pos.x = fmod(world_pos.x, 1.0f);

	return float4(
		(2. * world_pos.x - 1.f) * zoom / aspect_ratio * map_size.x / map_size.y,
		(2. * world_pos.y - 1.f) * zoom,
		abs(world_pos.x - 0.5) * 2.1f,
		1.0);
}

Output main(Input input) {
	Output output;
	output.position = input.is_globe_map ? globe_coords(input.vertex_position) : flat_coords(input.vertex_position);
	output.tex_coord = input.vertex_position;
	return output;
}
