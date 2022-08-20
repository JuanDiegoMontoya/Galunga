#version 460 core

struct ObjectUniforms
{
  mat3x2 transform;
  uint spriteIndex;
  uint tint4x8;
};

layout(std140, binding = 0) uniform CameraBuffer
{
  mat4 viewProj;
}cameraUniforms;

layout(std430, binding = 0) readonly restrict buffer UniformBuffer
{
  ObjectUniforms objectUniforms[];
};

layout(location = 0) out Varyings
{
  vec2 texCoord;
  flat uint spriteIndex;
  flat uint tint4x8;
}vs_out;

// vertices in [0, 1]
vec2 CreateQuad(in uint vertexID) // triangle fan
{
  uint b = 1 << vertexID;
  return vec2((0x9 & b) != 0, (0x3 & b) != 0);
}

void main()
{
  vs_out.texCoord = CreateQuad(gl_VertexID);
  vec2 aPos = vs_out.texCoord - 0.5;

  ObjectUniforms object = objectUniforms[gl_InstanceID + gl_BaseInstance];
  vs_out.spriteIndex = object.spriteIndex;
  vs_out.tint4x8 = object.tint4x8;
  mat3x2 transform3x2 = object.transform;
  vec2 wPos = transform3x2 * vec3(aPos, 1.0);

  gl_Position = cameraUniforms.viewProj * vec4(wPos, 0.5, 1.0);
}