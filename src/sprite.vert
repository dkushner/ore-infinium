uniform mat4 projection;
uniform mat4 modelview;

attribute vec2 position;
attribute vec2 offset;
attribute vec2 texcoord;
attribute vec2 scale;
attribute float rotation;
attribute vec4 color;

varying vec4 fragColor;
varying vec2 fragTexCoord;

void main(void)
{
    fragColor = color;
    fragTexCoord = texcoord;

    // column 1,
    // column 2,
    // column 3
    mat3 scale_mat = mat3
        ( 
        0.5*scale.x,    0.0,            0.0, 
        0.0,            0.5*scale.y,    0.0,
        0.0,            0.0,            1.0
        );  

    mat3 rotate_mat = mat3
        ( 
        cos(rotation),  sin(rotation),  0.0, 
        -sin(rotation), cos(rotation),  0.0,
        0.0,            0.0,            1.0
        );

    mat3 translate_mat = mat3
        ( 
        1.0,        0.0,        0.0, 
        0.0,        1.0,        0.0,
        offset.x,   offset.y,   1.0
        );

    vec3 xformed = translate_mat * rotate_mat * scale_mat * vec3( position, 1.0 );

    gl_Position = projection * modelview * vec4( xformed, 1.0 );
}
