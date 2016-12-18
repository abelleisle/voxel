uniform sampler2D texture;

varying vec4 texCoord;

void main(void){

	vec2 coord2d = vec2((fract(texCoord.x) + texCoord.w) / 16.0, texCoord.z);
	vec4 color = texture2D(texture, coord2d);

    gl_FragColor = color;
}
