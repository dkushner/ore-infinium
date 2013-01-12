/////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////
#include <GL/glew.h>
#include <GL/glut.h>

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;


GLuint prog = 0;
GLuint tex = 0;
const unsigned int SPRITE_COUNT = 20;



/////////////////////////////////////////////////////////////////////
// Sprite functionality
/////////////////////////////////////////////////////////////////////

struct SpriteVertex
{
    SpriteVertex() 
        : x(0), y(0)
        , xoff(0.0), yoff(0.0)
        , width(1.0), height(1.0)
        , s(0.0), t(0.0)
        , rotation(0.0)
        , r(255), g(255), b(255), a(255)
    {}
    void SetPos( float _x, float _y ) { x = _x; y = _y; }
    void SetTex( float _s, float _t ) { s = _s; t = _t; }

    float x, y;
    float xoff, yoff;
    float width, height;
    float s, t;
    float rotation;
    unsigned char r, g, b, a;
};


struct SpriteState
{
    SpriteState() 
        : xvel(0)
        , yvel(0)
        , rotvel(0)
    {}

    float xvel, yvel;   // units per second
    float rotvel;       // radians per second
};


struct SpriteWrangler
{
    SpriteWrangler( unsigned int aSpriteCount, unsigned int aWidth, unsigned int aHeight )
    {
        verts.resize( aSpriteCount * 4 );
        states.resize( aSpriteCount );

        for( size_t i = 0; i < states.size(); ++i )
        {
            states[i].xvel = randf( -30, 30 );
            states[i].yvel = randf( -30, 30 );
            states[i].rotvel = randf( -1, 1 );

            SpriteVertex vert;
            vert.xoff = randf( 0, aWidth );
            vert.yoff = randf( 0, aHeight );
            vert.width = randf( 20, 60 );
            vert.height = randf( 20, 60 );
            vert.rotation = randf( 0, 2 * 3.14159 );
            vert.r = (unsigned char)randf( 64, 255 );
            vert.g = (unsigned char)randf( 64, 255 );
            vert.b = (unsigned char)randf( 64, 255 );
            vert.a = 255;

            vert.SetPos( -1, -1 );
            vert.SetTex( 0, 0 );
            verts[i*4 + 0] = vert;            
            vert.SetPos( 1, -1 );
            vert.SetTex( 1, 0 );
            verts[i*4 + 1] = vert;            
            vert.SetPos( 1, 1 );
            vert.SetTex( 1, 1 );
            verts[i*4 + 2] = vert;            
            vert.SetPos( -1, 1 );
            vert.SetTex( 0, 1 );
            verts[i*4 + 3] = vert;            
        }
    }

    void Update( float dt, unsigned int aWidth, unsigned int aHeight )
    {
        for( size_t i = 0; i < states.size(); ++i )
        {
            SpriteVertex& vert = verts[i*4];
            vert.xoff += states[i].xvel * dt;
            vert.yoff += states[i].yvel * dt;
            vert.rotation += states[i].rotvel * dt;

            vert.xoff = wrap( vert.xoff, 0, aWidth );
            vert.yoff = wrap( vert.yoff, 0, aHeight );
            vert.rotation = wrap( vert.rotation, 0, 2 * 3.14159 );

            verts[i*4 + 1].xoff = verts[i*4 + 2].xoff = verts[i*4 + 3].xoff = vert.xoff;
            verts[i*4 + 1].yoff = verts[i*4 + 2].yoff = verts[i*4 + 3].yoff = vert.yoff;
            verts[i*4 + 1].rotation = verts[i*4 + 2].rotation = verts[i*4 + 3].rotation = vert.rotation;
        }    
    }

    vector< SpriteVertex > verts;
    vector< SpriteState > states;
};



/////////////////////////////////////////////////////////////////////
// Main program
/////////////////////////////////////////////////////////////////////

void init()
{
    GLenum glewError = glewInit();
    if( GLEW_OK != glewError )
    {
        stringstream err;
        err << "GLEW error: " << glewGetErrorString(glewError) << endl;
        throw std::logic_error( err.str() );      
    }    

    cout << "GLEW_VERSION : " << glewGetString(GLEW_VERSION) << endl;
    cout << "GL_VERSION   : " << glGetString(GL_VERSION) << endl;
    cout << "GLSL VERSION : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
    cout << "GL_VENDOR    : " << glGetString(GL_VENDOR) << endl;
    cout << "GL_RENDERER  : " << glGetString(GL_RENDERER) << endl;    

    if( !GLEW_VERSION_2_1 )
    {
        stringstream err;
        err << "OpenGL 2.1 or better required for GLSL support." << endl;
        throw std::logic_error( err.str() ); 
    }

    // load shaders
    string vert = LoadFile( "vert.glsl" );
    string frag = LoadFile( "frag.glsl" );
    prog = CreateProgram( vert, frag );

    // create random texture
    unsigned char buffer[ 32 * 32 * 4 ];
    for( unsigned int i = 0; i < 32*32; ++i )
    {
        buffer[i*4 + 0] = rand()%255;
        buffer[i*4 + 1] = rand()%255;
        buffer[i*4 + 2] = rand()%255;
        buffer[i*4 + 3] = 255;
    }

    // upload texture data
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
    glTexImage2D(GL_TEXTURE_2D, 0, 4, 32, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
}


void display()
{
    static SpriteWrangler wrangler( SPRITE_COUNT, 800, 600 );

    static float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float newTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float frameTime = newTime - currentTime;
    currentTime = newTime;

    wrangler.Update( frameTime, 800, 600 );
    vector< SpriteVertex >& verts = wrangler.verts;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glUseProgram( prog );

    // load uniforms
    GLfloat projection[16];
    glGetFloatv( GL_PROJECTION_MATRIX, projection );
    GLint projection_loc = glGetUniformLocation( prog, "projection" );
    glUniformMatrix4fv( projection_loc, 1, GL_FALSE, projection );

    GLfloat modelview[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, modelview );
    GLint modelview_loc = glGetUniformLocation( prog, "modelview" );
    glUniformMatrix4fv( modelview_loc, 1, GL_FALSE, modelview );

    GLint texture_loc = glGetUniformLocation( prog, "texture" );
    glUniform1i( texture_loc, 0 );    
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, tex );


    GLint texcoord_loc = glGetAttribLocation( prog, "texcoord" );
    glVertexAttribPointer( texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), &verts[0].s );
    glEnableVertexAttribArray( texcoord_loc );

    // render
    glDrawArrays( GL_QUADS, 0, SPRITE_COUNT * 4 );

    glDisableVertexAttribArray( texcoord_loc );

    glutSwapBuffers();
}

