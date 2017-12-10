#include "procedural_field.h"

ProceduralField::ProceduralField(unsigned w,
                                 unsigned h,
                                 const std::function<float(unsigned, unsigned)> &x,
                                 const std::function<float(unsigned, unsigned)> &y) {
  maxV = 0.f, minV = INFINITY;
  res[0] = w;
  res[1] = h;
  aergia::TextureAttributes attributesX;
  attributesX.target = GL_TEXTURE_2D;
  attributesX.width = w;
  attributesX.height = h;
  attributesX.type = GL_FLOAT;
  attributesX.internalFormat = GL_R32F;
  attributesX.format = GL_RED;
  attributesX.data = new float[w * h];
  aergia::TextureAttributes attributesY;
  attributesY.target = GL_TEXTURE_2D;
  attributesY.width = w;
  attributesY.height = h;
  attributesY.type = GL_FLOAT;
  attributesY.internalFormat = GL_R32F;
  attributesY.format = GL_RED;
  attributesY.data = new float[w * h];
  for (unsigned i = 0; i < h; i++)
    for (unsigned j = 0; j < w; j++) {
      auto v = ponos::vec2(x(j, i), y(j, i)).length();
      static_cast<float *>(attributesY.data)[i * w + j] = y(j, i);
      static_cast<float *>(attributesX.data)[i * w + j] = x(j, i);
      minV = std::min(minV, v);
      maxV = std::max(maxV, v);
    }
  aergia::TextureParameters parametersX;
  xText.reset(new aergia::Texture(attributesX, parametersX));
  aergia::TextureParameters parametersY;
  yText.reset(new aergia::Texture(attributesY, parametersY));
}

void ProceduralField::bindX(GLenum target) const {
  xText->bind(target);
}

void ProceduralField::bindY(GLenum target) const {
  yText->bind(target);
}
ponos::vec2 ProceduralField::resolution() const {
  return ponos::vec2(res[0], res[1]);
}
float ProceduralField::maxVelocity() const {
  return maxV;
}
float ProceduralField::minVelocity() const {
  return minV;
}
