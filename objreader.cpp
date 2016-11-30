#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include "geometry.h"
#include "misc.h"
#include "objreader.h"

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
            strcpy(m.fileName,token);
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

  map<string,Material>* materials = NULL;

  input.open(filePath.c_str(),ifstream::in);

  if(input.good()){

    // First read to get the number of items
    while( !input.eof() ){
      char buffer[100];
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
        //  currentMaterial = &materials->at(string(token));
      }else if(!strcmp(token,"mtllib")){
        token = strtok_r(NULL," ",&savePtr1);
        cout << "nome do arquivo: " << token << endl;
        string file = "./" + string(token);
        cout << file << endl;
        materials = readMTLFile(file);

        cout << "tamanho: " << materials->size() << endl;;
        map<string,Material>::iterator it = materials->begin();

			  while(it != materials->end()){

			    cout << it->first << ":" << endl;
			    Material m1 = it->second;
			    cout << m1.name << endl;
			    cout << "ambient "; printPoint(m1.ambient);
			    cout << "diffuse "; printPoint(m1.diffuse);
			    cout << "specular "; printPoint(m1.specular);
			    cout << "filename " << m1.fileName << endl << endl;

			    it++;
			  }
      }else if(!strcmp(token,"g")){

      }
    }
  }

  input.close();

  return o;
}
