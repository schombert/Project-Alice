in vec2 texcoord;
out vec4 out_color;

uniform sampler2D screen_texture;
uniform float gaussian_radius;
uniform vec2 screen_size;

float gaussian_blur(vec2 p, float std) {
	float r = 2.0 * std * std;
	float t = 3.1415 * r;
	float s = p.x * p.x + p.y * p.y;
	return (1.0 / t) * pow(2.71, -(s / r));
}

vec4 gaussian_colour() {
	if(gaussian_radius <= 1.0) {
		return texture2D(screen_texture, texcoord);
	}
	float r = gaussian_radius;
	float dx = 1.0 / screen_size.x / 2.f;
	float dy = 1.0 / screen_size.y / 2.f;
	vec4 col = vec4(0.0, 0.0, 0.0, 0.0);
	vec2 p = texcoord - vec2(dx * r, dy * r) + 0.5f * vec2(dx, dy);
	for(float x = -r; x <= r; x++, p.x += dx) {
		for(float y = -r; y <= r; y++, p.y += dy) {
			col += texture2D(screen_texture, p) * gaussian_blur(vec2(x, y), r / 2.0);
		}
	}
	return col;
}

void main() {
	out_color = gaussian_colour();
}
