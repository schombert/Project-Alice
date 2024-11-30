in vec3 tex_coord;
in float opacity;
in float text_size;
out vec4 frag_color;

uniform float is_black;
uniform float gamma;

uniform sampler2D texture_sampler;

vec4 gamma_correct(vec4 colour) {
	return vec4(pow(colour.rgb, vec3(1.f / gamma)), colour.a);
}


float clamp(float x, float lowerlimit = 0.0f, float upperlimit = 1.0f) {
  if (x < lowerlimit) return lowerlimit;
  if (x > upperlimit) return upperlimit;
  return x;
}

float smoothstep (float edge0, float edge1, float x) {
   x = clamp((x - edge0) / (edge1 - edge0));

   return x * x * (3.0f - 2.0f * x);
}


float get_value(float d) {
    float value = 1.f;    
    if (d > 0.5f) {
        return 0.f;
    } else if ((d > 0.500f) && (d < 0.510f)) {
        value = 1.f - smoothstep(0.500f, 0.510f, d);
    } else {
        value = 1.f;
    }
    return value;
}

float get_opacity(float d) {    
    float t = max(0.0f, d * 16.0f - 7.0f);
    return pow(t, 5.f);
}

void main() {
	float border_size = 0.022f;
	vec3 inner_color = vec3(1.0 - is_black, 1.0 - is_black, 1.0 - is_black);
	vec3 outer_color = vec3(0.27 * is_black, 0.27 * is_black, 0.27 * is_black);
    
    outer_color = mix(inner_color, outer_color, text_size * 40.f);
	
	vec4 color_in = texture(texture_sampler, vec2(tex_coord.rg));
       
    float dist = color_in.r;
    vec2 ddist = vec2(dFdx(dist), dFdy(dist));
    float pixel_distance = length(ddist) * 0.1f;
    
    float average_value = 0.f;
    for (int i = -5; i <= 5; i++) {
        average_value += get_value(dist + i * pixel_distance);
    }
    
    float average_opacity = 0.f;
    for (int i = -5; i <= 5; i++) {
        average_opacity += get_opacity(dist + i * pixel_distance);
    }
    
    average_value /= 11.f;
    average_opacity /= 11.f;
    
    
    frag_color = vec4(mix(inner_color, outer_color, average_value), average_opacity * opacity);
	frag_color = gamma_correct(frag_color);
}
