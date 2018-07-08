#version 130

uniform sampler2D uAlbedoMap;
uniform sampler2D uOcclusionMap;

in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vRay;
out vec4 fragColor;

void main()
{
    fragColor = texture(uAlbedoMap, vTexCoord) * texture(uOcclusionMap, vTexCoord)
        * max(0., -dot(normalize(vNormal), normalize(vRay)));
}
