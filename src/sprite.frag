#version 130

uniform sampler2D tex;
in vec2 ftexcoord;

out vec4 FragColor;

void main() {
    FragColor = texture(tex, ftexcoord);
}
