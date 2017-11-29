#include <aergia/aergia.h>
#include "particle_system.h"
#include "procedural_field.h"

#define W 512
#define H 512

const char *mix_source = "#version 440 core\n"\
    "in vec2 texCoord;"\
    "out vec4 outColor;"\
    "uniform sampler2D curTex;"\
    "uniform sampler2D lasTex;"\
    "void main() {"\
    "   outColor = mix(texture(curTex, texCoord), texture(lasTex, texCoord), 0.9);"\
"}";

int main() {
  aergia::SceneApp<> app(W, H, "Field Vis 2D", false);
  app.init();
  app.addViewport2D(0, 0, W, H);
  app.getCamera<aergia::Camera2D>(0)->fit(ponos::make_unit_bbox2D(), 1.2f);
  ProceduralField field(512,
                        512,
                        [](unsigned x, unsigned y) -> float {
                          UNUSED_VARIABLE(x);
                          UNUSED_VARIABLE(y);
                          return 1.f;
                        },
                        [](unsigned x, unsigned y) -> float {
                          UNUSED_VARIABLE(x);
                          UNUSED_VARIABLE(y);
                          return 0.f;
                        });
  ParticleSystem particleSystem(12800);
  aergia::TextureAttributes attributes;
  attributes.target = GL_TEXTURE_2D;
  attributes.width = W;
  attributes.height = H;
  attributes.type = GL_UNSIGNED_BYTE;
  attributes.internalFormat = GL_RGBA8;
  attributes.format = GL_RGBA;
  aergia::TextureParameters parameters;
  aergia::ProceduralTexture* pt[2];
  pt[0] = new aergia::ProceduralTexture(attributes, parameters);
  pt[1] = new aergia::ProceduralTexture(attributes, parameters);
  aergia::ShaderManager &sm = aergia::ShaderManager::instance();
  aergia::Shader screen(static_cast<GLuint>(sm.loadFromTexts(AERGIA_NO_VAO_VS, nullptr, AERGIA_TEX_FS)));
  aergia::Shader mix(static_cast<GLuint>(sm.loadFromTexts(AERGIA_NO_VAO_VS, nullptr, mix_source)));
  unsigned int curBuffer = 0;
  app.viewports[0].renderCallback = [&]() {
    // render new frame
    pt[curBuffer % 2]->render([&]() {
      field.bindX(GL_TEXTURE0);
      field.bindY(GL_TEXTURE1);
      particleSystem.solve();
      particleSystem.draw();
    });
    // mix Frames
    pt[(curBuffer + 1) % 2]->render([&]() {
      pt[curBuffer % 2]->bind(GL_TEXTURE0);
      pt[(curBuffer + 1) % 2]->bind(GL_TEXTURE1);
      mix.begin();
      mix.setUniform("curTex", 0);
      mix.setUniform("lasTex", 1);
      glDrawArrays(GL_TRIANGLES, 0, 3);
      mix.end();
    });
    // render to screen
    pt[(curBuffer + 0) % 2]->bind(GL_TEXTURE0);
    screen.begin();
    screen.setUniform("tex", 0);
    aergia::CHECK_GL_ERRORS;
    glDrawArrays(GL_TRIANGLES, 0, 3);
    screen.end();
    curBuffer++;
  };
  app.run();
  return 0;
}
