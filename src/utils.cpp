// code extraccted from http://www.csc.kth.se/~weinkauf/notes/amiramesh.html
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

const char *FindAndJump(const char *buffer, const char *SearchString) {
  const char *FoundLoc = strstr(buffer, SearchString);
  if (FoundLoc)
    return FoundLoc + strlen(SearchString);
  return buffer;
}

float* readAMFile(const char *filename, float &dx, float &dy, ponos::ivec3& d) {
  float *data = nullptr;
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    printf("Could not find %s\n", filename);
    return nullptr;
  }

  printf("Reading %s\n", filename);

  //We read the first 2k bytes into memory to parse the header.
  //The fixed buffer size looks a bit like a hack, and it is one, but it gets the job done.
  char buffer[2048];
  fread(buffer, sizeof(char), 2047, fp);
  buffer[2047] = '\0'; //The following string routines prefer null-terminated strings

  if (!strstr(buffer, "# AmiraMesh BINARY-LITTLE-ENDIAN 2.1")) {
    printf("Not a proper AmiraMesh file.\n");
    fclose(fp);
    return nullptr;
  }

  //Find the Lattice definition, i.e., the dimensions of the uniform grid
  int xDim(0), yDim(0), zDim(0);
  sscanf(FindAndJump(buffer, "define Lattice"), "%d %d %d", &xDim, &yDim, &zDim);
  printf("\tGrid Dimensions: %d %d %d\n", xDim, yDim, zDim);
  d[0] = xDim;
  d[1] = yDim;
  d[2] = zDim;

  //Find the BoundingBox
  float xmin(1.0f), ymin(1.0f), zmin(1.0f);
  float xmax(-1.0f), ymax(-1.0f), zmax(-1.0f);
  sscanf(FindAndJump(buffer, "BoundingBox"), "%g %g %g %g %g %g", &xmin, &xmax, &ymin, &ymax, &zmin, &zmax);
  printf("\tBoundingBox in x-Direction: [%g ... %g]\n", xmin, xmax);
  printf("\tBoundingBox in y-Direction: [%g ... %g]\n", ymin, ymax);
  printf("\tBoundingBox in z-Direction: [%g ... %g]\n", zmin, zmax);
  dx = (xmax -xmin) / xDim;
  dy = (ymax -ymin) / yDim;
  //Is it a uniform grid? We need this only for the sanity check below.
  const bool bIsUniform = (strstr(buffer, "CoordType \"uniform\"") != NULL);
  printf("\tGridType: %s\n", bIsUniform ? "uniform" : "UNKNOWN");

  //Type of the field: scalar, vector
  int NumComponents(0);
  if (strstr(buffer, "Lattice { float Data }")) {
    //Scalar field
    NumComponents = 1;
  } else {
    //A field with more than one component, i.e., a vector field
    sscanf(FindAndJump(buffer, "Lattice { float["), "%d", &NumComponents);
  }
  printf("\tNumber of Components: %d\n", NumComponents);

  //Sanity check
  if (xDim <= 0 || yDim <= 0 || zDim <= 0
      || xmin > xmax || ymin > ymax || zmin > zmax
      || !bIsUniform || NumComponents <= 0) {
    printf("Something went wrong\n");
    fclose(fp);
    return nullptr;
  }

  //Find the beginning of the data section
  const long idxStartData = strstr(buffer, "# Data section follows") - buffer;
  if (idxStartData > 0) {
    //Set the file pointer to the beginning of "# Data section follows"
    fseek(fp, idxStartData, SEEK_SET);
    //Consume this line, which is "# Data section follows"
    fgets(buffer, 2047, fp);
    //Consume the next line, which is "@1"
    fgets(buffer, 2047, fp);

    //Read the data
    // - how much to read
    const size_t NumToRead = xDim * yDim * zDim * NumComponents;
    // - prepare memory; use malloc() if you're using pure C
    data = new float[NumToRead];
    if (data) {
      // - do it
      const size_t ActRead = fread((void *) data, sizeof(float), NumToRead, fp);
      // - ok?
      if (NumToRead != ActRead) {
        printf("Something went wrong while reading the binary data section.\nPremature end of file?\n");
        delete[] data;
        fclose(fp);
        return nullptr;
      }

      //Test: Print all data values
      //Note: Data runs x-fastest, i.e., the loop over the x-axis is the innermost
      printf("\nPrinting all values in the same order in which they are in memory:\n");
      int Idx(0);
      for (int k = 0; k < zDim; k++) {
        for (int j = 0; j < yDim; j++) {
          for (int i = 0; i < xDim; i++) {
            //Note: Random access to the value (of the first component) of the grid point (i,j,k):
            // pData[((k * yDim + j) * xDim + i) * NumComponents]
            assert(data[((k * yDim + j) * xDim + i) * NumComponents] == data[Idx * NumComponents]);

            /*for (int c = 0; c < NumComponents; c++) {
              printf("%g ", data[Idx * NumComponents + c]);
            }
            printf("\n");*/
            Idx++;
          }
        }
      }
    }
  }
  printf("data complete.");

  fclose(fp);
  return data;
}
