#version 130

uniform mat4 mvp;

in vec2 position;
in vec3 texcoord;

in vec4 color;

out vec3 frag_texcoord;
out vec4 frag_color;

void main() {
    gl_Position = mvp * vec4(position, 0.0, 1.0);
    frag_texcoord = texcoord;
    frag_color = color;
}


uniform mat4 mvp;

in float lightRadius;
in vec2 lightPos;
in vec2 lineP0;
in vec2 lineP1;
in vec4 shadowColor = float4(0, 0, 0, 1);


VS_OUTPUT MyVS(
    float4 pos  : POSITION )
{
    float vVsP = pos.x; // v vs. p
    float zeroVsOne = pos.y; // v0 vs. v5, or p0 vs. p1

    // Use "lightspace", a coordinate system where the light source is at the origin
    vec2 lineP0LightSpace = lineP0 - lightPos;
    vec2 lineP1LightSpace = lineP1 - lightPos;

    float theta0 = atan2(lineP0LightSpace.y, lineP0LightSpace.x);
    float theta1 = atan2(lineP1LightSpace.y, lineP1LightSpace.x);

    // Make sure our lerp takes the shortest path from theta0 to theta1
    // (make sure theta0 and theta1 are <= PI apart from each other)
    if( theta1 - theta0 > 3.14159 ) {
        theta1 -= 2 * 3.14159;
    }

    if( theta0 - theta1 > 3.14159 ) {
        theta0 -= 2 * 3.14159;
    }

    float theta = lerp(theta0, theta1, zeroVsOne);
    vec2 vLightSpace = vec2(lightRadius * cos(theta), lightRadius * sin(theta));

    // clamp p0 and p1 to the light's radius...otherwise things get weird
    if( distance( lineP0LightSpace, float2(0,0) ) > lightRadius ) {
        lineP0LightSpace = vec2( lightRadius * cos(theta0), lightRadius * sin(theta0) );
    }

    if( distance( lineP1LightSpace, float2(0,0) ) > lightRadius ) {
        lineP1LightSpace = vec2( lightRadius * cos(theta1), lightRadius * sin(theta1) );
    }

    vec2 pLightSpace = lerp(lineP0LightSpace, lineP1LightSpace, zeroVsOne);

    // Translate back from "lightspace" to worldspace

    vec2 v = vLightSpace + lightPos;
    vec2 p = pLightSpace + lightPos;

    vec2 mergedPos = lerp(v, p, vVsP);
    vec4 mergedPos4 = vec4(mergedPos.x, mergedPos.y, 0, 1);

    Out.position = mul(mergedPos4, viewProj);

    return Out;
}
