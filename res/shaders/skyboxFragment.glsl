#version 130

uniform sampler2D uEnvironment;

in vec3 vWorldPos;
out vec4 fragColor;

const float PI = 3.14159265359;

vec2 norm2equirectangular(vec3 dir)
{
    vec2 uv = vec2(atan(-dir.z, dir.x) / (2. * PI) + 0.5, acos(dir.y) / PI);
    return uv;
}

void main()
{
    fragColor = texture(uEnvironment, norm2equirectangular(normalize(vWorldPos)));
}
