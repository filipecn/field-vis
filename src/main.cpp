#include <aergia/aergia.h>
#include "particle_system.h"
#include "procedural_field.h"
#include <unistd.h>
#include "utils.h"
#ifdef WIN32
#include <windows.h>
#endif

#define WIDTH 1024
#define HEIGHT 1024

const char *screen_fs = "#version 440 core\n"\
    "in vec2 texCoord;"\
    "out vec4 outColor;"\
    "uniform sampler2D tex;"\
    "void main() {"\
    "   vec4 color = texture(tex, texCoord);"
    // "outColor = vec4(color.xyz, floor(255.0 * color.w * opacity) / 255.0);"
    "   outColor = color;"
    "}";

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
    z = dimensions[2] - 100;
    field = new ProceduralField(W,
                                H,
                                [&](unsigned x, unsigned y) -> float {
                                  return data[z * W * H * 2 + y * W * 2 + x * 2 + 0];
                                },
                                [&](unsigned x, unsigned y) -> float {
                                  return data[z * W * H * 2 + y * W * 2 + x * 2 + 1];
                                });
  }
  W = H = 1024;
  ParticleSystem particleSystem(1280000, field, W, H);
  aergia::ShaderManager &sm = aergia::ShaderManager::instance();
  aergia::Shader screen(static_cast<GLuint>(sm.loadFromTexts(AERGIA_NO_VAO_VS, nullptr, screen_fs)));
  ponos::Timer timer;
  app.viewports[0].renderCallback = [&]() {
    auto elapsed = timer.tackTick();
    if (elapsed < 1000.0 / 60.0)
      sleep((1000.0 / 60.0 - elapsed) / 1000.0);
    particleSystem.update(app.getCamera<aergia::Camera2D>(0)->getTransform());
    // render to screen
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    particleSystem.bindTexure(GL_TEXTURE0);
    screen.begin();
    screen.setUniform("tex", 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    screen.end();
    //glDisable(GL_BLEND);
  };
  app.keyCallback = [&](int k, int a) {
    UNUSED_VARIABLE(k);
    UNUSED_VARIABLE(a);
    app.exit();
  };
  app.run();
  if (data)
    delete[] data;
  return 0;
}
