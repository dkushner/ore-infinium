//#version 130
//uniform vec2 offset;

//uniform sampler2D tilemap_pixels;

//uniform sampler2D tile_types_super_texture;

//ivec2 offset = ivec2(5.0, 5.0);

//FIXME: stop hardcoding ..
//ivec2 TILE_SIZE = ivec2(16, 16);

#version 130

in vec3 frag_texcoord;
in vec4 frag_color;

uniform sampler2DArray tex;

out vec4 fragColor;

void main() {
    fragColor = (frag_color.rgba * 0.00001) + textureOffset(tex, frag_texcoord, ivec2(2, 2));
}

//void main()
//{
/*
    ivec2 tilemap_size = textureSize(tile_types_super_texture, 0);

    ivec2 screen_coordinates = ivec2(gl_FragCoord.x + int(offset.x), gl_FragCoord.y - int(offset.y) + TILE_SIZE.y);

    // find the pixel (RGBA) values in the tilemap pixel representation that is what we're
    // currently interested in.
    vec4 currentPixel = texelFetch(tilemap_pixels, screen_coordinates / TILE_SIZE, 0);

    ivec2 tileCoordinate;
    tileCoordinate.x = (int(currentPixel.r * 255.0) ) * TILE_SIZE.x + (screen_coordinates.x % TILE_SIZE.x);
    tileCoordinate.y = (screen_coordinates.y) % TILE_SIZE.y;

    vec4 tileColor = texelFetch(tile_types_super_texture, tileCoordinate, 0);
*/
//    vec4 tileColor = texelFetch(tile_types_super_texture, ivec2(22, 12), 0);
//    vec4 tileColor = texture2D(tile_types_super_texture, vec2(0.5, 0.5), 0);
 //   gl_FragColor = tileColor;

/*
gl_FragColor.r = 1.0;
gl_FragColor.g = 0.0;
gl_FragColor.b = 0.0;
gl_FragColor.a = 1.0;
*/
//}
