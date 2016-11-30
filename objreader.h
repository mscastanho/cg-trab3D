#ifndef OBJREADER_H_
#define OBJREADER_H_

#include <string>
#include <map>
#include "misc.h"

using namespace std;

typedef struct material{
  char name[32];
  Point ambient;
  Point diffuse;
  Point specular;
  char fileName[32];
}Material;

typedef struct face {
  int vertices[3];
  int normals[3];
  int texels[3];
}Face;

typedef struct texel{
  float u;
  float v;
  Material* m;
}Texel;

typedef struct object {
    Point* vertices;
    Point* normals;
    Texel* texels;
    Face* faces;
    int nVertices;
    int nNormals;
    int nTexels;
    int nFaces;
}Object;



map<string,Material>* readMTLFile(string filePath);

Object* readOBJFile (string filePath);

#endif
