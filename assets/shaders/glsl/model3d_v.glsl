// Goes from 0 to 1
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 normal_direction;
layout (location = 2) in vec2 texture_coord;

out vec2 tex_coord;

uniform vec2 offset;
uniform float aspect_ratio;
uniform float zoom;
uniform vec2 map_size;
uniform float time;
uniform mat3 rotation;
uniform vec2 model_offset;
uniform float target_facing;
uniform float target_topview_fixup;
uniform uint subroutines_index;

vec4 globe_coords(vec3 world_pos) {
    vec3 new_world_pos;
    float angle_x = 2 * world_pos.x * PI;
    new_world_pos.x = cos(angle_x);
    new_world_pos.y = sin(angle_x);

    float angle_y = world_pos.z * PI;
    new_world_pos.x *= sin(angle_y);
    new_world_pos.y *= sin(angle_y);
    new_world_pos.z = cos(angle_y);
    new_world_pos = rotation * new_world_pos;
    new_world_pos /= PI;         // Will make the zoom be the same for the globe and flat map
    new_world_pos *= (1 + world_pos.y);
    new_world_pos.y *= 0.02;     // Sqeeze the z coords. Needs to be between -1 and 1
    new_world_pos.xz *= -1;     // Invert the globe
    new_world_pos.xyz += 0.5;     // Move the globe to the center
    return vec4(
        (2.f * new_world_pos.x - 1.f) / aspect_ratio  * zoom,
        (2.f * new_world_pos.z - 1.f) * zoom,
        (2.f * new_world_pos.y - 1.f), 1.0);
}

vec4 flat_coords(vec3 world_pos) {
	world_pos -= vec3(offset.x, 0.f, -offset.y);
	world_pos.x = mod(world_pos.x, 1.0f);
	return vec4(
		(+2.f * world_pos.x - 1.f) * zoom / aspect_ratio * map_size.x / map_size.y,
		(+2.f * world_pos.z - 1.f) * zoom,
		(-2.f * world_pos.y),
		1.0f
	);
}

vec4 calc_gl_position(vec3 world_pos) {
	switch(int(subroutines_index)) {
case 0: return globe_coords(world_pos);
//TODO: case 1: return perspective_coords(world_pos);
case 2: return flat_coords(world_pos);
default: break;
	}
	return vec4(0.f);
}

// A rotation so units can face were they are going
vec3 rotate_target(vec3 v) {
	vec3 k = vec3(0.f, 1.f, 0.f);
	float cos_theta = cos(target_facing - M_PI / 2.f);
	float sin_theta = sin(target_facing - M_PI / 2.f);
	return (v * cos_theta) + (cross(k, v) * sin_theta) + (k * dot(k, v)) * (1.f - cos_theta);
}
// Skew so the player can see half of the model
vec3 rotate_skew(vec3 v) {
	vec3 k = vec3(1.f, 0.f, 0.f);
	float cos_theta = cos(target_topview_fixup);
	float sin_theta = sin(target_topview_fixup);
	return (v * cos_theta) + (cross(k, v) * sin_theta) + (k * dot(k, v)) * (1.f - cos_theta);
}

void main() {
	float vertical_factor = ((map_size.x + map_size.y) / 3.f);
	float height_scale = map_size.x / 2.f;
	vec3 world_pos = vec3(vertex_position.x, vertex_position.y, vertex_position.z);
	world_pos = rotate_skew(rotate_target(world_pos));
	world_pos /= vec3(map_size.x, height_scale, vertical_factor);
	world_pos += vec3(model_offset.x / map_size.x, 0.f, model_offset.y / map_size.y);
	gl_Position = calc_gl_position(world_pos);
	tex_coord = texture_coord;
}
