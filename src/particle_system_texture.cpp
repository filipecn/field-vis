#include "particle_system_texture.h"

ParticleSystemTexture::ParticleSystemTexture(unsigned n) : particleCount(n) {
  const char *source =
      "#version 430\n" \
          "layout (local_size_x = 128, local_size_y = 1, local_size_z = 1) in;"\
          "layout (std430, binding = 0) buffer PositionBuffer {\n"\
          "\tvec2 positions[];\n"\
          "};\n"\
          "layout (std430, binding = 1) buffer LifeSpanBuffer {\n"\
          "\tfloat lifes[];\n"\
          "};\n"
          "uniform sampler2D vx;"
          "uniform sampler2D vy;"
          "highp float rand(vec2 co)\n"
          "{\n"
          " highp float a = 12.9898;\n"
          " highp float b = 78.233;\n"
          " highp float c = 43758.5453;\n"
          " highp float dt= dot(co.xy ,vec2(a,b));\n"
          " highp float sn= mod(dt,3.14);\n"
          " return fract(sin(sn) * c);\n"
          "}"
          "void main()\n"\
          "{\n"\
          "uint index = gl_GlobalInvocationID.x;\n"\
          "vec2 p = positions[index];\n"\
          "float l = lifes[index];\n"\
          "if(l <= 0) {\n"\
          " p = vec2(rand(vec2(0.5,0.5)), rand(vec2(1,1)) );"\
          " l = 1.0;"\
          "}"\
          "lifes[index] = l - 0.001;"\
          "positions[index] = vec2(texture(vx, vec2(0.5, 0.5)).r,2);\n"\
          "positions[index] = p;\n"\
"};\n";
  positions.resize(particleCount);
  lifeSpan.resize(particleCount, -1.f);
  pBuffer.reset(new aergia::StorageBuffer(particleCount * sizeof(Vec2), &positions[0]));
  lBuffer.reset(new aergia::StorageBuffer(particleCount * sizeof(float), &lifeSpan[0]));
  pBuffer->bind();
  lBuffer->bind();
  shader.reset(new aergia::ComputeShader(source));
  shader->setGroupSize(ponos::uivec3(particleCount, 1, 1));
  shader->setBuffer("PositionBuffer", pBuffer->id(), 0);
  shader->setBuffer("LifeSpanBuffer", lBuffer->id(), 1);
}

bool ParticleSystemTexture::solve() {
  shader->setUniform("vx", 0);
  shader->setUniform("vy", 1);
  bool r = shader->compute();
  lBuffer->read(&lifeSpan[0]);
  for (int i = 0; i < 100; i++)
    std::cout << lifeSpan[i] << " ";
  std::cout << std::endl;
  pBuffer->read(&positions[0]);
  for (int i = 0; i < 100; i++)
    std::cout << positions[i].x << " " << positions[i].y << std::endl;
  return r;
}
