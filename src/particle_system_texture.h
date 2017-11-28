#ifndef FIELD_VIS_PARTICLE_SYSTEM_TEXTURE_H
#define FIELD_VIS_PARTICLE_SYSTEM_TEXTURE_H

#include <aergia/graphics/compute_shader.h>
#include <aergia/io/storage_buffer.h>

class ParticleSystemTexture {
public:
  explicit ParticleSystemTexture(unsigned n);
  bool solve();
private:
  unsigned particleCount;
  std::shared_ptr<aergia::ComputeShader> shader;
  struct Vec2 {
    GLfloat x, y;
  };
  std::vector<Vec2> positions;
  std::vector<float> lifeSpan;
  std::shared_ptr<aergia::StorageBuffer> pBuffer, lBuffer;
};

#endif //FIELD_VIS_PARTICLE_SYSTEM_TEXTURE_H
