#version 130

in vec4 frag_color;

out vec4 fragColor;

void main() {
    fragColor = (frag_color * 0.0000001) + vec4(1.0, 0.0, 0.0, 1.0);
}
