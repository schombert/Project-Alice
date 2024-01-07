struct Input {
	float2 text_coord;
	bool is_globe_map;
};

struct Output {
	float4 frag_color:SV_Target0; 
};

uniform Texture2D unit_arrow_texture:register(t0);
uniform SamplerState unit_arrow_sampler:register(s0);

uniform float gamma:SV_Target7;


float4 gamma_correct(float4 color) {
	return float4(pow(color.rgb, float3(1.f/gamma)), color.a);
}


Output main(Input input) {
	Output output;
	//frag_color = vec4(tex_coord.x, tex_coord.y, 0, 1.);
	float4 out_color = unit_arrow_texture.Sample(unit_arrow_sampler, input.text_coord );
	output.frag_color = gamma_correct(out_color);
	return output;
}
