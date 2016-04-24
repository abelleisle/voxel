attribute vec4 coord;
varying vec2 tex_coord;

void main(void) {
    gl_Position = vec4(coord.xy, 0, 1);
    tex_coord = coord.zw;
}
