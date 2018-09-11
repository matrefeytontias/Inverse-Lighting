#version 130

uniform sampler2D uEnvironment;

in vec2 vSpherical;
in vec2 vuv;
out vec4 fragColor;

const float PI = 3.14159265359,
    sampleDelta = 0.025;

vec2 norm2equirectangular(vec3 dir)
{
    vec2 uv = vec2(atan(-dir.z, dir.x) / (2. * PI) + 0.5, acos(dir.y) / PI);
    return uv;
}

vec3 spherical2cartesian(float x, float y)
{
    return vec3(cos(x) * sin(y), cos(y), sin(-x) * sin(y));
}

void main()
{
    vec3 irradiance = vec3(0.),
        normal = spherical2cartesian(vSpherical.x, vSpherical.y),
        up = vec3(0., 1., 0.),
        right = cross(up, normal);
    up = cross(normal, right);
    float nrSamples = 0.;
    
    for(float phi = 0.; phi < 2. * PI; phi += sampleDelta)
    {
        for(float theta = 0.; theta < PI / 2.; theta += sampleDelta)
        {
            vec3 tangent = spherical2cartesian(phi, theta),
                sampleVec = tangent.x * up + tangent.y * normal + tangent.z * right;
            irradiance += texture(uEnvironment, norm2equirectangular(normalize(sampleVec))).rgb * cos(theta) * sin(theta);
            nrSamples += 1.;
        }
    }
    
    fragColor = vec4(irradiance / nrSamples, 1.);
    // fragColor = texture(uEnvironment, norm2equirectangular(normal));
    // fragColor = texture(uEnvironment, vuv);
}
