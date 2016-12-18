uniform sampler2D texture;

varying vec4 texCoord;

void main(void){

    //vec4 color = texture2D(texture, vec2(texCoord.x,1-texCoord.y));
	vec4 color = vec4(1.0, 0.5, 0.5, 1.0);

    gl_FragColor = color;
}
