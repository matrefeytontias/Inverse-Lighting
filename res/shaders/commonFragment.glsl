const float PI = 3.14159265359;

vec3 spherical2cartesian(float x, float y)
{
    return vec3(cos(x) * sin(y), cos(y), sin(-x) * sin(y));
}

vec2 norm2equi(vec3 dir)
{
    vec2 uv = vec2(atan(-dir.z, dir.x) / (2. * PI) + 0.5, acos(dir.y) / PI);
    return uv;
}
