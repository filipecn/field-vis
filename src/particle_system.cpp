#include "particle_system.h"

ParticleSystem::ParticleSystem(unsigned n, ProceduralField *p) : particleCount(n) {
  velocity.reset(p);
  resolution = velocity->resolution();
  init();
}

ParticleSystem::ParticleSystem(unsigned n, ponos::vec2 res) : particleCount(n), resolution(res) {
  init();
}

bool ParticleSystem::solve() {
  velocity->bindX(GL_TEXTURE0);
  velocity->bindY(GL_TEXTURE1);
  shader->setUniform("vx", 0);
  shader->setUniform("vy", 1);
  shader->setUniform("resolution", resolution);
  return shader->compute();
}
void ParticleSystem::draw() {
  pb->bind();
  aergia::glColor(aergia::COLOR_BLUE);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
  glDrawArrays(GL_POINTS, 0, particleCount);
}

void ParticleSystem::init() {
  positions.resize(particleCount);
  lifeSpan.resize(particleCount, -1.f);
  pBuffer.reset(new aergia::StorageBuffer(particleCount * sizeof(Vec2), &positions[0]));
  lBuffer.reset(new aergia::StorageBuffer(particleCount * sizeof(float), &lifeSpan[0]));
  pBuffer->bind();
  lBuffer->bind();
  shader.reset(new aergia::ComputeShader(source));
  shader->setGroupSize(ponos::uivec3(particleCount / 128, 1, 1));
  shader->setBuffer("PositionBuffer", pBuffer->id(), 0);
  shader->setBuffer("LifeSpanBuffer", lBuffer->id(), 1);
  aergia::BufferDescriptor bufferDescriptor;
  bufferDescriptor.type = GL_ARRAY_BUFFER;
  bufferDescriptor.addAttribute("position", 2, 0, GL_FLOAT);
  pb.reset(new aergia::VertexBuffer(pBuffer->id(), bufferDescriptor));
}
