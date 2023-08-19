in vec2 tex_coord;
layout (location = 0) out vec4 frag_color;

layout (binding = 5) uniform sampler2D colormap_water;

void main() {
	frag_color = vec4(0., 0., 0., 1.);

	// river_body.dds
	vec4 OutColor = texture( colormap_water, tex_coord );
	//! river_movement.dds
	//vec4 Movement = texture( AnimatedTexture, v.vTexCoord1 );
	//! colormap_water
	//vec4 WaterColor = texture( Water, v.vWaterTexCoord );
	//OutColor.rgb = (WaterColor.rgb+waterColorModValue) * Movement.a + OutColor.rgb * ( 1.0 - Movement.a );
	//OutColor.rgb += Movement.rgb;

	//OutColor.rgb = Movement.rgb;
	
	// Terra incognita
	//float4 TerraIncognita = tex2D( TerraIncognitaTexture, v.vTerrainTexCoord );
	//OutColor.rgb += ( TerraIncognita.g - 0.25 )*1.33;

	frag_color = OutColor;
}
