// Based on: http://wdobbie.com/post/gpu-text-rendering-with-vector-textures/

struct Glyph {
	int start, count;
};

struct Curve {
	vec2 p0, p1, p2;
};

uniform isamplerBuffer glyphs;

uniform samplerBuffer curves;
uniform samplerBuffer bold_curves;

uniform vec4 color;
uniform vec4 outline_color;


// Size of the window (in pixels) used for 1-dimensional anti-aliasing along each rays.
//   0 - no anti-aliasing
//   1 - normal anti-aliasing
// >=2 - exaggerated effect 
float antiAliasingWindowSize = 1.0;

// Enable a second ray along the y-axis to achieve 2-dimensional anti-aliasing.
bool enableSuperSamplingAntiAliasing = false;


in vec2 uv;
flat in int bufferIndex;
flat in float opacity;

out vec4 result;

Glyph loadGlyph(int index) {
	Glyph result;
	ivec2 data = texelFetch(glyphs, index).xy;
	result.start = data.x;
	result.count = data.y;
	return result;
}

Curve loadCurve(int index) {
	Curve result;
	result.p0 = texelFetch(curves, 3*index+0).xy;
	result.p1 = texelFetch(curves, 3*index+1).xy;
	result.p2 = texelFetch(curves, 3*index+2).xy;
	return result;
}

Curve loadBoldCurve(int index) {
	Curve result;
	result.p0 = texelFetch(bold_curves, 3*index+0).xy;
	result.p1 = texelFetch(bold_curves, 3*index+1).xy;
	result.p2 = texelFetch(bold_curves, 3*index+2).xy;
	return result;
}

float computeCoverage_real(float inverseDiameter, vec2 p0, vec2 p1, vec2 p2) {
	if (p0.y > 0 && p1.y > 0 && p2.y > 0) return 0.0;
	if (p0.y < 0 && p1.y < 0 && p2.y < 0) return 0.0;

	// Note: Simplified from abc formula by extracting a factor of (-2) from b.
	vec2 a = p0 - 2*p1 + p2;
	vec2 b = p0 - p1;
	vec2 c = p0;

	float t0, t1;
	if (abs(a.y) >= 1e-5) {
		// Quadratic segment, solve abc formula to find roots.
		float radicand = b.y*b.y - a.y*c.y;
		if (radicand <= 0) return 0.0;
	
		float s = sqrt(radicand);
		t0 = (b.y - s) / a.y;
		t1 = (b.y + s) / a.y;
	} else {
		// Linear segment, avoid division by a.y, which is near zero.
		// There is only one root, so we have to decide which variable to
		// assign it to based on the direction of the segment, to ensure that
		// the ray always exits the shape at t0 and enters at t1. For a
		// quadratic segment this works 'automatically', see readme.
		float t = p0.y / (p0.y - p2.y);
		if (p0.y < p2.y) {
			t0 = -1.0;
			t1 = t;
		} else {
			t0 = t;
			t1 = -1.0;
		}
	}

	float alpha = 0;

	if (t0 >= 0 && t0 < 1) {
		float x = (a.x*t0 - 2.0*b.x)*t0 + c.x;
		alpha += clamp(x * inverseDiameter + 0.5, 0, 1);
	}

	if (t1 >= 0 && t1 < 1) {
		float x = (a.x*t1 - 2.0*b.x)*t1 + c.x;
		alpha -= clamp(x * inverseDiameter + 0.5, 0, 1);
	}

	return alpha;
}

float computeCoverage(float inverseDiameter, vec2 p0, vec2 p1, vec2 p2) {
	if (p0.y > 0 && p1.y > 0 && p2.y > 0) return 0.0;
	if (p0.y < 0 && p1.y < 0 && p2.y < 0) return 0.0;

	// Note: Simplified from abc formula by extracting a factor of (-2) from b.
	vec2 a = p0 - 2*p1 + p2;
	vec2 b = p0 - p1;
	vec2 c = p0;

	float t0, t1;
	
		// Linear segment, avoid division by a.y, which is near zero.
		// There is only one root, so we have to decide which variable to
		// assign it to based on the direction of the segment, to ensure that
		// the ray always exits the shape at t0 and enters at t1. For a
		// quadratic segment this works 'automatically', see readme.
		float t = p0.y / (p0.y - p2.y);
		if (p0.y < p2.y) {
			t0 = -1.0;
			t1 = t;
		} else {
			t0 = t;
			t1 = -1.0;
	}
	

	float alpha = 0;
	
	if (t0 >= 0 && t0 < 1) {
		float x = (a.x*t0 - 2.0*b.x)*t0 + c.x;
		alpha += clamp(x * inverseDiameter + 0.5, 0, 1);
		}

	if (t1 >= 0 && t1 < 1) {
		float x = (a.x*t1 - 2.0*b.x)*t1 + c.x;
		alpha -= clamp(x * inverseDiameter + 0.5, 0, 1);
		}

	return alpha;
}


vec2 rotate(vec2 v) {
	return vec2(v.y, -v.x);
}

void main() {
	float alpha_bold = 0.f;
	float alpha = 0.f;

	vec2 dtex_dpixel = fwidth(uv);

	// Inverse of the diameter of a pixel in uv units for anti-aliasing.
	vec2 inverseDiameter = 1.0 / (antiAliasingWindowSize * dtex_dpixel);

	Glyph glyph = loadGlyph(bufferIndex);
	vec2 adj_uv = uv;

	float alpha_bold_upper_limit = max(0.f, 1.f - (dtex_dpixel.x / (2.f / 64.f) + dtex_dpixel.y / (2.f / 64.f)) * 0.25f);

	if (outline_color.a > 0.f && alpha_bold_upper_limit > 0.f) {
		for (int i = 0; i < glyph.count; i++) {
			Curve curve = loadBoldCurve(glyph.start + i);

			vec2 p0 = curve.p0 - uv;
			vec2 p1 = curve.p1 - uv;
			vec2 p2 = curve.p2 - uv;

			alpha_bold += computeCoverage_real(inverseDiameter.x, p0, p1, p2);
		}

		alpha_bold = clamp(alpha_bold, 0.0, 1.0) * alpha_bold_upper_limit;
		if (alpha_bold == 0.f) {
			discard;
		}
		adj_uv -= 2.f / 64.f;
	} else {
		alpha_bold = 0.f;
	}

	for (int i = 0; i < glyph.count; i++) {
		Curve curve = loadCurve(glyph.start + i);

		vec2 p0 = curve.p0 - adj_uv;
		vec2 p1 = curve.p1 - adj_uv;
		vec2 p2 = curve.p2 - adj_uv;

		alpha += computeCoverage_real(inverseDiameter.x, p0, p1, p2);
		if (enableSuperSamplingAntiAliasing) {
			alpha += computeCoverage_real(inverseDiameter.y, rotate(p0), rotate(p1), rotate(p2));
		}
	}

	if (enableSuperSamplingAntiAliasing) {
		alpha *= 0.5;
	}

	alpha = clamp(alpha, 0.0, 1.0);

	if (alpha > 0.f) {
		result = color * alpha * opacity * color.a;
	} else {
		result = outline_color * alpha_bold * opacity * color.a;
	}
}
