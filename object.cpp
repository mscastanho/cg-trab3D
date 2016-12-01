#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <map>
#include "geometry.h"
#include "misc.h"
#include "object.h"
#include "imageloader.h"

using namespace std;

map<string,Material>* readMTLFile(string filePath){
  string line;
  ifstream input;
  Material m;
  char type[8];
  map<string,Material> *materials = new map<string,Material>();

  input.open(filePath.c_str(),ifstream::in);

  if(input.good()){
    while( !input.eof() ){
      char buffer[100];
      input.getline(buffer,100);

      if(strlen(buffer) == 0)
        continue;

      char* token = strtok(buffer," ");

      if(!strcmp(token,"newmtl")){
        token = strtok(NULL," ");
        strcpy(m.name,token);

        for(int i = 0; i < 3 ; i++){
          input.getline(buffer,100);

          token = strtok(buffer," ");
          strcpy(type,token);

          token = strtok(NULL," ");
          double x = atof(token);

          token = strtok(NULL," ");
          double y = atof(token);

          token = strtok(NULL," ");
          double z = atof(token);

          Point p;
          p.x = x;
          p.y = y;
          p.z = z;

          if(!strcmp(type,"Ka"))
            m.ambient = p;
          else if(!strcmp(type,"Kd"))
            m.diffuse = p;
          else if(!strcmp(type,"Ks"))
            m.specular = p;
        }

        input.getline(buffer,100);
        if(strlen(buffer) != 0){
          token = strtok(buffer," ");

          if(!strcmp(token,"map_Kd")){
            token = strtok(NULL," ");
            token[strlen(token)-1] = '\0';
            strcpy(m.fileName,token);

            m.texture = LoadTextureRAW(m.fileName);
            printf("name: %s texture: %d\n",m.fileName,m.texture);
          }
        }

        materials->insert(make_pair(string(m.name),m));
      }

    }
  }else{
    cout << "TRETA!!!" << endl;
  }

  input.close();
  return materials;

}

Object* readOBJFile (string filePath){
  string line;
  ifstream input;
  Object *o = NULL;
  char *savePtr1, *savePtr2;
  int nVertices = 0;
  int nNormals = 0;
  int ntexels = 0;
  int nFaces = 0;
  char buffer[128];
  char mtlFile[128];

  map<string,Material>* materials = NULL;

  // MTL file has to have the same name as obj file
  strcpy(mtlFile,filePath.c_str());
  int length = strlen(mtlFile);
  mtlFile[length-3] = 'm';
  mtlFile[length-2] = 't';
  mtlFile[length-1] = 'l';

  materials = readMTLFile(string(mtlFile));

  input.open(filePath.c_str(),ifstream::in);

  if(input.good()){

    // First read to get the number of items
    while( !input.eof() ){

      input.getline(buffer,100);

      if(strlen(buffer) == 0)
        continue;

      char* token = strtok(buffer," ");

      if(!strcmp(token,"v")){
        nVertices++;
      }else if(!strcmp(token,"vt")){
        ntexels++;
      }else if(!strcmp(token,"vn")){
        nNormals++;
      }else if(!strcmp(token,"f")){
        nFaces++;
      }
    }

    input.clear();
    input.seekg(0);

    //Initialize object structure

    o = (Object*) malloc(sizeof(Object));
    o->vertices = (Point*) malloc (nVertices*sizeof(Point));
    o->normals = (Point*) malloc (nNormals*sizeof(Point));
    o->texels = (Texel*) malloc (ntexels*sizeof(Texel));
    o->faces = (Face*) malloc (nFaces*sizeof(Face));
    o->nVertices = nVertices;
    o->nNormals = nNormals;
    o->nTexels = ntexels;
    o->nFaces = nFaces;
    o->materials = materials;

    int nVerticesRead = 0;
    int nNormalsRead = 0;
    int ntexelsRead = 0;
    int nFacesRead = 0;

    Material* currentMaterial = NULL;

    // Second read to get all values
    while( !input.eof() ){
      char buffer[100];
      input.getline(buffer,100);

      if(strlen(buffer) == 0)
        continue;

      char* token = strtok_r(buffer," ",&savePtr1);

      if(!strcmp(token,"v")){

        token = strtok_r(NULL," ",&savePtr1);
        o->vertices[nVerticesRead].x = atof(token);

        token = strtok_r(NULL," ",&savePtr1);
        o->vertices[nVerticesRead].y = atof(token);

        token = strtok_r(NULL," ",&savePtr1);
        o->vertices[nVerticesRead].z = atof(token);

        nVerticesRead++;

      }else if(!strcmp(token,"vt")){

        token = strtok_r(NULL," ",&savePtr1);
        o->texels[ntexelsRead].u = atof(token);

        token = strtok_r(NULL," ",&savePtr1);
        o->texels[ntexelsRead].v = atof(token);

        o->texels[ntexelsRead].m = currentMaterial;

        ntexelsRead++;

      }else if(!strcmp(token,"vn")){

        token = strtok_r(NULL," ",&savePtr1);
        o->normals[nNormalsRead].x = atof(token);

        token = strtok_r(NULL," ",&savePtr1);
        o->normals[nNormalsRead].y = atof(token);

        token = strtok_r(NULL," ",&savePtr1);
        o->normals[nNormalsRead].z = atof(token);

        nNormalsRead++;

      }else if(!strcmp(token,"f")){

        char* subtoken;
        char face_buffer[64];

        for(int i = 0 ; i < 3 ; i++ ){
          token = strtok_r(NULL," ",&savePtr1);
          strcpy(face_buffer,token);


          subtoken = strtok_r(face_buffer,"/",&savePtr2);
          // -1 is neede because vertex indices start at 1 in .obj files
          o->faces[nFacesRead].vertices[i] = atoi(subtoken)-1;

          subtoken = strtok_r(NULL,"/",&savePtr2);
          o->faces[nFacesRead].texels[i] = atoi(subtoken)-1;

          subtoken = strtok_r(NULL,"/",&savePtr2);
          o->faces[nFacesRead].normals[i] = atoi(subtoken)-1;
        }

        nFacesRead++;

      }else if(!strcmp(token,"usemtl")){
          token = strtok_r(NULL," ",&savePtr1);
          currentMaterial = &materials->at(string(token));
      }else if(!strcmp(token,"mtllib")){
        /*token = strtok_r(NULL," ",&savePtr1);
        cout << "nome do arquivo: " << token << endl;
        materials = readMTLFile(token);

        cout << "readOBJFile: " << endl;
        printMaterialsMap(materials);
*/

      }else if(!strcmp(token,"g")){

      }
    }
  }

  input.close();

  return o;
}

