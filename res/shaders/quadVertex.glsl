#version 140

uniform mat4 uV;
uniform mat4 uInvP;

in vec2 POSITION;
out vec3 vWorldPos;

void main()
{
    vec4 pos = vec4(POSITION, 1., 1.);
    vWorldPos = inverse(mat3(uV)) * (uInvP * pos).xyz;
    gl_Position = pos;
}
