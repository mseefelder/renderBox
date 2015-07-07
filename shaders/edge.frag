#version 430

out vec4 out_Color;
uniform sampler2D normals;
uniform sampler2D depth;

void main(void)
{
    //check alpha
    float alpha = 1;

    float threshold = 0.4;

    vec3 base = vec3(0);

    vec3 N = texelFetch(normals, ivec2(gl_FragCoord.xy)+ivec2(0,1), 0).rgb;
    vec3 S = texelFetch(normals, ivec2(gl_FragCoord.xy)-ivec2(0,1), 0).rgb;
    vec3 E = texelFetch(normals, ivec2(gl_FragCoord.xy)+ivec2(1,0), 0).rgb;
    vec3 W = texelFetch(normals, ivec2(gl_FragCoord.xy)-ivec2(1,0), 0).rgb;

    vec3 gradient = N-S + E-W;

    float diff = ((length(gradient)) - threshold);
    if (diff > 0.0)
    {
    	base += vec3(1.0);//vec3(diff);
    }
    else
    {
        alpha = 0.0;
    }
    
    //----------------------------------------------
    float alpha_d = 1;

    float threshold_d = 0.2;

    vec3 base_d = vec3(0);

    vec3 Nd = texelFetch(depth, ivec2(gl_FragCoord.xy)+ivec2(0,1), 0).rgb;
    vec3 Sd = texelFetch(depth, ivec2(gl_FragCoord.xy)-ivec2(0,1), 0).rgb;
    vec3 Ed = texelFetch(depth, ivec2(gl_FragCoord.xy)+ivec2(1,0), 0).rgb;
    vec3 Wd = texelFetch(depth, ivec2(gl_FragCoord.xy)-ivec2(1,0), 0).rgb;

    vec3 gradient_d = Nd-Sd + Ed-Wd;

    float diff_d = ((length(gradient_d)) - threshold_d);
    if (diff_d > 0.0)
    {
        base_d += vec3(1.0);//vec3(diff);
    }
    else
    {
        alpha_d = 0.0;
    }

    out_Color = vec4(base_d + base, max(alpha_d, alpha));

}
