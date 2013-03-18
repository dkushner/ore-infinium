#version 130

in vec2 frag_texcoord;
in vec4 frag_color;

uniform sampler2D lightMap;

out vec4 fragColor;

void main() {
    vec2 texcoord = frag_texcoord;

    vec4 light = texture2D(lightMap, texcoord) * (frag_color);

    fragColor = light;
}
