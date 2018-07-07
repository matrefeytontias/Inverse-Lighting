#version 130

uniform mat4 uP;
uniform float uTime;

in vec3 NORMAL;
in vec3 POSITION;
in vec2 TEXCOORD_0;
out vec3 vNormal;
out vec2 vTexCoord;

void main()
{
    vTexCoord = TEXCOORD_0;
    mat3 rot = mat3(1., 0., 0., 0., cos(uTime), -sin(uTime), 0., sin(uTime), cos(uTime));
    vNormal = rot * NORMAL;
    gl_Position = uP * vec4(rot * POSITION - vec3(0., 0., 5.), 1.);
}
