#version 130

in vec3 frag_texcoord;
in vec4 frag_color;

//a sheet the size of 170x123, where each 16x16 is a tile that we want..presently only care about first (top-left)
uniform sampler2DArray tileSheet;
//a 16x16 normal map for our tile, only care about first
uniform sampler2DArray normalMap;

//presently a value of (0.5, 0.5, 0.0)
//uniform vec3 lightPos;

out vec4 fragColor;

//values used for shading algorithm...
uniform vec2 Resolution;      //resolution of screen
uniform vec3 LightPos;        //light position, normalized
uniform vec4 LightColor;      //light RGBA -- alpha is intensity
uniform vec4 AmbientColor;    //ambient RGBA -- alpha is intensity
uniform vec3 Falloff;         //attenuation coefficients


void main() {

    ///////////////////////////////////////////////////////////////////////////////

   //RGBA of our diffuse color
    vec4 DiffuseColor = texture(tileSheet, frag_texcoord);

    //RGB of our normal map
    vec3 NormalMap = texture(normalMap, frag_texcoord).rgb;

    //The delta position of light
    vec3 LightDir = vec3(LightPos.xy - (gl_FragCoord.xy / Resolution.xy), LightPos.z);

    //Correct for aspect ratio
    LightDir.x *= Resolution.x / Resolution.y;

    //Determine distance (used for attenuation) BEFORE we normalize our LightDir
    float D = length(LightDir);

    //normalize our vectors
    vec3 N = normalize(NormalMap * 2.0 - 1.0);
    vec3 L = normalize(LightDir);

    //Pre-multiply light color with intensity
    //Then perform "N dot L" to determine our diffuse term
    vec3 Diffuse = (LightColor.rgb * LightColor.a) * max(dot(N, L), 0.0);

    //pre-multiply ambient color with intensity
    vec3 Ambient = AmbientColor.rgb * AmbientColor.a;

    //calculate attenuation
    float Attenuation = 1.0 / ( Falloff.x + (Falloff.y*D) + (Falloff.z*D*D) );

    //the calculation which brings it all together
    vec3 Intensity = Ambient + Diffuse * Attenuation;
    vec3 FinalColor = DiffuseColor.rgb * Intensity;

    //////////////////////// TILE ///////////////////////////////////
    vec4 tile = texture(tileSheet, frag_texcoord);
    vec4 tileColor = (frag_color.rgba) * vec4(tile.rgb, tile.a);
    /////////////////////////////////////////////////////////////////

    fragColor = tileColor * vec4(FinalColor, DiffuseColor.a);
}

//    fragColor.rgb -= (1 - I.rgb);
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
//}
