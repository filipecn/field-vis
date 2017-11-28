#include <aergia/aergia.h>
#include "particle_system_texture.h"
#include "procedural_field.h"

int main() {
  aergia::SceneApp<> app(512, 512, "Field Vis 2D");
  app.init();
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
  ParticleSystemTexture particleSystem(100000);
  field.bindX(GL_TEXTURE0);
  field.bindY(GL_TEXTURE1);
  particleSystem.solve();
  aergia::ShaderManager &sm = aergia::ShaderManager::instance();
  aergia::Shader s2(static_cast<GLuint>(sm.loadFromTexts(AERGIA_NO_VAO_VS, nullptr, AERGIA_TEX_FS)));
  app.renderCallback = [&]() {
    field.bindY(GL_TEXTURE0);
    s2.begin();
    s2.setUniform("tex", 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    s2.end();
  };
  app.run();
  return 0;
}
