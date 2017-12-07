#ifndef FIELD_VIS_UTILS_H
#define FIELD_VIS_UTILS_H

#include <ponos/geometry/vector.h>
/** Find a string in the given buffer and return a pointer
    to the contents directly behind the SearchString.
    If not found, return the buffer. A subsequent sscanf()
    will fail then, but at least we return a decent pointer.
*/
const char* FindAndJump(const char* buffer, const char* SearchString);

float* readAMFile(const char* filename, float &dx, float &dy, ponos::ivec3& d);

#endif //FIELD_VIS_UTILS_H
