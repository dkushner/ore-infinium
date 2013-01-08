#version 120

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

in vec4 vposition;

attribute vec2 texcoord;

varying vec4 fragColor;
varying vec2 fragTexCoord;

void main(void)
{
    fragColor = color;
    fragTexCoord = texcoord;

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vposition.xyz, 1.0);
}
