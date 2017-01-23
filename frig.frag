uniform sampler2D texture;
varying vec4 texCoord;

void main(void){

	vec2 coord2d;

	if (texCoord.w < 0.0) { //side face
		coord2d = vec2((fract(texCoord.x + texCoord.z) - texCoord.w) / 16.0, fract(-texCoord.y));
	} else { //top or bottom face
		coord2d = vec2((fract(texCoord.x) + texCoord.w) / 16.0, fract(texCoord.z));
	}

	vec4 color = texture2D(texture, coord2d);
    
	gl_FragColor = color;
}
