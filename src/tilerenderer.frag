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

uniform sampler2DArray tileSheet;
uniform sampler2DArray normalMap;

out vec4 fragColor;

void main() {
    vec4 tile = texture(tileSheet, frag_texcoord);
    fragColor = (frag_color.rgba) * vec4(tile.rgb, tile.a);
}

/*
    "#version 130\n"

    "uniform sampler2DArray sampler;\n"
    "uniform vec3 lightPos;\n"

    "void main()\n{\n"
    "   vec2 size = textureSize(sampler, 0).xy;\n"

    "   vec3 N = texture(sampler, vec3(gl_FragCoord.xy / size, 0)).xyz * 2.0 - 1.0;\n"
    "   vec3 L = normalize(lightPos - gl_FragCoord.xyz);\n"
    "   vec4 I = vec4(1.0) * dot(L, N);\n"

    "   float dist = distance(gl_FragCoord.xyz, lightPos);\n"
    "   I *= 1.0 - min(1.5 * pow(dist / 512, 2.0), 1.0);\n"

    "   vec4 fragColor = texture(sampler, vec3(gl_FragCoord.xy / size, 1));\n"
    "   fragColor.rgb *= I.rgb;\n"
    "   gl_FragColor = fragColor;\n"
    "}\n";
*/





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
