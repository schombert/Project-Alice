#version 430 core

layout (binding = 0) uniform sampler2D border_texture;

in vec2 tex_coord;
in vec2 type;
layout (location = 0) out vec4 frag_color;

void main() {
	vec2 TexCoord = tex_coord.xy;
	TexCoord.y /= 2;

	float CornerOffset = 0;
	CornerOffset = 1;

	vec2 TexCoord2 = TexCoord;
	vec2 TexCoord3 = TexCoord;

	vec2 offset = type + type / 256;
	TexCoord.x += offset.x;
	// TexCoord.x -= 0.5;
	const float BIAS = -0.8;
	// float4 ProvinceBorder = tex2Dbias( BorderTexture, float4(TexCoord, 0, BIAS) );
	vec4 ProvinceBorder = texture( border_texture, TexCoord, BIAS );
	// vec4 ProvinceBorder = texture( border_texture, TexCoord );

	// TexCoord2.x += BorderTypeColor.r;
	// TexCoord2.y += 0.125; // 0.25
	// float4 CountryBorder = tex2Dbias( BorderTexture, float4(TexCoord2, 0, BIAS) );
	// float4 CountryBorder = tex2D( border_texture, TexCoord2 );

	TexCoord3.x += offset.y * CornerOffset;
	TexCoord3.y += 0.25; // 0.5
	// TexCoord3.y += (BorderTypeColor.a * CornerOffset);

	vec4 DiagBorder = texture( border_texture, TexCoord3, BIAS );
	// vec4 DiagBorder = texture( border_texture, TexCoord3 );

	ProvinceBorder.rgb *= ProvinceBorder.a;
	// CountryBorder.rgb *= CountryBorder.a;
	DiagBorder.rgb *= DiagBorder.a;

	vec4 OutColor = vec4(0., 0., 0., 0.);

	// OutColor = ProvinceBorder;
	OutColor.rgb = ProvinceBorder.rgb*ProvinceBorder.a;
	OutColor.a = ProvinceBorder.a;
	// OutColor.a = max( ProvinceBorder.a, CountryBorder.a );
	OutColor.a = max( OutColor.a, DiagBorder.a );

	// OutColor.rgb = CountryBorder.rgb * CountryBorder.a + OutColor.rgb*( 1.0f - CountryBorder.a );
	OutColor.rgb = max( OutColor.rgb, DiagBorder.rgb );
	// OutColor = float4( v.vUV_ProvUV.x * 10.f, v.vUV_ProvUV.y * 5.f, 0.f, 1.f);
	// OutColor = float4( ProvinceBorder.a, CountryBorder.a, DiagBorder.a, 1.f );
	// OutColor = vec4( offset.y, offset.x, 0.f, 1.f );
	// OutColor = float4( v.vUV_ProvUV.x, v.vUV_ProvUV.y, 0.f, 1.f );
	// OutColor = vec4( fract(32 * (type + type / 256)), 0.f, 0.f, 1.f );
	// OutColor = vec4( tex_coord.x, tex_coord.y, 0.f, 1.f );
	frag_color = OutColor;
}
