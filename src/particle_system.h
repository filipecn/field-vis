#ifndef FIELD_VIS_PARTICLE_SYSTEM_TEXTURE_H
#define FIELD_VIS_PARTICLE_SYSTEM_TEXTURE_H

#include <aergia/graphics/compute_shader.h>
#include <aergia/io/storage_buffer.h>

class ParticleSystem {
public:
  explicit ParticleSystem(unsigned n);
  bool solve();
  void draw();
private:
  unsigned particleCount;
  std::shared_ptr<aergia::ComputeShader> shader;
  struct Vec2 {
    GLfloat x, y;
  };
  std::vector<Vec2> positions;
  std::vector<float> lifeSpan;
  std::shared_ptr<aergia::StorageBuffer> pBuffer, lBuffer;
  // rendering
  std::shared_ptr<aergia::VertexBuffer> pb;
};

#endif //FIELD_VIS_PARTICLE_SYSTEM_TEXTURE_H
