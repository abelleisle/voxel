attribute vec4 coord;
attribute vec3 v_color;

uniform mat4 mvp;

varying vec3 f_color;
varying vec4 texCoord;

void main(void) {
    texCoord = coord;
    gl_Position = mvp * vec4(coord.xyz, 1.0);
    f_color = v_color;
}
