#include "particle_system.h"

ParticleSystem::ParticleSystem(unsigned n, ProceduralField *p, unsigned w, unsigned h)
    : particleCount(n) {
  curBuffer = 0;
  velocity.reset(p);
  resolution = velocity->resolution();
  init();
  // create renderbuffers
  aergia::TextureAttributes attributes;
  attributes.target = GL_TEXTURE_2D;
  attributes.width = w;
  attributes.height = h;
  attributes.type = GL_UNSIGNED_BYTE;
  attributes.internalFormat = GL_RGBA;
  attributes.format = GL_RGBA;
  attributes.depth = 0;
  aergia::TextureParameters parameters;
  pt[0].reset(new aergia::ProceduralTexture(attributes, parameters));
  pt[1].reset(new aergia::ProceduralTexture(attributes, parameters));
  aergia::ShaderManager &sm = aergia::ShaderManager::instance();
  mix.reset(new aergia::Shader(static_cast<GLuint>(sm.loadFromTexts(AERGIA_NO_VAO_VS, nullptr, screen_source))));
}

void ParticleSystem::update(ponos::Transform transform) {
  render->setUniform("proj", ponos::transpose(transform.matrix()));
  solve();
  pt[curBuffer % 2]->render([&] {
    pt[(curBuffer + 1) % 2]->bind(GL_TEXTURE0);
    mix->begin();
    mix->setUniform("opacity", 0.9996f);
    mix->setUniform("tex", 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    mix->end();
    draw();
  });
  pt[(curBuffer + 1) % 2]->render([&] {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    pt[curBuffer % 2]->bind(GL_TEXTURE0);
    mix->begin();
    mix->setUniform("opacity", 1.f);
    mix->setUniform("tex", 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    mix->end();
    glDisable(GL_BLEND);
  });
  // swap textures
  curBuffer++;
}

bool ParticleSystem::solve() {
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_STENCIL_TEST);
  velocity->bindX(GL_TEXTURE0);
  velocity->bindY(GL_TEXTURE1);
  shader->setUniform("vx", 0);
  shader->setUniform("vy", 1);
  shader->setUniform("resolution", resolution);
  static ponos::HaltonSequence rng(3);
  shader->setUniform("seed", rng.randomFloat());
  return shader->compute();
}

void ParticleSystem::draw() {
  velocity->bindX(GL_TEXTURE0);
  velocity->bindY(GL_TEXTURE1);
  colorRamp->bind(GL_TEXTURE2);
  render->setUniform("tex", 2);
  render->begin();
  pb->bind();
  // aergia::glColor(aergia::COLOR_WHITE);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
  glDrawArrays(GL_POINTS, 0, particleCount);
  render->end();
}

void ParticleSystem::bindTexure(GLenum t) {
  pt[curBuffer % 2]->bind(t);
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
  // color ramp
  aergia::TextureAttributes attributes;
  attributes.target = GL_TEXTURE_2D;
  attributes.width = 100;
  attributes.height = 1;
  attributes.type = GL_UNSIGNED_BYTE;
  attributes.internalFormat = GL_RGBA;
  attributes.format = GL_RGBA;
  attributes.data = new unsigned char[100 * 4];
  aergia::ColorPalette palette = aergia::HEAT_MATLAB_PALETTE;
  aergia::TextureParameters parameters;
  for(int i  =0; i < 100; i++) {
    auto c = palette(i / 100.f, 1.f);
    static_cast<unsigned char*>(attributes.data)[4 * i + 0] = static_cast<unsigned char>(c.r * 255.0);
    static_cast<unsigned char*>(attributes.data)[4 * i + 1] = static_cast<unsigned char>(c.g * 255.0);
    static_cast<unsigned char*>(attributes.data)[4 * i + 2] = static_cast<unsigned char>(c.b * 255.0);
    static_cast<unsigned char*>(attributes.data)[4 * i + 3] = static_cast<unsigned char>(c.a * 255.0);
  }
  colorRamp.reset(new aergia::Texture(attributes, parameters));
  aergia::ShaderManager & sm = aergia::ShaderManager::instance();
  render.reset(new aergia::Shader(static_cast<GLuint>(sm.loadFromTexts(render_vs, nullptr, render_fs))));
  render->setUniform("maxV", velocity->maxVelocity());
  render->setUniform("minV", velocity->minVelocity());
  render->setUniform("vx", 0);
  render->setUniform("vy", 1);
  render->setUniform("tex", 2);
  std::cout << "min max " << velocity->minVelocity() << " " << velocity->maxVelocity() << std::endl;
}
