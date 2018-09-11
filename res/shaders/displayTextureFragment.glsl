uniform sampler2D uTex;

varying vec2 uv;

void main()
{
    gl_FragColor = texture2D(uTex, uv);
}
