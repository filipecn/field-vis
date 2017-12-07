#ifndef FIELD_VIS_PARTICLE_SYSTEM_TEXTURE_H
#define FIELD_VIS_PARTICLE_SYSTEM_TEXTURE_H

#include <aergia/graphics/compute_shader.h>
#include <aergia/io/storage_buffer.h>
#include "procedural_field.h"

class ParticleSystem {
public:
  explicit ParticleSystem(unsigned n, ProceduralField *p);
  explicit ParticleSystem(unsigned n, ponos::vec2 res);
  bool solve();
  void draw();
private:
  void init();
  unsigned particleCount;
  std::shared_ptr<aergia::ComputeShader> shader;
  std::shared_ptr<ProceduralField> velocity;
  struct Vec2 {
    GLfloat x, y;
  };
  std::vector<Vec2> positions;
  std::vector<float> lifeSpan;
  ponos::vec2 resolution;
  std::shared_ptr<aergia::StorageBuffer> pBuffer, lBuffer;
  // rendering
  std::shared_ptr<aergia::VertexBuffer> pb;

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
        "uniform vec2 resolution;"
        "highp float rand(vec2 co)\n"
        "{\n"
        " highp float a = 12.9898;\n"
        " highp float b = 78.233;\n"
        " highp float c = 43758.5453;\n"
        " highp float dt= dot(co.xy ,vec2(a,b));\n"
        " highp float sn= mod(dt,3.14);\n"
        " return fract(sin(sn) * c);\n"
        "}"
        "float velocity(sampler2D tex, const vec2 uv) {\n"
        "    vec2 px = 1.0 / resolution;\n"
        "    vec2 vc = (floor(uv * resolution)) * px;\n"
        "    vec2 f = fract(uv * resolution);\n"
        "    float tl = texture(tex, vc).r;\n"
        "    float tr = texture(tex, vc + vec2(px.x, 0)).r;\n"
        "    float bl = texture(tex, vc + vec2(0, px.y)).r;\n"
        "    float br = texture(tex, vc + px).r;\n"
        "    return mix(mix(tl, tr, f.x), mix(bl, br, f.x), f.y);\n"
        "}"
        "void main()\n"\
          "{\n"\
          "uint index = gl_GlobalInvocationID.x;\n"\
          "vec2 p = positions[index];\n"\
          "float l = lifes[index];\n"\
          "if(l <= 0) {\n"\
          " p = vec2(rand(vec2(0.5,0.5) * index), rand(vec2(1,1) * index));"
        " l = 10.0;"\
          "}"\
          "lifes[index] = l - 0.005;"\
          "positions[index] = p + 0.0001 * vec2(texture(vx, p).r,texture(vy, p).r);\n"
        // "positions[index] = p + 0.001 * vec2(velocity(vx, p),velocity(vy, p));\n"
        "};\n";
};

#endif //FIELD_VIS_PARTICLE_SYSTEM_TEXTURE_H
