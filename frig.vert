attribute vec4 coord;

uniform mat4 mvp;

varying vec4 texCoord;

void main(void) {
    texCoord = coord;
    gl_Position = mvp * vec4(coord.xyz, 1.0);
}
