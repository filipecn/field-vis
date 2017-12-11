#ifndef FIELD_VIS_LIC_H
#define FIELD_VIS_LIC_H

#include "procedural_field.h"

class LIC {
public:
  LIC(ProceduralField *v);
  void bindTexture(GLuint t);
  void solve();
  int minNumberOfHits;
private:
  ponos::ivec2 resolution;
  std::shared_ptr<ProceduralField> velocity;
  std::shared_ptr<aergia::Texture> texture, output;
  std::shared_ptr<aergia::ComputeShader> lic;

  const char *lic_source =
      "#version 430\n" \
          "layout (local_size_x = 1, local_size_y = 1) in;"\
          "layout(rgba32f, binding = 0) uniform image2D img_output;"
          "uniform sampler2D vx;"
          "uniform sampler2D vy;"
          "uniform sampler2D tex;"
          "uniform vec2 resolution;"
          "void main()\n"\
          "{\n"\
          "vec4 pixel = vec4(1.0, 0.0, 0.0, 1.0);"
          "ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);"
          "vec2 pos = pixel_coords;"
          "vec3 num = vec3(0);"
          "for(int i = 0; i < 10; i++) {"
          "vec2 uv = pos / resolution;"
          "vec2 v = normalize(vec2(texture(vx, uv).r, texture(vy, uv).r));"
          "num += texture(tex, uv).rgb;"
          "pos += v;"
          "}"
          "pos = pixel_coords;"
          "for(int i = 0; i < 10; i++) {"
          "vec2 uv = pos / resolution;"
          "vec2 v = normalize(vec2(texture(vx, uv).r, texture(vy, uv).r));"
          "num += texture(tex, uv).rgb;"
          "pos -= v;"
          "}"
          " num /= 20;"
          "pixel = texelFetch(tex, pixel_coords, 0);"
          "pixel = vec4(num, 1);"
          "imageStore(img_output, pixel_coords, pixel);\n"\
          "};\n";
};

#endif //FIELD_VIS_LIC_H
