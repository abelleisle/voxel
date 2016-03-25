#version 120

uniform sampler3D sampler;

void main()
{
    vec4 tex = texture3D(sampler, gl_TexCoord[0].stp);
    gl_FragColor = vec4(1.0f,1.0f,1.0f,1.0f);//tex;
}
