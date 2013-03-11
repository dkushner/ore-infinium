#version 130

in vec2 frag_texcoord;
in vec4 frag_color;

uniform sampler2D lightMap;
uniform float time;

out vec4 fragColor;

void main() {
    vec2 texcoord = frag_texcoord;

    //transform the texcoord by several pixels, somewhat randomly based on time
    //this makes the light flicker and sway a bit from origin, and around radius
    texcoord *= sin(time * 3) * sin((time + 2.0)* 5) * sin((time + 3) * 7) * .05 + 1;

    vec4 light = texture2D(lightMap, texcoord) * (frag_color);

    //modify the actual light values to affect the light itself, and make it dim/brighten with time random
    light *= sin(time * 3) * sin((time + 2.0)* 5) * sin((time + 3) * 7) * .15 + 1;

    fragColor = light;
}
