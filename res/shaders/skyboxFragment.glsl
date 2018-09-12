#version 130

uniform sampler2D uEnvironment;

in vec3 vWorldPos;
out vec4 fragColor;

vec2 norm2equi(vec3 dir);

void main()
{
    fragColor = texture(uEnvironment, norm2equi(normalize(vWorldPos)));
}
