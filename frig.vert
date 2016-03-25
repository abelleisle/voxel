#version 120

varying vec2 TexCoord;

void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0;
    TexCoord = gl_TexCoord[0].xy;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
