#version 130

uniform sampler2D uEnvironment;

in vec2 vSpherical;
in vec2 vuv;
out vec4 fragColor;

const float PI = 3.14159265359, sampleDelta = 0.01;

vec2 norm2equi(vec3 dir);
vec3 spherical2cartesian(float x, float y);

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
            irradiance += texture(uEnvironment, norm2equi(normalize(sampleVec))).rgb * cos(theta) * sin(theta);
            nrSamples += 1.;
        }
    }
    
    fragColor = vec4(irradiance * PI / nrSamples, 1.);
}
