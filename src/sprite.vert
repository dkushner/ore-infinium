#version 130

in vec4 vposition;
in vec2 vtexcoord;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out vec2 ftexcoord;

void main() {
    ftexcoord = vtexcoord;
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vposition.xyz, 1.0);
}
