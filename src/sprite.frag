#version 120

uniform sampler2D texture;

varying vec4 fragColor;
varying vec2 fragTexCoord;

void main (void)  
{     
    gl_FragColor = fragColor * texture2D( texture, fragTexCoord );    
}