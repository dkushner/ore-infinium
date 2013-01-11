//#version 120

/*
uniform sampler2D texture;

varying vec4 fragColor;
varying vec2 fragTexCoord;

void main (void)
{
    gl_FragColor = fragColor * texture2D( texture, fragTexCoord );
}
*/
/*
varying vec2 frag_texcoord;
varying vec4 frag_color;

uniform sampler2D sampler;

void main() {
    gl_FragColor = frag_color * texture2D(sampler,frag_texcoord);
}
*/

#version 330

in vec4 fcolor;
out vec4 FragColor;
void main() {
    FragColor = fcolor;
}
