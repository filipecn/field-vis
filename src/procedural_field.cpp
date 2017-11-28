#include "procedural_field.h"

ProceduralField::ProceduralField(unsigned w,
                                 unsigned h,
                                 const std::function<float(unsigned, unsigned)> &x,
                                 const std::function<float(unsigned, unsigned)> &y) {
  {
    aergia::TextureAttributes attributes;
    attributes.target = GL_TEXTURE_2D;
    attributes.width = w;
    attributes.height = h;
    attributes.type = GL_FLOAT;
    attributes.internalFormat = GL_R32F;
    attributes.format = GL_RED;
    attributes.data = new float[w * h];
    for (unsigned i = 0; i < w; i++)
      for (unsigned j = 0; j < h; j++)
        static_cast<float*>(attributes.data)[i * w + j] = x(j, i);
    aergia::TextureParameters parameters;
    xText.reset(new aergia::Texture(attributes, parameters));
  }
  {
    aergia::TextureAttributes attributes;
    attributes.target = GL_TEXTURE_2D;
    attributes.width = w;
    attributes.height = h;
    attributes.type = GL_FLOAT;
    attributes.internalFormat = GL_R32F;
    attributes.format = GL_RED;
    attributes.data = new float[w * h];
    for (unsigned i = 0; i < w; i++)
      for (unsigned j = 0; j < h; j++)
        static_cast<float*>(attributes.data)[i * w + j] = y(j, i);
    aergia::TextureParameters parameters;
    yText.reset(new aergia::Texture(attributes, parameters));
  }
}

void ProceduralField::bindX(GLenum target) const {
  xText->bind(target);
}

void ProceduralField::bindY(GLenum target) const {
  yText->bind(target);
}
