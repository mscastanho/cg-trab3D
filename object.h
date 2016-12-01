#ifndef OBJECT_H_
#define OBJECT_H_

#include <string>
#include <map>
#include "misc.h"

using namespace std;

typedef struct material{
  char name[32];
  Point ambient;
  Point diffuse;
  Point specular;
  GLuint texture;
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

typedef struct vertex{
  float x;
  float y;
  float z;
  Material *m;
}Vertex;

typedef struct object {
    Point* vertices;
    Point* normals;
    Texel* texels;
    Face* faces;
    int nVertices;
    int nNormals;
    int nTexels;
    int nFaces;
    map<string,Material>* materials;
}Object;

map<string,Material>* readMTLFile(string filePath);

Object* readOBJFile (string filePath);

void drawObject(Object* o);

void loadTexturesFromMaterials(map<string,Material>* materials);

void printMaterialsMap(map<string,Material>* materials);

#endif
