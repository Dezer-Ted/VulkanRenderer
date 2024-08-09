#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragTangent;
layout(location = 4) in vec4 fragPos;

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform sampler2D normalSampler;
layout(binding = 3) uniform sampler2D roughnessSampler;
layout(binding = 4) uniform sampler2D specularSampler;
layout(push_constant) uniform constants {
    vec3 cameraPos;
    int enableNormals;
    int shadingMode;
}PushConstants;

layout(location = 0) out vec4 outColor;
vec3 lightDirection = vec3(0.577f,-0.577f,0.577f);
float saturate(float value) {
    if(value < 0) {
        return 0;
    }
    else if(value > 1) {
        return 1;
    }
    else {
        return value;
    }
}

vec3 Phong(float specularReflectance, float exp,vec3 viewDirection,vec3 normal) {
    vec3 specularReflectanceVector = vec3(specularReflectance,specularReflectance,specularReflectance);
    if(exp == 0) {
        return specularReflectanceVector;
    }
    vec3 normalLightReflectance = reflect(lightDirection,normal);
    const float cos = saturate(dot(normalLightReflectance,normal));
    if(exp == 1) {
        return specularReflectanceVector * cos;
    }
    else if(exp == 2) {
        return specularReflectanceVector * (cos*cos);
    }
    return specularReflectanceVector* pow(cos,exp);
}
void main() {
    vec3 color;
    vec4 normal;
    float pi = 3.14159;
    float shininess = 25.f;
    float lightIntensity = 7.f;
    vec3 ambientOcclusion = vec3(0.003f,0.003f,0.003f);
    if(PushConstants.enableNormals == 1) {
        normal = texture(normalSampler,fragTexCoord);
        normal = vec4( 2 * vec3(normal) - vec3(1, 1, 1),1);
        vec3 binormal = cross(fragNormal, fragTangent);
        mat3 tangentSpaceAxis = mat3(fragTangent, binormal, fragNormal);
        normal = vec4(normalize(tangentSpaceAxis * normal.xyz),1);
    }
    else {
        normal = vec4(fragNormal,1);
    }
    float observedArea = dot(vec3(normal),-lightDirection);
    if(observedArea < 0) {
        outColor = vec4(0.f,0.f,0.f,1.f);
        return;
    }
    vec4 albedoSample = texture(texSampler,fragTexCoord);
    float roughness = texture(roughnessSampler,fragTexCoord).r;
    vec3 viewDirection = normalize(PushConstants.cameraPos-vec3(fragPos));
    vec3 specular = Phong(texture(specularSampler,fragTexCoord).r,roughness*shininess,viewDirection,vec3(normal));
    vec3 diffuse = vec3(albedoSample) * lightIntensity / pi;


    if(PushConstants.shadingMode == 0) {
        outColor = vec4(diffuse,1);
        return;
    }
    else if( PushConstants.shadingMode == 1) {
        outColor = normal;
        return;
    }
    else if(PushConstants.shadingMode == 2) {
        outColor = vec4(specular,1);
        return;
    }
    else {
        outColor = vec4(observedArea * diffuse + specular + ambientOcclusion ,1);
        return;
    }
}
