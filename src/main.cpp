#include <aergia/aergia.h>
#include "particle_system.h"
#include "procedural_field.h"
#include <unistd.h>
#include "utils.h"
#ifdef WIN32
#include <windows.h>
#endif

#define WIDTH 512
#define HEIGHT 512

const char *vs = "#version 440 core\n"
    "in vec2 position;"
    "in vec2 texcoord;"
    "uniform mat4 proj;"
    "out vec2 texCoord;"
    "void main() {"
    "   texCoord = texcoord;"
    "   gl_Position = proj * vec4(position, 0.0, 1.0);"
    "}";

const char *screen_source = "#version 440 core\n"\
    "in vec2 texCoord;"\
    "out vec4 outColor;"\
    "uniform sampler2D tex;"\
    "uniform float opacity;"\
    "void main() {"\
    "   vec4 color = texture(tex, texCoord);"
    "   outColor = vec4(floor(255.0 * color * opacity) / 255.0);"\
"}";

void dump(aergia::TextureAttributes attributes, aergia::ProceduralTexture *pt) {
  return;
  int width = attributes.width;
  int height = attributes.height;

  unsigned char *data = NULL;

  data = new unsigned char[(int) (width * height) * 4];

  memset(data, 0, width * height * 4);

  glActiveTexture(GL_TEXTURE0);
  pt->bind(GL_TEXTURE0);
  glGetTexImage(attributes.target, 0, attributes.format,
                attributes.type, data);

  aergia::CHECK_GL_ERRORS;

  std::cout << width << " " << height << std::endl;

  for (int j(/*height - 1*/5); j >= 0; --j) {
    for (int i(0); i < 10 /*width*/; ++i) {
      std::cout << "(" <<
                (int) data[(int) (j * (width * 4) + i * 4 + 0)] << "," <<
                (int) data[(int) (j * (width * 4) + i * 4 + 1)] << "," <<
                (int) data[(int) (j * (width * 4) + i * 4 + 2)] << "," <<
                (int) data[(int) (j * (width * 4) + i * 4 + 3)] << ") ";
    }
    std::cout << std::endl;
  }
}

int main() {
  unsigned W = WIDTH, H = HEIGHT;
  float dx, dy;
  ponos::ivec3 dimensions;
#ifdef WIN32
  float *data = readAMFile("C:/Cylinder2D.am", dx, dy, dimensions);
#else
  float *data = readAMFile("/mnt/windows/Cylinder2D.am", dx, dy, dimensions);
#endif
  aergia::SceneApp<> app(W, H, "Field Vis 2D", false);
  app.init();
  app.addViewport2D(0, 0, W, H);
  app.getCamera<aergia::Camera2D>(0)->fit(ponos::make_unit_bbox2D(), 1.2f);
  ProceduralField *field = nullptr;
  if (data == nullptr) {
    field = new ProceduralField(512,
                                512,
                                [&](unsigned x, unsigned y) -> float {
                                  if (y == H / 2 && x == W / 2)
                                    return 0.;
                                  float _x = static_cast<float>(x) - W / 2;
                                  float _y = static_cast<float>(y) - H / 2;
                                  ponos::vec2 normal(2 * _x, 2 * _y);
                                  return -normal.y / normal.length();
                                },
                                [&](unsigned x, unsigned y) -> float {
                                  if (y == H / 2 && x == W / 2)
                                    return 0.;
                                  float _x = static_cast<float>(x) - W / 2;
                                  float _y = static_cast<float>(y) - H / 2;
                                  ponos::vec2 normal(2 * _x, 2 * _y);
                                  return normal.x / normal.length();
                                });
  } else {
    int z = 0;
    W = static_cast<unsigned int>(dimensions[0]);
    H = static_cast<unsigned int>(dimensions[1]);
    z = dimensions[2] - 1;
    field = new ProceduralField(W,
                                H,
                                [&](unsigned x, unsigned y) -> float {
                                  return data[z * W * H * 2 + y * W * 2 + x * 2 + 0];
                                },
                                [&](unsigned x, unsigned y) -> float {
                                  return data[z * W * H * 2 + y * W * 2 + x * 2 + 1];
                                });
  }
  ParticleSystem particleSystem(1280000, field);
  W = H = 512;
  aergia::TextureAttributes attributes;
  attributes.target = GL_TEXTURE_2D;
  attributes.width = W;
  attributes.height = H;
  attributes.type = GL_UNSIGNED_BYTE;
  attributes.internalFormat = GL_RGBA;
  attributes.format = GL_RGBA;
  aergia::TextureParameters parameters;
  aergia::ProceduralTexture *pt[2];
  pt[0] = new aergia::ProceduralTexture(attributes, parameters);
  pt[1] = new aergia::ProceduralTexture(attributes, parameters);
  aergia::ShaderManager &sm = aergia::ShaderManager::instance();
  aergia::Shader screen(static_cast<GLuint>(sm.loadFromTexts(AERGIA_NO_VAO_VS, nullptr, AERGIA_TEX_FS)));
  aergia::Shader mix(static_cast<GLuint>(sm.loadFromTexts(AERGIA_NO_VAO_VS, nullptr, screen_source)));
  unsigned int curBuffer = 0;
  UNUSED_VARIABLE(curBuffer);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_STENCIL_TEST);
  field->bindX(GL_TEXTURE0);
  field->bindY(GL_TEXTURE1);
  particleSystem.solve();

  ponos::Timer timer;
  app.viewports[0].renderCallback = [&]() {
    auto elapsed = timer.tackTick();
    if (elapsed < 1000.0 / 60.0)
      sleep((1000.0 / 60.0 - elapsed) / 1000.0);
    pt[curBuffer % 2]->render([&] {
      pt[(curBuffer + 1) % 2]->bind(GL_TEXTURE0);
      mix.begin();
      mix.setUniform("opacity", 0.996f);
      mix.setUniform("tex", 0);
      glDrawArrays(GL_TRIANGLES, 0, 3);
      mix.end();
      particleSystem.draw();
    });
    dump(attributes, pt[curBuffer % 2]);
    pt[(curBuffer + 1) % 2]->render([&] {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      pt[curBuffer % 2]->bind(GL_TEXTURE0);
      mix.begin();
      mix.setUniform("opacity", 1.f);
      mix.setUniform("tex", 0);
      glDrawArrays(GL_TRIANGLES, 0, 3);
      mix.end();
      glDisable(GL_BLEND);
    });
    // render to screen
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    pt[(curBuffer + 1) % 2]->bind(GL_TEXTURE0);
    screen.begin();
    screen.setUniform("tex", 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    screen.end();
    glDisable(GL_BLEND);
    aergia::CHECK_GL_ERRORS;
    // update particles
    field->bindX(GL_TEXTURE0);
    field->bindY(GL_TEXTURE1);
    particleSystem.solve();
    // swap textures
    curBuffer++;
  };
  app.run();
  if (data)
    delete[] data;
  return 0;
}
