layout (location = 0) in vec2 vertex_position; //0
layout (location = 1) in vec2 v_tex_coord; //1
out vec2 tex_coord;

uniform float screen_width;
uniform float screen_height;
// The 2d coordinates on the screen
// d_rect.x - x coordinate
// d_rect.y - y coordinate
// d_rect.z - width
// d_rect.w - height
uniform vec4 d_rect;

void main() {
	// Transform the d_rect rectangle to screen space coordinates
	// vertex_position is used to flip and/or rotate the coordinates
	gl_Position = vec4(
		-1.0 + (2.0 * ((vertex_position.x * d_rect.z)  + d_rect.x) / screen_width),
		 1.0 - (2.0 * ((vertex_position.y * d_rect.w)  + d_rect.y) / screen_height),
		0.0, 1.0);
	tex_coord = v_tex_coord;
}
