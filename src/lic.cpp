#include "lic.h"
#include <chrono>
#include <random>

LIC::LIC(ProceduralField *v) {
  velocity.reset(v);
  unsigned int width = static_cast<unsigned int>(velocity->resolution().x);
  unsigned int height = static_cast<unsigned int>(velocity->resolution().y);
  resolution[0] = width;
  resolution[1] = height;
  {
    aergia::TextureAttributes attributes;
    attributes.target = GL_TEXTURE_2D;
    attributes.width = width;
    attributes.height = height;
    attributes.type = GL_FLOAT;
    attributes.internalFormat = GL_RGBA32F;
    attributes.format = GL_RGBA;
    attributes.depth = 0;
    attributes.data = nullptr;
    aergia::TextureParameters parameters;
    output.reset(new aergia::Texture(attributes, parameters));
  }
  {
    aergia::TextureAttributes attributes;
    attributes.target = GL_TEXTURE_2D;
    attributes.width = width;
    attributes.height = height;
    attributes.type = GL_UNSIGNED_BYTE;
    attributes.internalFormat = GL_RGBA;
    attributes.format = GL_RGBA;
    attributes.depth = 0;
    attributes.data = new unsigned char[attributes.width * attributes.height * 4];
    aergia::TextureParameters parameters;
    ponos::HaltonSequence rng(3);
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    std::normal_distribution<double> distribution (0.0,1.0);
    for (unsigned int i = 0; i < attributes.width; i++)
      for (unsigned int j = 0; j < attributes.height; j++) {
        static_cast<unsigned char *>(attributes.data)[(j * attributes.width + i) * 4 + 0] =
        static_cast<unsigned char *>(attributes.data)[(j * attributes.width + i) * 4 + 1] =
        static_cast<unsigned char *>(attributes.data)[(j * attributes.width + i) * 4 + 2] =
        static_cast<unsigned char>(distribution(generator) * 255.0);

        static_cast<unsigned char *>(attributes.data)[(j * attributes.width + i) * 4 + 3] = 255;
      }
    texture.reset(new aergia::Texture(attributes, parameters));
  }
  lic.reset(new aergia::ComputeShader(lic_source));
  lic->setGroupSize(ponos::uivec3(width, height, 1));
}

void LIC::bindTexture(GLuint t) {
  output->bind(t);
}

void LIC::solve() {
  output->bindImage(GL_TEXTURE0);
  velocity->bindX(GL_TEXTURE1);
  velocity->bindY(GL_TEXTURE2);
  texture->bind(GL_TEXTURE3);
  lic->setUniform("vx", 1);
  lic->setUniform("vy", 2);
  lic->setUniform("tex", 3);
  lic->setUniform("resolution", ponos::vec2(resolution[0], resolution[1]));
  lic->compute();
}
