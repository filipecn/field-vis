#ifndef FIELD_VIS_PROCEDURAL_FIELD_H
#define FIELD_VIS_PROCEDURAL_FIELD_H

#include <aergia/aergia.h>

class ProceduralField {
public:
  ProceduralField(unsigned w,
                  unsigned h,
                  const std::function<float(unsigned, unsigned)> &x,
                  const std::function<float(unsigned, unsigned)> &y);
  void bindX(GLenum target) const;
  void bindY(GLenum target) const;
  ponos::vec2 resolution() const;
  float maxVelocity() const;
  float minVelocity() const;
private:
  float maxV, minV;
  ponos::ivec2 res;
  std::shared_ptr<aergia::Texture> xText, yText;
};

#endif //FIELD_VIS_PROCEDURAL_FIELD_H
