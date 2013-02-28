#version 130

in vec2 frag_texcoord;
in vec4 frag_color;

//uniform sampler2D tex;

out vec4 fragColor;

void main() {
    float distance = 100 * distance(frag_texcoord.xy, vec2(0.5, 0.5));
    fragColor = vec4(frag_color.rgb, (1 / distance));
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
