#version 430

out vec4 out_Color;
uniform sampler2D edges;
uniform sampler2D colors;

void main(void)
{
	vec4 pixel = texelFetch(colors, ivec2(gl_FragCoord.xy), 0);

	float alpha = texelFetch(edges, ivec2(gl_FragCoord.xy), 0).a;

	float background = 1.0;
	if(length(pixel.xyz) == 0.0)
		discard;

    vec3 N = texelFetch(edges, ivec2(gl_FragCoord.xy)+ivec2(0,1), 0).rgb;
    vec3 S = texelFetch(edges, ivec2(gl_FragCoord.xy)-ivec2(0,1), 0).rgb;
    vec3 E = texelFetch(edges, ivec2(gl_FragCoord.xy)+ivec2(1,0), 0).rgb;
    vec3 W = texelFetch(edges, ivec2(gl_FragCoord.xy)-ivec2(1,0), 0).rgb;

    vec3 gradient = (N+S+E+W)/4.0;

    out_Color = vec4((pixel.rgb-(gradient*alpha)), pixel.a);

}
