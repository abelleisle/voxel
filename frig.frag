uniform sampler2D texture;

varying vec2 texCoord;

void main(void){

    vec4 color = texture2D(texture, vec2(texCoord.x,1-texCoord.y));

    if(color.a <= .1)
        discard;

    gl_FragColor = color;
}
