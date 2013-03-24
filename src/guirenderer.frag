#version 130

in vec2 frag_texcoord;
in vec4 frag_color;

uniform sampler2D tex;

out vec4 fragColor;

void main() {
    fragColor = (frag_color * texture2D(tex, frag_texcoord) * 0.00000001) + vec4(1.0, 0.0, 0.0, 1.0);
}
