#version 450
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inNormal;
layout (location = 4) in vec3 inTangent;

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec2 fragTexCoord;
layout (location = 2) out vec3 fragNormal;
layout (location = 3) out vec3 fragTangent;
layout (location = 4) out vec4 fragPos;

layout (binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

void main() {
    mat4 worldMat = ubo.proj * ubo.view * ubo.model;
    fragPos = worldMat * vec4(inPosition, 1);
    gl_Position = fragPos;
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    fragNormal = normalize((ubo.model * vec4(inNormal,0)).xyz);
    fragTangent = normalize((ubo.model * vec4(inTangent,0)).xyz);
}