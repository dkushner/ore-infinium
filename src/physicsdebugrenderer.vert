#version 130

uniform mat4 mvp;

in vec2 position;

//in vec4 color;

void main() {
    gl_Position = mvp * vec4(position, 0.0, 1.0);
//    frag_color = color;
}
