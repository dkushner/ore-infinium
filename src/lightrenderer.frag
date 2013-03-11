#version 130

in vec2 frag_texcoord;
in vec4 frag_color;

uniform sampler2D lightMap;
uniform float time;

out vec4 fragColor;

void main() {
//    float distance = 8 * distance(frag_texcoord.xy, vec2(0.5, 0.5));
    fragColor = vec4(modf(time, ));
//texture2D(lightMap, frag_texcoord) * (frag_color);
//vec4(frag_color.rgb * (1/distance), frag_color.a * (2/ distance));
}
