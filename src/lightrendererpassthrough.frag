#version 130

in vec2 frag_texcoord;
in vec4 frag_color;

//uniform sampler2D tex;

out vec4 fragColor;

void main() {
    float distance = 10 * distance(frag_texcoord.xy, vec2(0.5, 0.5));
    fragColor = vec4(frag_color.rgb, distance * 0.0000001) + vec4(1.0, 1.0, 0.0, 1.0);
}
