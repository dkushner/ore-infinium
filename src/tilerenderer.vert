#version 130

uniform mat4 mvp;

uniform vec2 offset;

in vec2 position;
in vec3 texcoord;

in vec4 color;

out vec3 frag_texcoord;
out vec4 frag_color;

void main() {
    gl_Position = mvp * vec4(position - offset, 0.0, 1.0);
    frag_texcoord = texcoord;
    frag_color = color;
}
