#version 130

in vec3 frag_texcoord;
in vec4 frag_color;

uniform sampler2DArray tex;

out vec4 fragColor;

void main() {
    vec4 texel = texture(tex, frag_texcoord);
    fragColor = (frag_color.rgba) * vec4(texel.rgb, texel.a);
}

/*

    ivec2 tileCoordinate;
    tileCoordinate.x = (int(currentPixel.r * 255.0) ) * TILE_SIZE.x + (screen_coordinates.x % TILE_SIZE.x);
    tileCoordinate.y = (screen_coordinates.y) % TILE_SIZE.y;
*/
