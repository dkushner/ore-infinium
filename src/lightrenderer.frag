#version 130

in vec2 frag_texcoord;
in vec4 frag_color;

uniform sampler2D lightMap;

out vec4 fragColor;

void main() {
    vec2 texcoord = frag_texcoord;

    vec4 source = texture2D(lightMap, texcoord);
    vec4 light =  vec4(source.rgb * frag_color.rgb, source.a);

    fragColor += light;
}
