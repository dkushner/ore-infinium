uniform sampler2D backBuffer

uniform float r;
uniform float g;
uniform float b;
uniform float ratio;

void main()
{
    vec4 color;
    float avg, dr, dg, db;

    color = texture2D(backBuffer, gl_TexCoord[0].st);
    avg = (color.r + color.g + color.b) / 3.0;
    dr = avg * r;
    dg = avg * g;
    db = avg * b;
    color.r = color.r - (ratio * (color.r - dr));
    color.g = color.g - (ratio * (color.g - dg));
    color.b = color.b - (ratio * (color.b - db));

    gl_FragColor = color;
}
