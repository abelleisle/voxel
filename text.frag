uniform sampler2D tex;
uniform vec4 color;
varying vec2 tex_coord;

void main(void) {
    gl_FragColor = vec4(1, 1, 1, texture2D(tex,tex_coord).a) * color;
    //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
