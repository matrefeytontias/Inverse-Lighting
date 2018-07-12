#version 130

uniform mat4 uP;
uniform mat4 uV;
uniform vec3 uCameraPos;

in vec3 NORMAL;
in vec3 POSITION;
in vec2 TEXCOORD_0;
out vec3 vNormal;
out vec3 vPos;
out vec3 vRay;
out vec2 vTexCoord;

void main()
{
    vNormal = NORMAL;
    vPos = POSITION;
    vRay = POSITION - uCameraPos;
    vTexCoord = TEXCOORD_0;
    gl_Position = uP * uV * vec4(POSITION, 1.);
}
