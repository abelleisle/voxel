varying vec3 f_color;
uniform sampler2D texture;

varying vec4 texCoord;

void main(void){
    vec2 coord2d;
    float id;
    float side = (texCoord.w*10)-10;
    modf(texCoord.w, id);

    coord2d = vec2(id*16,side*16);
    vec4 color = texture2D(texture, coord2d);

    gl_FragColor = color;
}
