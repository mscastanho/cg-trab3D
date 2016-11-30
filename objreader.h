#ifndef OBJREADER_H_
#define OBJREADER_H_

#include <string>
#include <map>

using namespace std;

typedef struct object {
    Point* vertices;
    Point* normals;
    Point* texcoord;
    int nVertices;

}Object;

typedef struct material{
  char name[32];
  Point ambient;
  Point diffuse;
  Point specular;
  char fileName[32];
}Material;

map<string,Material>* readMTLFile(string filePath);

#endif
