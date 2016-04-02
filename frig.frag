uniform sampler2D texture;

varying vec2 texCoord;

void main(void){

    vec4 color = texture2D(texture, vec2(texCoord.x,1-texCoord.y));

    gl_FragColor = color;
}
