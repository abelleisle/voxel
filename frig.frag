#version 120

uniform sampler2D sampler;

varying vec2 TexCoord;

void main()
{
    //vec4 tex = texture2D(sampler, gl_TexCoord[0].st);
    gl_FragColor = texture2D(sampler, gl_TexCoord[0].st);
    //gl_FragColor = vec4(1.0f,0.0f,0.0f,1.0f);
}
