#version 130

uniform sampler2D uAlbedoMap;
uniform sampler2D uMetallicRoughness;
uniform sampler2D uNormalMap;
uniform sampler2D uEmissiveMap;
uniform sampler2D uOcclusionMap;

uniform sampler2D uIrradianceMap;

const float PI = 3.14159265359;

in vec3 vNormal;
in vec3 vPos;
in vec3 vRay;
in vec2 vTexCoord;
out vec3 fragColor;

const vec3 dielectricSpecular = vec3(.04), black = vec3(0.);

vec2 norm2equi(vec3 dir);

vec3 brdf(vec3 v, vec3 l, vec3 n, vec3 albedo, vec2 metalRough, vec3 cdiff, vec3 F0)
{
    vec3 h = normalize(v + l);
    float alpha = metalRough.g * metalRough.g,
        alpha2 = alpha * alpha,
        nh = max(1e-5, dot(n, h)),
        nl = max(1e-5, dot(n, l)),
        nv = max(1e-5, dot(n, v)),
        vh = max(1e-5, dot(v, h));
    
    // Taken from Unreal's SIGGRAPH 2013 "Real Shading" presentation
    float temp = nh * nh * (alpha2 - 1.) + 1.;
    float D = alpha2 / (PI * temp * temp);
    
    float k = (metalRough.g * metalRough.g + 1.) / 8.;
    float G1 = nv / (nv * (1. - k) + k),
        G2 = nl / (nl * (1. - k) + k);
    float G = G1 * G2;
    
    vec3 F = F0 + (1. - F0) * exp2((-5.55473 * vh - 6.98316) * vh);
    
    return (1. - F) * cdiff / PI + F * G * D / (4 * nl * nv);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1. - roughness), F0) - F0) * exp2((-5.55473 * cosTheta - 6.98316) * cosTheta);
}

// Get the cotangent frame as a 3x3 matrix for tangent space calculations
// From http://www.thetenthplanet.de/archives/1180
mat3 cotangentFrame(vec3 n, vec3 p, vec2 uv)
{
    // Get the triangle's edge vectors and delta UV
    vec3 dp1 = dFdx(p), dp2 = dFdy(p);
    vec2 duv1 = dFdx(uv), duv2 = dFdy(uv);
    
    // Get the tangent and bitangent as covectors
    vec3 dp1perp = cross(n, dp1), dp2perp = cross(dp2, n);
    vec3 t = dp2perp * duv1.x + dp1perp * duv2.x,
        b = dp2perp * duv1.y + dp1perp * duv2.y;
    
    // Construct a scale-invariant frame to avoid determinant calculation
    float invmax = inversesqrt(max(dot(t, t), dot(b, b)));
    return mat3(t * invmax, b * invmax, n);
}

void main()
{
    vec2 metalRough = texture(uMetallicRoughness, vTexCoord).bg;
    vec3 v = normalize(vRay),
        n = normalize(vNormal),
        albedo = texture(uAlbedoMap, vTexCoord).rgb,
    // Taken from Khronos' glTF 2.0 specification, Appendix B
        cdiff = mix(albedo * (1. - dielectricSpecular.r), black, metalRough.r),
        F0 = mix(dielectricSpecular, albedo, metalRough.r);
    
    n = cotangentFrame(n, vPos, vTexCoord) * (texture(uNormalMap, vTexCoord).xyz * 2. - 1.);
    n = normalize(n);
    
    fragColor = texture(uEmissiveMap, vTexCoord).rgb +
        20. * brdf(v, v, n, albedo, metalRough, cdiff, F0) * max(0., -dot(n, v)) / (1. + dot(vRay, vRay))
        * texture(uOcclusionMap, vTexCoord).r;
}
