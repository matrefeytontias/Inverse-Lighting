attribute vec2 POSITION;
attribute vec2 TEXCOORD0;

varying vec2 uv;

void main()
{
    uv = TEXCOORD0;
    gl_Position = vec4(POSITION, 0., 1.);
}
