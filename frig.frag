uniform sampler2D texture;

varying vec4 texCoord;

void main(void){

	//side face
	//vec2 coord2d = vec2((fract(texCoord.x) + texCoord.w) / 16.0, texCoord.z);
	//top or bottom face
	vec2 coord2d = vec2((fract(texCoord.x) + texCoord.w) / 16.0f, fract(texCoord.z));

	vec4 color = texture2D(texture, coord2d);

    gl_FragColor = color;
}
