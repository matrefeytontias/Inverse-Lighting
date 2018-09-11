#version 130

in vec2 POSITION;
out vec2 vSpherical;
out vec2 vuv;

const float PI = 3.14159265359;

void main()
{
    float phi = POSITION.x * PI, theta = (POSITION.y + 1.) / 2. * PI;
    vuv = (POSITION + 1.) / 2.;
    vSpherical = vec2(phi, theta);
    gl_Position = vec4(POSITION, 1., 1.);
}