void drawObject(Object* o){

  GLfloat materialEmission[] = { 0.10, 0.10, 0.10, 1};
  GLfloat materialColorA[] = { 0.2, 0.2, 0.2, 1};
  GLfloat materialColorD[] = { 1.0, 1.0, 1.0, 1};
  GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1};
  GLfloat mat_shininess[] = { 100.0 };
  glColor3f(0,0,1);

  for (int i = 0 ; i < o->nFaces ; i++){
    Texel tg = o->texels[o->faces[i].texels[0]];
    GLuint texture = tg.m->texture;

      for(int j = 0 ; j < 3 ; j++){

        Texel t = o->texels[o->faces[i].texels[j]];
        GLuint texture = t.m->texture;

        glBindTexture (GL_TEXTURE_2D, texture);
        glBegin(GL_TRIANGLES);

        glTexCoord2f(t.u,t.v);

        Point v = o->vertices[o->faces[i].vertices[j]];
        glVertex3f(v.x,v.y,v.z);

        Point n = o->normals[o->faces[i].normals[j]];
  			glNormal3f(n.x,n.y,n.z);
      }

      glEnd();
  }

}

void loadTexturesFromMaterials(map<string,Material>* materials){

  map<string,Material>::iterator it = materials->begin();

  while(it != materials->end()){

    Material m = it->second;

    if(strlen(m.fileName) != 0){
        m.texture = LoadTextureRAW(m.fileName);
    }
    it++;
  }

}

void printMaterialsMap(map<string,Material>* materials){
  cout << "tamanho: " << materials->size() << endl;;
  map<string,Material>::iterator it = materials->begin();

  while(it != materials->end()){

    cout << "key: " << it->first << endl;
    Material m1 = it->second;
    cout << m1.name << endl;
    cout << "ambient "; printPoint(m1.ambient);
    cout << "diffuse "; printPoint(m1.diffuse);
    cout << "specular "; printPoint(m1.specular);
    cout << "filename " << m1.fileName << endl;
    cout << "index " << m1.texture << endl;
    it++;
  }
}
