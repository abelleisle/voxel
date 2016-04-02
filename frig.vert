attribute vec3 coord;
attribute vec2 textureCoord;

uniform mat4 mvp;

varying vec2 texCoord;

void main(void) {
    texCoord = textureCoord;
    gl_Position = mvp * vec4(coord.xyz, 1.0);
}
