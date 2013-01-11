/*
#version 120

attribute vec2 position;

attribute vec2 texcoord;
attribute vec4 color;

varying vec4 frag_color;
varying vec2 frag_texcoord;

void main()
{
    frag_texcoord = texcoord;
    frag_color = color;

//    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vposition.xyz, 1.0);
gl_Position = vec4(position, 0.0, 1.0);
}
*/
#version 120
attribute vec4 vposition;
attribute vec4 vcolor;

varying vec4 fcolor;
void main() {
    fcolor = vcolor;
    gl_Position = vposition;
}